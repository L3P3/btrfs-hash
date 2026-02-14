/* include/config.h.  Generated from config.h.in by configure.  */
/* include/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */

/* disable backtrace stuff in kerncompat.h */
#define BTRFS_DISABLE_BACKTRACE 1

/* Define to 1 if zoned device support is available */
/* #undef BTRFS_ZONED */

/* Crypto implementation source name */
#define CRYPTOPROVIDER "openssl"

/* Use Botan */
#define CRYPTOPROVIDER_BOTAN 0

/* Use builtin implementation */
#define CRYPTOPROVIDER_BUILTIN 0

/* Use libcrypt */
#define CRYPTOPROVIDER_LIBGCRYPT 0

/* Use libkcapi */
#define CRYPTOPROVIDER_LIBKCAPI 0

/* Use libsodium */
#define CRYPTOPROVIDER_LIBSODIUM 0

/* Use OpenSSL */
#define CRYPTOPROVIDER_OPENSSL 1

/* Define to 1 if you experimental and unstable features are build */
#define EXPERIMENTAL 0

/* for encode and decode tv_nsec in ext2 inode */
/* #undef EXT4_EPOCH_BITS */

/* For encode and decode tv_nsec info in ext2 inode */
/* #undef EXT4_EPOCH_MASK */

/* For encode and decode tv_nsec info in ext2 inode */
/* #undef EXT4_NSEC_MASK */

/* Define to 1 if you have the 'backtrace' function. */
#define HAVE_BACKTRACE 1

/* Define to 1 if you have the 'backtrace_symbols_fd' function. */
#define HAVE_BACKTRACE_SYMBOLS_FD 1

/* Compiler supports -mavx2 */
#define HAVE_CFLAG_mavx2 1

/* Compiler supports -msha */
#define HAVE_CFLAG_msha 1

/* Compiler supports -msse2 */
#define HAVE_CFLAG_msse2 1

/* Compiler supports -msse4.1 */
#define HAVE_CFLAG_msse41 1

/* Define to 1 if your system has a working 'chown' function. */
#define HAVE_CHOWN 1

/* Define to 1 if you have the 'clock_gettime' function. */
#define HAVE_CLOCK_GETTIME 1

/* Define to 1 if you have the <execinfo.h> header file. */
#define HAVE_EXECINFO_H 1

/* Define to 1 if ext2_inode_large includes i_atime_extra */
#define HAVE_EXT4_EPOCH_MASK_DEFINE 1

/* Define to 1 if you have the 'fchdir' function. */
#define HAVE_FCHDIR 1

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the 'fdatasync' function. */
#define HAVE_FDATASYNC 1

/* Define to 1 if you have the 'fork' function. */
#define HAVE_FORK 1

/* Define to 1 if you have the 'ftruncate' function. */
#define HAVE_FTRUNCATE 1

/* Define to 1 if you have the 'getcwd' function. */
#define HAVE_GETCWD 1

/* Define to 1 if you have the 'getmntent' function. */
#define HAVE_GETMNTENT 1

/* Define to 1 if you have the 'getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define to 1 if you have the 'gettimeofday' function. */
#define HAVE_GETTIMEOFDAY 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the 'lchown' function. */
#define HAVE_LCHOWN 1

/* Define to 1 if libudev is available */
#define HAVE_LIBUDEV 1

/* Define to 1 if you have the <linux/fsverity.h> header file. */
#define HAVE_LINUX_FSVERITY_H 1

/* Define to 1 if you have the <linux/hw_breakpoint.h> header file. */
#define HAVE_LINUX_HW_BREAKPOINT_H 1

/* Define to 1 if you have the <linux/perf_event.h> header file. */
#define HAVE_LINUX_PERF_EVENT_H 1

/* Define to 1 if you have the 'localtime_r' function. */
#define HAVE_LOCALTIME_R 1

/* Define to 1 if your system has a GNU libc compatible 'malloc' function, and
   to 0 otherwise. */
#define HAVE_MALLOC 1

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the 'memchr' function. */
#define HAVE_MEMCHR 1

/* Define to 1 if you have the 'memmove' function. */
#define HAVE_MEMMOVE 1

/* Define to 1 if you have the 'memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have the <minix/config.h> header file. */
/* #undef HAVE_MINIX_CONFIG_H */

/* Define to 1 if you have the 'mkdir' function. */
#define HAVE_MKDIR 1

/* Define to 1 if you have the 'mkfifo' function. */
#define HAVE_MKFIFO 1

/* Define to 1 if you have a working 'mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <mntent.h> header file. */
#define HAVE_MNTENT_H 1

/* Define to 1 if you have the 'munmap' function. */
#define HAVE_MUNMAP 1

/* Define to 1 if you have the 'openat' function. */
#define HAVE_OPENAT 1

/* Define to 1 if kernel headers do not define FIEMAP_EXTENT_SHARED */
/* #undef HAVE_OWN_FIEMAP_EXTENT_SHARED_DEFINE */

/* Define to 1 if you have the <printf.h> header file. */
#define HAVE_PRINTF_H 1

