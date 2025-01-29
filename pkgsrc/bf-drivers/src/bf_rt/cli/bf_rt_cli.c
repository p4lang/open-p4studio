/*******************************************************************************
 *  Copyright (C) 2024 Intel Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions
 *  and limitations under the License.
 *
 *
 *  SPDX-License-Identifier: Apache-2.0
 ******************************************************************************/


#include <Python.h>
#include <stdio.h>
#include <bf_rt/bf_rt_init.h>
#include <bf_rt/bf_rt_common.h>
#include "target-utils/clish/shell.h"
#include <dvm/bf_drv_intf.h>
#include <target-sys/bf_sal/bf_sys_sem.h>
#include <lld/python_shell_mutex.h>
/*
 * bfrt_python is not protected by bfshell's command-level lock.
 * We have a lock defined in python_shell_mutex to ensure multiple
 * instances of bfrt_python aren't spawned. All python shell(bfrt_python and
 * debug_python) share one lock.
 */

/*
 * Start BF Runtime CLI. This program heavily borrows from the python C
 * interface example program.
 */
PyObject *bfrtpModule = NULL;
static int bf_rt_start_cli(int in_fd,
                           int out_fd,
                           const char *install_dir,
                           const char *udf,
                           bool interactive) {
  PyObject *pFunc;
  PyObject *pArgs, *pValue;
  uint32_t array_size = 0;
  bf_dev_id_t *dev_id_list = NULL;
  int ret_val = 0;

  // first run, initialize python interpreter.
  // Use the system-default config and add site-packages modules.
  if (!Py_IsInitialized()) {
    PyConfig config;
    PyConfig_InitPythonConfig(&config);
    wchar_t py_path[256];
    swprintf(py_path, 256, L"%slib/python%d.%d/site-packages", install_dir, PY_MAJOR_VERSION, PY_MINOR_VERSION);
    config.pythonpath_env = py_path;
    Py_InitializeFromConfig(&config);
  }

  bf_rt_num_device_id_list_get(&array_size);
  if (array_size) {
    dev_id_list = bf_sys_malloc(array_size * sizeof(bf_dev_id_t));
    bf_rt_device_id_list_get(dev_id_list);
  }

  if (bfrtpModule == NULL) {
      PyObject *pName;
      pName = PyUnicode_DecodeFSDefault("bfrtcli");
      if (pName == NULL) {
        fprintf(stderr, "Failed to decode module name 'bfrtcli' as a Python string.\n");
        ret_val = 1;
        goto cleanup;
      }

      bfrtpModule = PyImport_Import(pName);
      Py_DECREF(pName);

    if (bfrtpModule == NULL) {
      fprintf(stderr, "Failed to import module 'bfrtcli'.\n");

      if (PyErr_Occurred()) {
          PyErr_Print();
      } else {
          fprintf(stderr, "No additional error information from Python.\n");
      }

      ret_val = 1;
      goto cleanup;
    }
  }


  if (bfrtpModule != NULL) {
    // Create a call to the start_bfrt function in bfrtcli.py
    pFunc = PyObject_GetAttrString(bfrtpModule, "start_bfrt");
    /* pFunc is a new reference */

    if (pFunc && PyCallable_Check(pFunc)) {
      // Use a tuple as our argument list
      if (udf) {
        pArgs = PyTuple_New(6);
      } else {
        pArgs = PyTuple_New(4);
      }
      // Create python objects from c types.
      // Place references to them in the argument tuple.
      pValue = PyLong_FromLong(in_fd);
      PyTuple_SetItem(pArgs, 0, pValue);
      pValue = PyLong_FromLong(out_fd);
      PyTuple_SetItem(pArgs, 1, pValue);
      /*
       * Convert from the filepath c string to a python string using the
       * filesystem's default encoding
       */
      pValue = PyUnicode_DecodeFSDefault(install_dir);
      PyTuple_SetItem(pArgs, 2, pValue);
      PyObject *pyList = PyList_New(array_size);
      for (uint32_t i = 0; i < array_size; i++) {
        pValue = PyLong_FromLong(dev_id_list[i]);
        PyList_SetItem(pyList, i, pValue);
      }
      PyTuple_SetItem(pArgs, 3, pyList);
      if (udf) {
        pValue = PyUnicode_DecodeFSDefault(udf);
        PyTuple_SetItem(pArgs, 4, pValue);
        pValue = PyBool_FromLong(interactive);
        PyTuple_SetItem(pArgs, 5, pValue);
      }

      // Actually make the function call.
      // This is where we block until the CLI exits
      pValue = PyObject_CallObject(pFunc, pArgs);
      Py_DECREF(pArgs);

      // Handle exit codes and decrement references to free our python objects
      if (pValue != NULL) {
        long ret = PyLong_AsLong(pValue);
        if (ret == 0) {
          printf("bf_rt cli exited normally.\n");
        } else {
          printf("bf_rt cli exited with error: %ld\n", ret);
        }
        Py_DECREF(pValue);
      } else {
        Py_DECREF(pFunc);
        PyErr_Print();
        fprintf(stderr, "bf_rt cli python call failed\n");
        ret_val = 1;
        goto cleanup;
      }
    } else {
      if (PyErr_Occurred()) PyErr_Print();
      fprintf(stderr, "Cannot find start_bfrt function.\n");
    }
    Py_XDECREF(pFunc);
  } else {
    PyErr_Print();
    fprintf(stderr, "Failed to load bfrtcli python library\n");
    ret_val = 1;
    goto cleanup;
  }
cleanup:
  // After execution of Py_Fynalize will be needed call
  // Py_Initialize and PyImport_Import that leads to a memory leak
  // because of previous imported lib will not be removed
  PyGC_Collect();
  if (dev_id_list) {
    bf_sys_free(dev_id_list);
  }
  return ret_val;
}

