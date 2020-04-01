#ifndef _GENERAL_CONFIG_H_
#define _GENERAL_CONFIG_H_

#ifndef NCORES
#define NCORES ((uint64_t)sysconf(_SC_NPROCESSORS_ONLN))
#endif
#ifndef L1_CACHE_LINE_SIZE
#define L1_CACHE_LINE_SIZE 64
#endif


#define DEBUG_MODE
#define USAGE_CHECK

#define ERROR_VERBOSE 0
#define LOW_VERBOSE 1
#define MED_VERBOSE 2
#define HIGH_VERBOSE 3

#if defined ERROR_VERBOSE || LOW_VERBOSE || defined MED_VERBOSE || defined HIGH_VERBOSE
#define VERBOSE
#endif

extern int32_t verbose;

#define MAX_SPECIFIER_SIZE 4
#define SMALL_READ_LEN 16
#define SMALL_PATH_LEN 128
#define MED_PATH_LEN 256
#define BIG_PATH_LEN 512


//this is not the most generic this, i.e non-intel 64 bit arch will
//will have wrong print defined. This wont break the program but will
//throw warnings at compile time.
#if defined(__x86_64__)
#define FORMAT_64 "%lu"
#else
#define FORMAT_64 "%llu"
#endif


#define SMALL_BUF_LEN 32
#define MED_BUF_LEN 128
#define BIG_BUF_LEN 512


#endif
