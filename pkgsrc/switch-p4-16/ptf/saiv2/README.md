SAI Tests
=========

CI tests
--------

CI executes the `pep8` and `pylint` checkers. Run them manually before commit:

`pylint sourcefile.py`

`pylint` configuration is stored in _saiv2/.pylintrc_, so it must be called from _saiv2_ directory.

`pycodestyle sourcefile.py`

Older distributions has `pep8` executable instead of `pycodestyle`.


### Formatters

One of the following formatters can be used to automatically fix multiple `pep8` issues:
- `autopep8 -iaa _sourcefile.py_`
- `yapf -i --style pep8 sourcefile.py`
- `isort sourcefile.py` (imports only)

Don't forget to review the automatically formatted code.

### Parameter documentation

Each function should have parameter documentation (_Google style_), as specified [here](https://docs.pylint.org/en/1.6.0/extensions.html).
The plugin checks **parameters**, **return value** and **exceptions**.

### Troubleshooting

1. **prints has parentheses**<br>
   Just import `from __future__ import print_function`<br>
2. **import errors**<br>
   `pylint` uses a `PYTHONPATH`, so something like this should help:<br>
   `export PYTHONPATH=$PYTHONPATH:p4factory/submodules/bf-switch/ptf/:p4factory/install/lib/python2.7/site-packages/`<br> 
   or<br>
   `export PYTHONPATH="$PYTHONPATH:${WORKSPACE}/ptf/:/bf/install/lib/python2.7/site-packages/"`
3. **`pylint` doesn't work**<br>
   Just install the latest version (using `pip`)
4. **ignoring errors:**
    - pep8: `# noqa`
    - pylint: `# pylint: disable=my-error`<br>
      If used in separate line, then don't forget to enable it again!<br>
    - example:<br>
      `from common.utils import * # noqa pylint: disable=wrong-import-position` 
5. **generated members**<br>
   Some of members are auto-generated (e.g. *dev_port* in *sai_base_test.py*) and hence
   not detected by _pylint_. To avoid _E1101_ error, they should be added to
   _generated-members_ list in _.pylintrc_.
6. **most of parameters are overridden from common interface**<br>
   Use _For the other parameters, see_ or _For the parameters, see_ phrase.


Exceptions handling
-------------------

In case of error, the *sai\_rpc\_server.cpp* throws an exception. When exception is thrown,
no other data can be returned (unless it is part of the exception itself).
Depending on *sai\_adapter.CATCH\_EXCEPTIONS* value, exceptions are being handled by *sai\_adapter.py*
(*True*) or thrown to the caller (*False*).

### Examples

The status can be checked using *self.status()* method:

	# sai_adapter.CATCH_EXCEPTIONS == False
	self.switch_id = sai_thrift_create_switch(
	    self.client, init_switch=True, src_mac_address=ROUTER_MAC)
	self.assertEqual(self.status(), SAI_STATUS_SUCCESS)

In order to catch exceptions, try-except block can be used:

	# sai_adapter.CATCH_EXCEPTIONS == True
	try:
	    self.switch_id = sai_thrift_create_switch(
	        self.client, init_switch=True, src_mac_address=ROUTER_MAC)
	except sai_thrift_exception as e:
	    print("status: ", e.status)
