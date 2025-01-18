#ifndef P4STUDIO_CONFIG_H
#define P4STUDIO_CONFIG_H


/* Name of package */
#cmakedefine PACKAGE "${PACKAGE}"

/* Define to the address where bug reports for this package should be sent. */
#cmakedefine PACKAGE_BUGREPORT "${PACKAGE_BUGREPORT}"

/* Define to the full name of this package. */
#cmakedefine PACKAGE_NAME "${PACKAGE_NAME}"

/* Define to the one symbol short name of this package. */
#cmakedefine PACKAGE_TARNAME "${PACKAGE_TARNAME}"

/* Define to the home page for this package. */
#cmakedefine PACKAGE_URL "${PACKAGE_URL}"

/* Define to the version of this package. */
#define PACKAGE_VERSION "${PACKAGE_VERSION}"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "${PACKAGE_STRING}"

/* Version number of package */
#define VERSION "${VERSION}"

#define DEFAULT_ZLOG_CFG_FILE "${CMAKE_INSTALL_PREFIX}/share/bfsys/zlog-cfg"

/************************** HEADER FILES *************************/

/* Define to 1 if you have the <arpa/inet.h> header file. */
#cmakedefine HAVE_ARPA_INET_H 1

/* Define to 1 if you have the <assert.h> header file. */
#cmakedefine HAVE_ASSERT_H 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#cmakedefine HAVE_DLFCN_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
#cmakedefine HAVE_FCNTL_H 1

/* Define to 1 if you have the <float.h> header file. */
#cmakedefine HAVE_FLOAT_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#cmakedefine HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H 1

/* Define to 1 if you have the <malloc.h> header file. */
#cmakedefine HAVE_MALLOC_H 1

/* Define to 1 if you have the <regex.h> header file. */
#cmakedefine HAVE_REGEX_H 1

/* Define to 1 if you have the <netdb.h> header file. */
#cmakedefine HAVE_NETDB_H 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#cmakedefine HAVE_NETINET_IN_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#cmakedefine HAVE_STDIO_H 1

/* Define to 1 if you have the <stdbool.h> header file. */
#cmakedefine HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stddef.h> header file. */
#cmakedefine HAVE_STDDEF_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#cmakedefine HAVE_STDLIB_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <syslog.h> header file. */
#cmakedefine HAVE_SYSLOG_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the <pthread.h> header file. */
#cmakedefine HAVE_PTHREAD_H 1

/* Define to 1 if you have the <math.h> header file. */
#cmakedefine HAVE_MATH_H 1

/* Define to 1 if you have the <memory.h> header file. */
#cmakedefine HAVE_MEMORY_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/file.h> header file. */
#cmakedefine HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#cmakedefine HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#cmakedefine HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/resource.h> header file. */
#cmakedefine HAVE_SYS_RESOURCE_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#cmakedefine HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#cmakedefine HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#cmakedefine HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <sys/un.h> header file. */
#cmakedefine HAVE_SYS_UN_H 1

/* Define to 1 if you have the <sys/poll.h> header file. */
#cmakedefine HAVE_SYS_POLL_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#cmakedefine HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/epoll.h> header file. */
#cmakedefine HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/eventfd.h> header file. */
#cmakedefine HAVE_SYS_EVENTFD_H 1

/* Define to 1 if you have the <sys/inotify.h> header file. */
#cmakedefine HAVE_SYS_INOTIFY_H 1

/* Define to 1 if you have the <sys/signalfd.h> header file. */
#cmakedefine HAVE_SYS_SIGNALFD_H 1

/* Define to 1 if you have the <sched.h> header file. */
#cmakedefine HAVE_SCHED_H 1

/* Define to 1 if you have the <strings.h> header file. */
#cmakedefine HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <pcap/pcap.h> header file. */
#cmakedefine HAVE_PCAP_H 1

/* Define to 1 if you have the <getopt.h> header file. */
#cmakedefine HAVE_GETOPT_H 1

/* Define to 1 if you have the <grp.h> header file. */
#cmakedefine HAVE_GRP_H 1

/* Define to 1 if you have the <pwd.h> header file. */
#cmakedefine HAVE_PWD_H 1

/* Define to 1 if you have the <locale.h> header file. */
#cmakedefine HAVE_LOCALE_H 1

/* Define to 1 if you have the <luaconf.h> header file. */
#cmakedefine HAVE_LUACONF_H 1

