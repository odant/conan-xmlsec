/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <direct.h> header file. */
/* #undef HAVE_DIRECT_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define if you have POSIX threads libraries and header files. */
#define HAVE_PTHREAD 1

/* Have PTHREAD_PRIO_INHERIT. */
#define HAVE_PTHREAD_PRIO_INHERIT 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strcasecmp' function. */
#define HAVE_STRCASECMP 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "xml-security-c"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "dev@santuario.apache.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "XML-Security-C"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "XML-Security-C 2.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "xml-security-c"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "2.0.2"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "2.0.2"

/* Define to 1 if getcwd(NULL, 0) works */
#define XSEC_HAVE_GETCWD_DYN 1

/* Define if NSS is in use */
/* #undef XSEC_HAVE_NSS */

/* Define if OpenSSL is in use */
#define XSEC_HAVE_OPENSSL 1

/* Define to 1 if Xalan is available. */
/* #undef XSEC_HAVE_XALAN */

/* Define to 1 if OpenSSL has EVP_CIPHER_CTX_set_padding. */
#define XSEC_OPENSSL_CANSET_PADDING 1

/* Define to 1 if OpenSSL uses const input buffers. */
/* #undef XSEC_OPENSSL_CONST_BUFFERS */

/* Define to 1 if OpenSSL X509 API has const input buffer. */
#define XSEC_OPENSSL_D2IX509_CONST_BUFFER 1

/* Define to 1 if OpenSSL has full AES support. */
#define XSEC_OPENSSL_HAVE_AES 1

/* Define to 1 if OpenSSL has CRYPTO_cleanup_all_ex_data. */
#define XSEC_OPENSSL_HAVE_CRYPTO_CLEANUP_ALL_EX_DATA 1

/* Define to 1 if OpenSSL has EC support. */
#define XSEC_OPENSSL_HAVE_EC 1

/* Define to 1 if OpenSSL has EVP_PKEY_id function. */
#define XSEC_OPENSSL_HAVE_EVP_PKEY_ID 1

/* Define to 1 if OpenSSL has GCM support. */
#define XSEC_OPENSSL_HAVE_GCM 1

/* Define to 1 if OpenSSL has PKCS1_MGF1 function. */
#define XSEC_OPENSSL_HAVE_MGF1 1

/* Define to 1 if OpenSSL has SHA2 support. */
#define XSEC_OPENSSL_HAVE_SHA2 1

/* Define to 1 if XKMS support is included. */
#define XSEC_XKMS_ENABLED 1

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
/* #undef _FILE_OFFSET_BITS */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

#include <xsec/framework/XSECVersion.hpp>