/* Define to 1 if your system has a GNU libc compatible 'realloc' function,
   and to 0 otherwise. */
#define HAVE_REALLOC 1

/* Define to 1 if you have the 'realpath' function. */
#define HAVE_REALPATH 1

/* Define to 1 if you have the 'regcomp' function. */
#define HAVE_REGCOMP 1

/* Define to 1 if you have the 'register_printf_modifier' function. */
#define HAVE_REGISTER_PRINTF_MODIFIER 1

/* Define to 1 if you have the 'register_printf_specifier' function. */
#define HAVE_REGISTER_PRINTF_SPECIFIER 1

/* Define to 1 if you have the 'rmdir' function. */
#define HAVE_RMDIR 1

/* Define to 1 if you have the 'select' function. */
#define HAVE_SELECT 1

/* Define to 1 if you have the 'socket' function. */
#define HAVE_SOCKET 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdio.h> header file. */
#define HAVE_STDIO_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the 'strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the 'strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the 'strdup' function. */
#define HAVE_STRDUP 1

/* Define to 1 if you have the 'strerror' function. */
#define HAVE_STRERROR 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the 'strncasecmp' function. */
#define HAVE_STRNCASECMP 1

/* Define to 1 if you have the 'strndup' function. */
#define HAVE_STRNDUP 1

/* Define to 1 if you have the 'strrchr' function. */
#define HAVE_STRRCHR 1

/* Define to 1 if you have the 'strstr' function. */
#define HAVE_STRSTR 1

/* Define to 1 if you have the 'strtol' function. */
#define HAVE_STRTOL 1

/* Define to 1 if you have the 'strtoul' function. */
#define HAVE_STRTOUL 1

/* Define to 1 if you have the 'strtoull' function. */
#define HAVE_STRTOULL 1

/* Define to 1 if 'i_atime_extra' is a member of 'struct ext2_inode_large'. */
/* #undef HAVE_STRUCT_EXT2_INODE_LARGE_I_ATIME_EXTRA */

/* Define to 1 if 'st_blksize' is a member of 'struct stat'. */
#define HAVE_STRUCT_STAT_ST_BLKSIZE 1

/* Define to 1 if 'st_rdev' is a member of 'struct stat'. */
#define HAVE_STRUCT_STAT_ST_RDEV 1

/* Define to 1 if you have the 'sysinfo' function. */
#define HAVE_SYSINFO 1

/* Define to 1 if you have the <sys/file.h> header file. */
#define HAVE_SYS_FILE_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/param.h> header file. */
#define HAVE_SYS_PARAM_H 1

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/statfs.h> header file. */
#define HAVE_SYS_STATFS_H 1

/* Define to 1 if you have the <sys/statvfs.h> header file. */
#define HAVE_SYS_STATVFS_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the 'uname' function. */
#define HAVE_UNAME 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to 1 if you have the 'vfork' function. */
#define HAVE_VFORK 1

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the <wchar.h> header file. */
#define HAVE_WCHAR_H 1

/* Define to 1 if 'fork' works. */
#define HAVE_WORKING_FORK 1

/* Define to 1 if 'vfork' works. */
#define HAVE_WORKING_VFORK 1

/* Define to 1 if the system has the type '_Bool'. */
#define HAVE__BOOL 1

/* Define to 1 if the system has the `__builtin_add_overflow' built-in
   function */
#define HAVE___BUILTIN_ADD_OVERFLOW 1

/* Define to 1 if the system has the `__builtin_cpu_supports__pclmul' built-in
   function */
#define HAVE___BUILTIN_CPU_SUPPORTS__PCLMUL 1

/* Define to 1 if the system has the `__builtin_mul_overflow' built-in
   function */
#define HAVE___BUILTIN_MUL_OVERFLOW 1

/* Define to 1 if the system has the `__builtin_sub_overflow' built-in
   function */
#define HAVE___BUILTIN_SUB_OVERFLOW 1

/* Define to 1 if 'lstat' dereferences a symlink specified with a trailing
   slash. */
#define LSTAT_FOLLOWS_SLASHED_SYMLINK 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "linux-btrfs@vger.kernel.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "btrfs-progs"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "btrfs-progs v6.17.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "btrfs-progs"

/* URL */
#define PACKAGE_URL "https://btrfs.readthedocs.io"

/* Define to the version of this package. */
#define PACKAGE_VERSION "v6.17.1"

/* Define to 1 if all of the C89 standard headers exist (not just the ones
   required in a freestanding environment). This macro is provided for
   backward compatibility; new code need not use it. */
#define STDC_HEADERS 1

/* Enable extensions on AIX, Interix, z/OS.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable general extensions on macOS.  */
#ifndef _DARWIN_C_SOURCE
# define _DARWIN_C_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable X/Open compliant socket functions that do not require linking
   with -lxnet on HP-UX 11.11.  */
#ifndef _HPUX_ALT_XOPEN_SOCKET_API
# define _HPUX_ALT_XOPEN_SOCKET_API 1
#endif
/* Identify the host operating system as Minix.
   This macro does not affect the system headers' behavior.
   A future release of Autoconf may stop defining this macro.  */
