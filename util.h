#ifndef _UTIL_H_
#define _UTIL_H_

#include <dirent.h>
#include <regex.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <execinfo.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stddef.h>

#include "general-config.h"
#include "opt.h"


extern const char* progname;
#define NUM_CORES sysconf(_SC_NPROCESSORS_ONLN)

#define DBG_ASSERT(X, msg, args...) {		\
    if (__builtin_expect((!(X)), 0)) {		\
      errdie(msg, ##args);			\
    }						\
  }						\
    
#define PRINT(V_LEVEL, ...) {			\
    if (verbose >= V_LEVEL) {			\
      fprintf(stderr, __VA_ARGS__);		\
    }						\
  }						\




//error handling
#define MAX_CALL_STACK_OUTPUT 256
#define errdie(msg, args...) dieOnErrno(__FILE__, __LINE__, errno, msg, ##args)
void dieOnErrno(const char* fn, int32_t ln, int32_t en, const char* msg, ...);
void die(const char* fmt, ...);

//alloc wrappers
#define mycalloc(x, y) myCalloc((x), (y), __FILE__, __LINE__)
void* myCalloc(size_t nmemb, size_t size, const char* fname, const int32_t ln);

#define mymalloc(x) myMalloc((x), __FILE__, __LINE__)
void* myMalloc(size_t size, const char* fname, const int32_t ln);

#define myacalloc(x, y, z) myACalloc((x), (y), (z), __FILE__, __LINE__);
void* myACalloc(size_t alignment, size_t nmemb, size_t size, const char* fname, const int32_t ln);

#define myaalloc(x, y) myAAlloc((x), (y), __FILE__, __LINE__);
void* myAAlloc(size_t alignment, size_t size, const char* fname, const int32_t ln);

void myFree(void* ptr);

//thread creation
#define mypthread_create(w, x, y, z) myPthread_Create((w), (x), (y), (z), __FILE__, __LINE__)
void myPthread_Create(pthread_t* tid,
		      pthread_attr_t* attr,
		      void* (fun)(void*),
		      void* args,
		      const char* fname,
		      const int32_t ln);

//set core affinity for thread attr
#define myset_core(x, y) mySet_Core((x), (y), __FILE__, __LINE__)
void mySet_Core(pthread_attr_t* attr,
		size_t core,
		const char* fname,
		const int32_t ln);

#define mybarrierinit(x, y) myBarrierInit((x), (y), __FILE__, __LINE__);
void myBarrierInit(pthread_barrier_t* barrier,
		   int32_t nthreads,
		   const char* fname,
		   const int32_t ln);

//IO
#define myfexists(X) (access((X), F_OK) != -1)

#define myopen2(x, y) myOpen2((x), (y), __FILE__, __LINE__)
int32_t myOpen2(const char* path, int32_t flags, const char* fname, const int32_t ln);

#define myopen3(x, y, z) myOpen3((x), (y), (z), __FILE__, __LINE__)
int32_t myOpen3(const char* path, int32_t flags, mode_t mode, const char* fname, const int32_t ln);

#define myread(x, y, z) myRead((x), (y), (z), __FILE__, __LINE__)
int32_t myRead(int32_t fd, void* buf, size_t count, const char* fname, const int32_t ln);

#define mywrite(x, y, z) myWrite((x), (y), (z), __FILE__, __LINE__)
int32_t myWrite(int32_t fd, void* buf, size_t nbytes, const char* fname, const int32_t ln);

#define myfopen(x, y) myFOpen((x), (y), __FILE__, __LINE__)
FILE* myFOpen(const char* path, const char* mode, const char* fname, const int32_t ln);

#define myfread(w, x, y, z) myFRead((w), (x), (y), (z), __FILE__, __LINE__)
int32_t myFRead(void* ptr, size_t size, size_t nmemb, FILE* fp, const char* fname, const int32_t ln);

#define myfwrite(w, x, y, z) myFWrite((w), (x), (y), (z), __FILE__, __LINE__)
int32_t myFWrite(void* ptr, size_t size, size_t nmemb, FILE* fp, const char* fname, const int32_t ln);
//////////////////////////////////////////////////////////////////////
//string stuff
int32_t myStrCopy(char * dst, char * src);

//////////////////////////////////////////////////////////////////////
//statistics stuff
double getMedian(uint64_t* arr, int len);
double getMean(uint64_t* arr, int len);
double getSD(uint64_t* arr, int len);
double getVar(uint64_t* arr, int len);
double getMin(uint64_t* arr, int len) ;
double getMax(uint64_t* arr, int len) ;



//////////////////////////////////////////////////////////////////////
//timing stuff

#define unit_change 1000
#define ns_per_sec ((uint64_t)1000000000)
#define us_per_sec ((uint64_t)1000000)
#define ms_per_sec ((uint64_t)1000)

enum time_unit{s = 1, ms = ms_per_sec, us = us_per_sec, ns = ns_per_sec};
static const char time_unit_str[4][4] = {
  "s",
  "ms",
  "us",
  "ns"
};
uint64_t to_nsecs(struct timespec t) ;
uint64_t ns_diff(struct timespec t1, struct timespec t2) ;
uint64_t to_usecs(struct timespec t) ;
uint64_t us_diff(struct timespec t1, struct timespec t2) ;
uint64_t to_msecs(struct timespec t) ;
uint64_t ms_diff(struct timespec t1, struct timespec t2) ;
uint64_t to_secs(struct timespec t) ;
uint64_t s_diff(struct timespec t1, struct timespec t2) ;
double unit_convert(double time_ns, enum time_unit desired);
const char* unit_to_str(enum time_unit u);


//misc
#define sizeof_bits(X) ((sizeof(X)) << 3)

#endif
