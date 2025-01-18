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

// TODO: re-instate this utest for any OS where we have os_privs_ support
#ifdef __linux__

#ifndef _GNU_SOURCE
#define _GNU_SOURCE  // to get secure_getenv() from stdlib.h
#endif
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <grp.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <os_privs.h>

#include <gtest.h>

namespace runner_utests {

int get_ids(int *uidp, int *gidp) {
  assert((uidp != NULL) && (gidp != NULL));
  *uidp = getuid();
  *gidp = getgid();
  return 0;
}
int get_sudo_ids(int *uidp, int *gidp, char **userp) {
  assert((uidp != NULL) && (gidp != NULL));
  int uid = -1;
  const char *sudo_uid = secure_getenv("SUDO_UID");
  if (sudo_uid != NULL) {
    errno = 0;
    uid = (uid_t) strtoll(sudo_uid, NULL, 10);
    if (errno != 0) uid = -1;
  }
  int gid = -1;
  const char *sudo_gid = secure_getenv("SUDO_GID");
  if (sudo_gid != NULL) {
    errno = 0;
    gid = (uid_t) strtoll(sudo_gid, NULL, 10);
    if (errno != 0) gid = -1;
  }
  char *user = secure_getenv("SUDO_USER");
  *uidp = uid;
  *gidp = gid;
  if (userp != NULL) *userp = user;
  return ((uid >= 0) && (gid >= 0)) ?0 :-1;
}
int get_cwd_ids(int *uidp, int *gidp) {
  assert((uidp != NULL) && (gidp != NULL));
  struct stat statbuf;
  int result = stat(".", &statbuf);
  if (result == 0) {
    *uidp = statbuf.st_uid;
    *gidp = statbuf.st_gid;
  }
  return result;
}
int get_unpriv_ids(int *uidp, int *gidp, char **userp) {
  assert((uidp != NULL) && (gidp != NULL));
  (void)get_ids(uidp, gidp);
  if ((*uidp == 0) || (*gidp == 0)) {
    (void)get_sudo_ids(uidp, gidp, userp);
    if ((*uidp <= 0) || (*gidp <= 0)) {
      // Try get from CWD
      (void)get_cwd_ids(uidp, gidp);
      if ((*uidp <= 0) || (*gidp <= 0)) {
        *uidp = *gidp = 65534; // Nobody/nogroup
      }
    }
  }
  return 0;
}
int has_root_priv(void) { // yes=1 no=0
  return (geteuid() == 0);
}

int can_open_raw_sock(void) { // yes=1 no=0
  int can_open = 0;
  // Try opening the raw socket
  int sock = socket(PF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sock >= 0) {
    can_open = 1;
    close(sock);
  }
  return can_open;
}
int can_open_file(const char *path) { // yes=1 no=0
  int can_open = 0;
  int fd = open(path, O_RDWR);
  if (fd >= 0) {
    can_open = 1;
    close(fd);
  } else {
    perror("open");
  }
  return can_open;
}
int can_open_files(const char *path1, const char *path2) {
  // Returns count of files that can be opened else 0
  int cnt = 0;
  if (can_open_file(path1)) cnt++;
  if (can_open_file(path2)) cnt++;
  return cnt;
}


bool                    locked = true;
std::thread            *thread = nullptr;
std::mutex              mtx;
std::condition_variable cv;

void cleanup(const char *path1, const char *path2) {
  (void)unlink(path1);
  (void)unlink(path2);
}
void cleanup_wait(const char *path1, const char *path2) {
  std::unique_lock<std::mutex> lck(mtx);
  while (locked) cv.wait(lck);
  cleanup(path1, path2);
}
void cleanup_signal(void) {
  std::unique_lock<std::mutex> lck(mtx);
  locked = false;
  cv.notify_one();
}

class RunnerTest : public testing::Test {
public:
  void TearDown() override {
    if (thread == nullptr) return;
    cleanup_signal();
    // Then join/delete thread
    thread->join();
    delete thread;
    thread = nullptr;
  }
};


TEST_F(RunnerTest,OsPrivs) {
  int uid = 0, gid = 0; // For unprivileged uid/gid

  // Only do anything if run as root..
  if ((!has_root_priv()) ||
      (get_unpriv_ids(&uid, &gid, NULL) < 0)) {
    printf("RunnerTest.OsPrivs: !!!!!!!!!! NOTE RunnerTest.OsPrivs needs to be run with sudo !!!!!!!!!!\n");
    return;
  }

  const char *root_filename = "00.DELETE_ME.root";
  const char *user_filename = "00.DELETE_ME.user";

  // Create thread - this should maintain original privs
  // and so should be allowed to delete files created
  thread = new std::thread(cleanup_wait, root_filename, user_filename);

  // Create root and user tmp files '00.DELETE_ME.root|user' with mode 600
  (void)unlink(root_filename);
  (void)unlink(user_filename);
  int fdr = open(root_filename, O_CREAT|O_RDONLY, S_IRUSR|S_IWUSR);
  int fdu = open(user_filename, O_CREAT|O_RDONLY, S_IRUSR|S_IWUSR);
  int ret = fchown(fdu, uid, gid); // Chown to unpriv uid/gid
  if (fdr >= 0) (void)close(fdr);
  if (fdu >= 0) (void)close(fdu);
  if ((fdr < 0) || (fdu < 0) || (ret < 0)) {
    printf("RunnerTest.OsPrivs: could not create/chown files %s and %s\n",
           root_filename, user_filename);
    FAIL();
  }


  // Should be able to open raw sock and open both files
  EXPECT_EQ(1, can_open_raw_sock());
  EXPECT_EQ(2, can_open_files(root_filename, user_filename));

  // Do os_privs_init() to remove effective privs
  // - even under sudo will have NO effective privs
  //
  // os_privs_dropped() should be false
  // os_has_cap_net_raw() should be true - because in permitted set
  // check can not open raw sock
  // check can only open 1 file of 2 (root file)
  EXPECT_EQ(0, os_privs_init());
  EXPECT_EQ(0, os_privs_dropped());
  EXPECT_EQ(1, os_privs_has_cap_net_raw());
  EXPECT_EQ(0, can_open_raw_sock());
  EXPECT_EQ(1, can_open_files(root_filename, user_filename));

  // Call os_privs_for_veth_attach()
  // Do os_can_open_raw_sock() - should be true - CAP_NET_RAW now effective
  EXPECT_EQ(0, os_privs_for_veth_attach());
  EXPECT_EQ(1, os_privs_has_cap_net_raw());
  EXPECT_EQ(1, can_open_raw_sock());

  // Call os_privs_reset
  // os_privs_dropped() should be false
  // Do os_can_open_raw_sock() - should be false - CAP_NET_RAW now ineffective again
  EXPECT_EQ(0, os_privs_reset());
  EXPECT_EQ(0, os_privs_dropped());
  EXPECT_EQ(1, os_privs_has_cap_net_raw());
  EXPECT_EQ(0, can_open_raw_sock());

  // Call os_privs_for_file_access()
  // Check can open BOTH files
  EXPECT_EQ(0, os_privs_for_file_access());
  EXPECT_EQ(2, can_open_files(root_filename, user_filename));

  // Call os_privs_reset
  // os_privs_dropped() should be false
  // Check can only open 1 file of 2 (root file)
  EXPECT_EQ(0, os_privs_reset());
  EXPECT_EQ(0, os_privs_dropped());
  EXPECT_EQ(1, can_open_files(root_filename, user_filename));

  // Call os_privs_drop_permanently
  // os_privs_dropped() should now be true
  EXPECT_EQ(0, os_privs_drop_permanently());
  EXPECT_EQ(1, os_privs_dropped());

  // Try os_privs_for_veth_attach - should fail with EPERM
  // Do os_can_open_raw_sock() - should be false - CAP_NET_RAW now permanently ineffective
  EXPECT_EQ(-1, os_privs_for_veth_attach());
  EXPECT_EQ(0, os_privs_has_cap_net_raw());
  EXPECT_EQ(0, can_open_raw_sock());

  // Try os_privs_for_file_access - still ok as this func only sets
  // privs as effective if they were already permitted.
  // Check can only open 1 file of 2 (root file)
  EXPECT_EQ(0, os_privs_for_file_access());
  EXPECT_EQ(1, can_open_files(root_filename, user_filename));

  EXPECT_EQ(1, os_privs_dropped());
}

}
#endif