#ifndef _MINIX
/* # undef _MINIX */
#endif
/* Enable general extensions on NetBSD.
   Enable NetBSD compatibility extensions on Minix.  */
#ifndef _NETBSD_SOURCE
# define _NETBSD_SOURCE 1
#endif
/* Enable OpenBSD compatibility extensions on NetBSD.
   Oddly enough, this does nothing on OpenBSD.  */
#ifndef _OPENBSD_SOURCE
# define _OPENBSD_SOURCE 1
#endif
/* Define to 1 if needed for POSIX-compatible behavior.  */
#ifndef _POSIX_SOURCE
/* # undef _POSIX_SOURCE */
#endif
/* Define to 2 if needed for POSIX-compatible behavior.  */
#ifndef _POSIX_1_SOURCE
/* # undef _POSIX_1_SOURCE */
#endif
/* Enable POSIX-compatible threading on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-5:2014.  */
#ifndef __STDC_WANT_IEC_60559_ATTRIBS_EXT__
# define __STDC_WANT_IEC_60559_ATTRIBS_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-1:2014.  */
#ifndef __STDC_WANT_IEC_60559_BFP_EXT__
# define __STDC_WANT_IEC_60559_BFP_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-2:2015.  */
#ifndef __STDC_WANT_IEC_60559_DFP_EXT__
# define __STDC_WANT_IEC_60559_DFP_EXT__ 1
#endif
/* Enable extensions specified by C23 Annex F.  */
#ifndef __STDC_WANT_IEC_60559_EXT__
# define __STDC_WANT_IEC_60559_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TS 18661-4:2015.  */
#ifndef __STDC_WANT_IEC_60559_FUNCS_EXT__
# define __STDC_WANT_IEC_60559_FUNCS_EXT__ 1
#endif
/* Enable extensions specified by C23 Annex H and ISO/IEC TS 18661-3:2015.  */
#ifndef __STDC_WANT_IEC_60559_TYPES_EXT__
# define __STDC_WANT_IEC_60559_TYPES_EXT__ 1
#endif
/* Enable extensions specified by ISO/IEC TR 24731-2:2010.  */
#ifndef __STDC_WANT_LIB_EXT2__
# define __STDC_WANT_LIB_EXT2__ 1
#endif
/* Enable extensions specified by ISO/IEC 24747:2009.  */
#ifndef __STDC_WANT_MATH_SPEC_FUNCS__
# define __STDC_WANT_MATH_SPEC_FUNCS__ 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable X/Open extensions.  Define to 500 only if necessary
   to make mbstate_t available.  */
#ifndef _XOPEN_SOURCE
/* # undef _XOPEN_SOURCE */
#endif


/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define to 1 on platforms where this makes off_t a 64-bit type. */
/* #undef _LARGE_FILES */

/* Number of bits in time_t, on hosts where this is settable. */
/* #undef _TIME_BITS */

/* Define for Solaris 2.5.1 so the uint32_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT32_T */

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT64_T */

/* Define for Solaris 2.5.1 so the uint8_t typedef from <sys/synch.h>,
   <pthread.h>, or <semaphore.h> is not used. If the typedef were allowed, the
   #define below would cause a syntax error. */
/* #undef _UINT8_T */

/* Define to 1 on platforms where this makes time_t a 64-bit type. */
/* #undef __MINGW_USE_VC2005_COMPAT */

/* Define to empty if 'const' does not conform to ANSI C. */
/* #undef const */

/* For encode and decode tv_nsec info in ext2 inode */
/* #undef ext2fs_inode_includes */

/* Define as 'int' if <sys/types.h> doesn't define. */
/* #undef gid_t */

/* Define to the type of a signed integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int32_t */

/* Define to the type of a signed integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef int64_t */

/* Define to the type of a signed integer type of width exactly 8 bits if such
   a type exists and the standard includes do not define it. */
/* #undef int8_t */

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to 'int' if <sys/types.h> does not define. */
/* #undef mode_t */

/* Define to 'long int' if <sys/types.h> does not define. */
/* #undef off_t */

/* Define as a signed integer type capable of holding a process identifier. */
/* #undef pid_t */

/* Define to rpl_realloc if the replacement function should be used. */
/* #undef realloc */

/* Define as 'unsigned int' if <stddef.h> doesn't define. */
/* #undef size_t */

/* Define as 'int' if <sys/types.h> doesn't define. */
/* #undef ssize_t */

/* Define as 'int' if <sys/types.h> doesn't define. */
/* #undef uid_t */

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 32 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint32_t */

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint64_t */

/* Define to the type of an unsigned integer type of width exactly 8 bits if
   such a type exists and the standard includes do not define it. */
/* #undef uint8_t */

/* Define as 'fork' if 'vfork' does not work. */
/* #undef vfork */

/* Define to empty if the keyword 'volatile' does not work. Warning: valid
   code using 'volatile' can become incorrect without. Disable with care. */
/* #undef volatile */
