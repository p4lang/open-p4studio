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

#include <fcntl.h>
#include <pthread.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>

#include "bmi_interface.h"
#include "bmi_port.h"

typedef struct bmi_port_s {
  bmi_interface_t *bmi;
  int port_num;
  char *ifname;
  int fd;
} bmi_port_t;

#define PORT_COUNT_MAX 1024

typedef struct bmi_port_mgr_s {
  bmi_port_t ports_info[PORT_COUNT_MAX];
  fd_set fds;
  int max_fd;
  struct timeval timeout;
  bmi_packet_handler_t packet_handler;
  pthread_t select_thread;
  pthread_mutex_t lock;
} bmi_port_mgr_t;

bmi_port_mgr_t* g_port_mgr = NULL;

static inline int port_in_use(bmi_port_t *port) {
  return (port->bmi != NULL);
}

static inline int port_num_valid(int port_num) {
  return (port_num >= 0 && port_num < PORT_COUNT_MAX);
}

static inline bmi_port_t *get_port(bmi_port_mgr_t *port_mgr, int port_num) {
  return &port_mgr->ports_info[port_num];
}

static void *run_select(void *data) {
  bmi_port_mgr_t *port_mgr = (bmi_port_mgr_t *) data;
  int n;
  int i;
  bmi_port_t *port_info;
  const char *pkt_data;
  int pkt_len;
  fd_set fds;

  struct timeval timeout;
  while (1) {
    /* timeout is needed to update set */
    timeout = port_mgr->timeout;

    fds = port_mgr->fds;
    n = select(port_mgr->max_fd + 1, &fds, NULL, NULL, &timeout);
    /* TODO: investigate this further */
    assert(n >= 0 || errno == EINTR);

    /* the thread terminates */
    if (port_mgr->max_fd == -1) return NULL;

    if (n <= 0) { // timeout or EINTR
      continue;
    }

    if (0 != pthread_mutex_lock(&port_mgr->lock)) {
      perror("Lock");
      return NULL;
    }

    /* use Judy instead ? */
    for (i = 0; n && i < PORT_COUNT_MAX; i++) {
      port_info = get_port(port_mgr, i);
      if (FD_ISSET(port_info->fd, &fds) && port_info->bmi) {
        int drops_since_last_rx = 0;
	--n;
        /* MODEL-761: Note bmi_interface_recv now NON-blocking */
        /* Also note, capture len is 16K and buf len is 4096K so should be */
        /*  able to capture 256 packets on every port at any time */
        pkt_len = bmi_interface_recv(port_info->bmi, &pkt_data, &drops_since_last_rx);
        if (drops_since_last_rx > 0) {
          fprintf(stdout, "ERROR: Dropped %d packets on port %d - reduce ingress rate!\n",
                  drops_since_last_rx, i);
        }
	if (pkt_len < 0) continue;
        if (pkt_len > 16128) {
          fprintf(stdout, "ERROR: Excessive packet length %d on port %d!\n", pkt_len, i);
          assert(pkt_len <= 16128); /* Max size ever allowed by H/W */
        }
	/* printf("Received pkt of len %d on port %d\n", pkt_len, i); */
	if (port_mgr->packet_handler) {
	  port_mgr->packet_handler(i, pkt_data, pkt_len);
	}
      }
    }

    if (0 != pthread_mutex_unlock(&port_mgr->lock)) {
      perror("Unlock");
      return NULL;
    }
  }

  return NULL;
}

int bmi_port_create_mgr(bmi_port_mgr_t **port_mgr) {
  bmi_port_mgr_t *port_mgr_;

  if(!port_mgr) return -1;

  port_mgr_ = malloc(sizeof(bmi_port_mgr_t));
  if(!port_mgr_) return -1;

  memset(port_mgr_, 0, sizeof(bmi_port_mgr_t));

  FD_ZERO(&port_mgr_->fds);

  port_mgr_->max_fd = 0;
  port_mgr_->timeout.tv_sec = 0;
  port_mgr_->timeout.tv_usec = 100000; // 100ms

  pthread_mutex_init(&port_mgr_->lock, NULL);

  *port_mgr = port_mgr_;
  g_port_mgr = *port_mgr;
  return 0;
}

int bmi_set_packet_handler(bmi_port_mgr_t *port_mgr,
			   bmi_packet_handler_t packet_handler) {
  port_mgr->packet_handler = packet_handler;
  return 0;
}

