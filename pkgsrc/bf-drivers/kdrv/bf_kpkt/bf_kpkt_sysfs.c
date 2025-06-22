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

#include <linux/types.h>
#include <linux/string.h>
#include <linux/device.h>

#include <linux/netdevice.h>

#include <bf_types/bf_types.h>
#include <dvm/bf_drv_intf.h>
#include <dvm/bf_dma_types.h>

#include "bf_kpkt_priv.h"

static const char *bf_kpkt_sysfs_evt_name[BF_KPKT_EVT_MAX] = {"init",
                                                              "m_init",
                                                              "pktdev_add",
                                                              "dev_add",
                                                              "dev_del",
                                                              "dev_lock",
                                                              "dev_unlock"};
static const char *bf_kpkt_sysfs_st_name[BF_KPKT_ST_MAX] = {"uninit",
                                                            "init",
                                                            "master_init",
                                                            "pktdev_add",
                                                            "dev_add",
                                                            "dev_del",
                                                            "lock",
                                                            "unlock"};

typedef int (*bf_kpkt_sysfs_fn)(struct bf_kpkt_adapter *adapter, const char *buf);

static int fn_none(struct bf_kpkt_adapter *adapter, const char *buf) { return 0; }

static int fn_m_init(struct bf_kpkt_adapter *adapter, const char *buf) {
  printk(KERN_WARNING "bf_m_init %x\n", buf[0]);
  if (bf_kpkt_lld_master_init(adapter->dev_id, adapter, buf) != 0) {
    return -EIO;
  } else {
    adapter->bf_kpkt_st = BF_KPKT_ST_MASTER_INIT;
    return 0;
  }
}

static int fn_dev_add(struct bf_kpkt_adapter *adapter, const char *buf) {
  (void)buf;
  printk(KERN_WARNING "bf_dev_add\n");
  if (bf_kpkt_dev_add(adapter) != 0) {
    return -EIO;
  } else {
    return 0;
  }
}

static int fn_dev_del(struct bf_kpkt_adapter *adapter, const char *buf) {
  (void)buf;
  printk(KERN_WARNING "bf_dev_del\n");
  if (bf_kpkt_dev_del(adapter) != 0) {
    return -EIO;
  } else {
    return 0;
  }
}

static int fn_dev_to_m(struct bf_kpkt_adapter *adapter, const char *buf) {
  int rc;
  rc = fn_dev_del(adapter, buf);
  rc |= fn_m_init(adapter, buf);
  adapter->bf_kpkt_st = BF_KPKT_ST_MASTER_INIT;
  return rc;
}

static int fn_m_to_m(struct bf_kpkt_adapter *adapter, const char *buf) {
  int rc;
  printk(KERN_WARNING "bf: bf_m_init invoked while in m_init state %x\n", buf[0]);
  bf_kpkt_lld_master_deinit(adapter);
  msleep(1);
  rc = fn_m_init(adapter, buf);
  return rc;
}

static const bf_kpkt_sysfs_fn bf_st_fn[BF_KPKT_ST_MAX][BF_KPKT_EVT_MAX] = {
    /* ST_UNINIT */
    {fn_none, fn_none, fn_none, fn_none, fn_none, fn_none, fn_none},
    /* ST_INIT */
    {fn_none, fn_m_init, fn_none, fn_none, fn_none, fn_none, fn_none},
    /* ST_MASTER_INIT */
    {fn_none, fn_m_to_m, fn_none, fn_dev_add, fn_none, fn_none, fn_none},
    /* ST_PKT_DEV_ADD */
    {fn_none, fn_none, fn_none, fn_none, fn_none, fn_none, fn_none},
    /* ST_DEV_ADD */
    {fn_none, fn_dev_to_m, fn_none, fn_none, fn_dev_del, fn_none, fn_none},
    /* ST_DEV_DEL */
    {fn_none, fn_m_init, fn_none, fn_dev_add, fn_none, fn_none, fn_none},
    /* ST_DEV_LOCK */
    {fn_none, fn_none, fn_none, fn_none, fn_none, fn_none, fn_none},
    /* ST_DEV_UNLOCK */
    {fn_none, fn_none, fn_none, fn_none, fn_none, fn_none, fn_none},
};