/* Define to 1 if you have the <lualib.h> header file. */
#cmakedefine HAVE_LUALIB_H 1

/* Define to 1 if you have the <lua.h> header file. */
#cmakedefine HAVE_LUA_H 1

/*************************** FUNCTIONS ***************************/

/* Define to 1 if you have the `clock_gettime' function. */
#cmakedefine HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the `floor' function. */
#cmakedefine HAVE_FLOOR 1

/* Define to 1 if you have the `gethostbyname' function. */
#cmakedefine HAVE_GETHOSTBYNAME 1

/* Define to 1 if you have the `getpagesize' function. */
#cmakedefine HAVE_GETPAGESIZE 1

/* Define to 1 if you have the `gettimeofday' function. */
#cmakedefine HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the `inet_ntoa' function. */
#cmakedefine HAVE_INET_NTOA 1

/* Define to 1 if you have the `malloc' function. */
#cmakedefine HAVE_MALLOC 1

/* Define to 1 if you have the `memmove' function. */
#cmakedefine HAVE_MEMMOVE 1

/* Define to 1 if you have the `memset' function. */
#cmakedefine HAVE_MEMSET 1

/* Define to 1 if you have the `mmap' function. */
#cmakedefine HAVE_MMAP 1

/* Define to 1 if you have the `munmap' function. */
#cmakedefine HAVE_MUNMAP 1

/* Define to 1 if you have the `pow' function. */
#cmakedefine HAVE_POW 1

/* Define to 1 if you have the `regcomp' function. */
#cmakedefine HAVE_REGCOMP 1

/* Define to 1 if you have the `setenv' function. */
#cmakedefine HAVE_SETENV 1

/* Define to 1 if you have the `socket' function. */
#cmakedefine HAVE_SOCKET 1

/* Define to 1 if you have the `sqrt' function. */
#cmakedefine HAVE_SQRT 1

/* Define to 1 if you have the `strchr' function. */
#cmakedefine HAVE_STRCHR 1

/* Define to 1 if you have the `strcspn' function. */
#cmakedefine HAVE_STRCSPN 1

/* Define to 1 if you have the `strdup' function. */
#cmakedefine HAVE_STRDUP 1

/* Define to 1 if you have the `strerror' function. */
#cmakedefine HAVE_STRERROR 1

/* Define to 1 if you have the `strrchr' function. */
#cmakedefine HAVE_STRRCHR 1

/* Define to 1 if you have the `strspn' function. */
#cmakedefine HAVE_STRSPN 1

/* Define to 1 if you have the `strstr' function. */
#cmakedefine HAVE_STRSTR 1

/* Define to 1 if you have the `strtol' function. */
#cmakedefine HAVE_STRTOL 1

/* Define to 1 if you have the `strtoul' function. */
#cmakedefine HAVE_STRTOUL 1

/* Define to 1 if you have the `strtoull' function. */
#cmakedefine HAVE_STRTOULL 1

/* Define to 1 if you have the `epoll_ctl' function. */
#cmakedefine HAVE_EPOLL_CTL 1

/* Define to 1 if you have the `eventfd' function. */
#cmakedefine HAVE_EVENTFD 1

/* Define to 1 if you have the `inotify_init' function. */
#cmakedefine HAVE_INOTIFY_INIT 1

/* Define to 1 if you have the `nanosleep' function. */
#cmakedefine HAVE_NANOSLEEP 1

/* Define to 1 if you have the `poll' function. */
#cmakedefine HAVE_POLL 1

/* Define to 1 if you have the `select' function. */
#cmakedefine HAVE_SELECT 1

/* Define to 1 if you have the `signalfd' function. */
#cmakedefine HAVE_SIGNALFD 1

/* Define to 1 if you have the `gethostbyname_r' function. */
#cmakedefine HAVE_GETHOSTBYNAME_R 1

/* Define to 1 if you have the `strerror_r' function. */
#cmakedefine HAVE_STRERROR_R 1

/* Define to 1 if you have the `ether_aton_r' function. */
#cmakedefine HAVE_ETHER_ATON_R 1

/* Define to 1 if you have the `inet_network' function. */
#cmakedefine HAVE_INET_NETWORK 1

/* Define to 1 if you have the `chroot' function. */
#cmakedefine HAVE_CHROOT 1

/* Define to 1 if you have the `getopt_long' function. */
#cmakedefine HAVE_GETOPT_LONG 1

#endif