void reg_parse_main();
void reg_set_instream(FILE *);
void reg_set_outstream(FILE *);

// A klish plugin allowing BF Runtime CLI to be started from bfshell.
CLISH_PLUGIN_SYM(bfrt_cli_cmd) {
  (void)script;
  (void)out;
  bf_status_t sts;
  clish_shell_t *bfshell = clish_context__get_shell(clish_context);
  bool success = TRY_PYTHON_SHL_LOCK();
  if (!success) {
    bfshell_printf(clish_context,
                   "Only one Python shell instance allowed at a time. bfrt "
                   "python and debug python share the python shell "
                   "resource.\n");
    return 0;
  }

  clish_pargv_t *pargv = clish_context__get_pargv(clish_context);
  const clish_parg_t *parg = clish_pargv_find_arg(pargv, "py_file");
  const char *udf = NULL;
  if (parg) {
    udf = clish_parg__get_value(parg);
  }
  parg = clish_pargv_find_arg(pargv, "interactive");
  const char *i_str = NULL;
  bool interactive = false;
  if (parg) {
    i_str = clish_parg__get_value(parg);
    if (i_str && strcmp(i_str, "1") == 0) {
      interactive = true;
    }
  }

  tinyrl_t *bftinyrl = clish_shell__get_tinyrl(bfshell);
  reg_set_instream(tinyrl__get_istream(bftinyrl));
  reg_set_outstream(tinyrl__get_ostream(bftinyrl));

  sts = bf_rt_start_cli(fileno(tinyrl__get_istream(bftinyrl)),
                        fileno(tinyrl__get_ostream(bftinyrl)),
                        clish_context__get_install_dir(clish_context),
                        udf,
                        interactive);
  if (sts != BF_SUCCESS) {
    bfshell_printf(clish_context,
                   "%s:%d could not initialize bf_rt for the cli. err: %d\n",
                   __func__,
                   __LINE__,
                   sts);
  }
  reg_set_instream(stdin);
  reg_set_outstream(stdout);
  RELEASE_PYTHON_SHL_LOCK();
  return 0;
}

CLISH_PLUGIN_SYM(run_file_cmd) {
  (void)script;
  (void)out;

  clish_shell_t *this = clish_context__get_shell(clish_context);
  clish_pargv_t *pargv = clish_context__get_pargv(clish_context);

  const char *filename = NULL;
  const clish_parg_t *parg = clish_pargv_find_arg(pargv, "filename");
  if (parg) {
    filename = clish_parg__get_value(parg);
  }

  int stop_on_error = 1;
  parg = clish_pargv_find_arg(pargv, "stop_on_error");
  if (parg) {
    const char *i_str = clish_parg__get_value(parg);
    if (i_str && strcmp(i_str, "0") == 0) {
      stop_on_error = 0;
    }
  }

  int result = -1;
  struct stat fileStat;

  /*
   * Check file specified is not a directory
   */
  if (filename && (0 == stat((char *)filename, &fileStat)) &&
      (!S_ISDIR(fileStat.st_mode))) {
    /*
     * push this file onto the file stack associated with this
     * session. This will be closed by clish_shell_pop_file()
     * when it is finished with.
     */
    result = clish_shell_push_file(this, filename, stop_on_error);
  }

  return result ? -1 : 0;
}

CLISH_PLUGIN_INIT(bf_rt) {
  (void)clish_shell;
  clish_plugin_add_sym(plugin, bfrt_cli_cmd, "bfrt_cli_cmd");
  clish_plugin_add_sym(plugin, run_file_cmd, "run_file_cmd");
  return 0;
}