static ssize_t bf_kpkt_cfg_get(struct device *dev,
                               struct device_attribute *attr,
                               char *buf) {
  struct bf_kpkt_adapter *adapter;
  ssize_t size;
  struct kpkt_sysfs_buff_s *sysfs_buf =
      container_of(attr, struct kpkt_sysfs_buff_s, dev_attr);

  if (!sysfs_buf) {
    dev_err(dev, "bad attr pointer in sysfs_read\n");
    return -ENXIO; /* something not quite right here; but, don't panic */
  } else {
    adapter = sysfs_buf->adapter;
    if (!adapter) {
      return -ENXIO;
    }
    if (adapter->bf_kpkt_st >= BF_KPKT_ST_MAX) {
      size = scnprintf(buf, PAGE_SIZE, "device state: bad\n");
    } else {
      size = scnprintf(buf,
                       PAGE_SIZE,
                       "device state: %s\n",
                       bf_kpkt_sysfs_st_name[adapter->bf_kpkt_st]);
    }
  }
  return size;
}

static ssize_t bf_kpkt_cfg_set(struct device *dev,
                               struct device_attribute *attr,
                               const char *buf,
                               size_t count) {
  struct bf_kpkt_adapter *adapter;
  int evt_no;
  struct kpkt_sysfs_buff_s *sysfs_buf =
      container_of(attr, struct kpkt_sysfs_buff_s, dev_attr);

  /* currently, we check for the values written into the file. The default has
   * to be '1' for all sysfs files.
   * for device initialization case, '1' means cold and '2' means warm init.
   * no other values are permitted to be written.
   */
  if (buf[0] != '1' && buf[0] != '2') {
    /* nothing to do; pretend "done" */
    return -EINVAL;
  }
  if (!sysfs_buf) {
    dev_err(dev, "bad attr pointer in sysfs_write\n");
    return -ENXIO; /* something not quite right here; but, don't panic */
  } else {
    adapter = sysfs_buf->adapter;
    evt_no = sysfs_buf->evt_no;

    if (!adapter || evt_no >= BF_KPKT_EVT_MAX ||
        adapter->bf_kpkt_st >= BF_KPKT_ST_MAX) {
      /* something not quite right here; but, don't panic */
      return -ENXIO;
    }
    if ((*bf_st_fn[adapter->bf_kpkt_st][evt_no])(adapter, buf) != 0) {
      return -EIO;
    }
  }
  return count;
}

int bf_kpkt_sysfs_add(struct bf_kpkt_adapter *adapter) {
  int i, rc = 0;
  size_t size = BF_KPKT_SYSFS_NAME_SIZE;
  u8 *name;

  for (i = 0; i < BF_KPKT_EVT_MAX; i++) {
    adapter->kpkt_sysfs_buff[i].dev_attr.show = bf_kpkt_cfg_get;
    adapter->kpkt_sysfs_buff[i].dev_attr.store = bf_kpkt_cfg_set;
    adapter->kpkt_sysfs_buff[i].evt_no = i;
    adapter->kpkt_sysfs_buff[i].adapter = adapter; /* point back to itself */
    adapter->kpkt_sysfs_buff[i].dev_attr.attr.mode = S_IWUSR | S_IRUGO;
    name = adapter->kpkt_sysfs_buff[i].name;
    snprintf(name, size, "%s", bf_kpkt_sysfs_evt_name[i]);
    adapter->kpkt_sysfs_buff[i].dev_attr.attr.name = name;
    rc |=
        device_create_file(adapter->dev, &adapter->kpkt_sysfs_buff[i].dev_attr);
  }

  return rc;
}

void bf_kpkt_sysfs_del(struct bf_kpkt_adapter *adapter) {
  int i;

  for (i = 0; i < BF_KPKT_EVT_MAX; i++) {
    device_remove_file(adapter->dev, &adapter->kpkt_sysfs_buff[i].dev_attr);
  }
}