int bmi_port_start_pkt_processing(void) {
  pthread_create(&g_port_mgr->select_thread, NULL, run_select, g_port_mgr);
  return 0;
}

int bmi_port_send(bmi_port_mgr_t *port_mgr,
		  int port_num, const char *buffer, int len) {
  if(!port_num_valid(port_num)) return -1;
  bmi_port_t *port = get_port(port_mgr, port_num);
  if(!port_in_use(port)) return -1;

  if(bmi_interface_send(port->bmi, buffer, len) != 0) return -1;

  return 0;
}

int bmi_port_interface_add(bmi_port_mgr_t *port_mgr,
			   const char *ifname, int port_num,
			   const char *pcap_dump) {
  if(!port_num_valid(port_num)) return -1;

  bmi_port_t *port = get_port(port_mgr, port_num);
  if(port_in_use(port)) return -1;

  port->ifname = strdup(ifname);

  bmi_interface_t *bmi;
  if(bmi_interface_create(&bmi, ifname) != 0) return -1;

  if(pcap_dump) bmi_interface_add_dumper(bmi, pcap_dump);

  if (0 != pthread_mutex_lock(&port_mgr->lock)) {
    perror("Lock");
    free(bmi);
    return -1;
  }

  port->bmi = bmi;

  int fd = bmi_interface_get_fd(port->bmi);
  port->fd = fd;
  if (fd > port_mgr->max_fd) port_mgr->max_fd = fd;

  /* Maintain minimum timeout */
  const struct timeval *t = bmi_interface_get_select_timeout(port->bmi);
  if (t != NULL) {
    if ((t->tv_sec < port_mgr->timeout.tv_sec) ||
        ((t->tv_sec == port_mgr->timeout.tv_sec) && (t->tv_usec < port_mgr->timeout.tv_usec))) {
      port_mgr->timeout = *t;
    }
  }

  FD_SET(fd, &port_mgr->fds);

  if (0 != pthread_mutex_unlock(&port_mgr->lock)) {
    perror("Unlock");
    return -1;
  }

  return 0;
}

int
bmi_port_interface_is_up(bmi_port_mgr_t *port_mgr, int port_num, bool *is_up) {
  if(!port_num_valid(port_num)) return -1;

  bmi_port_t *port = get_port(port_mgr, port_num);
  if(!port_in_use(port)) return -1;

  char c = 0;
  char path[1024] = {0};
  sprintf(path, "/sys/class/net/%s/operstate", port->ifname);

  int fd = open(path, O_RDONLY);
  if (-1 == fd) {
      perror("open");
      return -1;
  }
  /* File should contain either "up" or "down", just read the first letter
   * of the word. */
  if (read(fd, &c, 1) != 1) {
      perror("read");
      close(fd);
      return -1;
  }
  close(fd);
  if (c == 'u') {
    (*is_up) = true;
  }
  else {
    (*is_up) = false;
  }
  return 0;
}

int bmi_port_interface_remove(bmi_port_mgr_t *port_mgr, int port_num) {
  if (!port_num_valid(port_num)) return -1;

  bmi_port_t *port = get_port(port_mgr, port_num);
  if (!port_in_use(port)) return -1;

  int ret = 0;
  char *ifn = port->ifname;

  if (0 != pthread_mutex_lock(&port_mgr->lock)) {
    perror("Lock");
    return -1;
  }

  if (bmi_interface_destroy(port->bmi) != 0) ret = -1;

  if (ret == 0) {
    FD_CLR(port->fd, &port_mgr->fds);
    memset(port, 0, sizeof(bmi_port_t)); // NULL everything
  }

  if (0 != pthread_mutex_unlock(&port_mgr->lock)) {
    perror("Unlock");
    return -1;
  }

  // Free port->ifname outside lock
  if (ret == 0) free(ifn);

  return ret;
}

int bmi_port_destroy_mgr(bmi_port_mgr_t *port_mgr) {
  int i;
  for(i = 0; i < PORT_COUNT_MAX; i++) {
    bmi_port_t *port = get_port(port_mgr, i);
    if (port_in_use(port)) {
      bmi_port_interface_remove(port_mgr, i);
    }
  }

  port_mgr->max_fd = -1;  // used to signal the thread it needs to terminate

  port_mgr->timeout.tv_sec = port_mgr->timeout.tv_usec = 0;

  pthread_join(port_mgr->select_thread, NULL);

  pthread_mutex_destroy(&port_mgr->lock);

  free(port_mgr);
  g_port_mgr = NULL;
  return 0;
}