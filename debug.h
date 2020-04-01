#ifndef _DEBUG_H_
#define _DEBUG_H_

#include "util.h"
#include "hashtable.h"
#include "macro-helper.h"

#define INIT_DEBUGGER init_debugger()
static hashTable * global_debug_table = NULL;
void init_debugger();

typedef struct frame_data {
  uint32_t nargs;
  uint32_t format_len; 
  uint32_t var_name_len;
  uint32_t var_size_len;
  uint32_t line_num;
  char data[];
}frame_data_t;
#define META_DATA_SIZE offsetof(frame_data_t, data)

#define N_FRAMES 1024 //must be power of 2
#define MAX_DATA_TYPE_SIZE sizeof(uint64_t) //for now this should be enough
#define PRINT_ALIGN 9 //for clean printing. Variables with name > 16
		       //chars will be an issue

//to seperate variables and formats into seperate __VA_ARGS__
#define VARS(...) __VA_ARGS__
#define FORMATS(...) __VA_ARGS__

//to sum size of variable passed
#define TO_SIZEOF(X) sizeof(X)
#define TOTAL_TYPE_LEN(...) SUM(TO_SIZEOF, __VA_ARGS__)

//sum size of variable names/format specifiers passed
#define TO_STRLEN(X) strlen(X)
#define TOTAL_STR_LEN(...) SUM(TO_STRLEN, __VA_ARGS__)

//to get names of all variables passed as strings
#define TO_NAME(X) #X
#define VAR_NAMES(...) APPLY(TO_NAME, __VA_ARGS__)

//helper to combine names of variables 
#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)

//creates buffer to store all frame data
#define CREATE_BUFFER(ln, X) void * COMBINE(restricted_do_not_use, ln) = mycalloc(X + META_DATA_SIZE, sizeof(char));

//copies in strings/variables to frame buffer
#define TO_COPY(X, Y) memcpy((X), &(Y), sizeof(Y)); (X)+=MAX_DATA_TYPE_SIZE
#define TO_COPY_STR(X, Y) memcpy((X), (Y), strlen(Y)); (X)+=(strlen(Y) + 1)

//applies copy/copy_str to entire __VA_ARGS__ set
#define VAR_COPY(X, ...) APPLY_X(TO_COPY, X, __VA_ARGS__)
#define VAR_COPY_STR(X, ...) APPLY_X(TO_COPY_STR, X, __VA_ARGS__)

//stores the frame symbols. In macro because alot of the information
//becomes lost/harder to find once passed to function
#define FRAME_AT_POINT(f, fn, ln, F, V) {					\
  do {									\
    if(!global_debug_table){						\
      global_debug_table = initTable();					\
    }									\
    CREATE_BUFFER(ln, strlen(f) + 1 + strlen(fn) + 1 + TOTAL_STR_LEN(F) + TOTAL_TYPE_LEN(V) + TOTAL_STR_LEN(VAR_NAMES(V)) + PP_NARG(V) * 2); \
    frame_data_t * COMBINE(temp_restricted_do_not_use, ln) = (frame_data_t *)COMBINE(restricted_do_not_use, ln); \
    COMBINE(restricted_do_not_use, ln) += META_DATA_SIZE;		\
    memcpy(COMBINE(restricted_do_not_use, ln), f, strlen(f));		\
    COMBINE(restricted_do_not_use, ln) += strlen(f) + 1;		\
    memcpy(COMBINE(restricted_do_not_use, ln), fn, strlen(fn));		\
    COMBINE(restricted_do_not_use, ln) += strlen(fn) + 1;		\
    VAR_COPY_STR(COMBINE(restricted_do_not_use, ln), F);			\
    VAR_COPY_STR(COMBINE(restricted_do_not_use, ln), VAR_NAMES(V));	\
    VAR_COPY(COMBINE(restricted_do_not_use, ln), V);			\
    COMBINE(temp_restricted_do_not_use, ln)->nargs = PP_NARG(V);	\
    COMBINE(temp_restricted_do_not_use, ln)->format_len = TOTAL_STR_LEN(F) + PP_NARG(V);	\
    COMBINE(temp_restricted_do_not_use, ln)->var_name_len = TOTAL_STR_LEN(VAR_NAMES(V)) + PP_NARG(V); \
    COMBINE(temp_restricted_do_not_use, ln)->var_size_len = TOTAL_TYPE_LEN(V); \
    COMBINE(temp_restricted_do_not_use, ln)->line_num = ln;		\
    addFrame(pthread_self(), COMBINE(temp_restricted_do_not_use, ln));	\
  } while(0);								\
  }									\


//API function
#define NEW_FRAME(F, V) FRAME_AT_POINT(__FILE__, __FUNCTION__, __LINE__, FORMATS(F), FORMATS(V));
#define GET_FRAMES getFrames(pthread_self())

void printFrame(frame_data_t * frame_data_s);
void addFrame(pthread_t tid, frame_data_t * frame_data_s);
void getFrames(pthread_t tid);

#endif
