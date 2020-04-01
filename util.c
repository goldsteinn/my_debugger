#include "util.h"


const char* progname;

//////////////////////////////////////////////////////////////////////

//print stack trace

void stackTrace(){
  void * callstack[MAX_CALL_STACK_OUTPUT];
  
  int32_t nframes = backtrace(callstack, MAX_CALL_STACK_OUTPUT);
  char ** frame_strs = backtrace_symbols(callstack, nframes);

  //With regards to addr2line: you could technically leave this on w/
  //optimizations on (i.e -O1/-O2/-O3) but will be next to meaningless
  //(more likely counter productive due to the error margin). -g
  //argument must be set by the compiler for this to have any meaning
  //as well (compiler doesnt leave any flags for that though)
#ifndef __OPTIMIZE__
  char cmdline_addr2line[MED_BUF_LEN] = "";
  int32_t offset_start = 0, offset_end = 0;
#endif

  for (int32_t i=0; i<nframes; i++) {
    fprintf(stderr, "%s\n", frame_strs[i]);

#ifndef __OPTIMIZE__
    offset_start = 0;
    while(frame_strs[i][offset_start] != '\0' &&
	  frame_strs[i][offset_start] != '+') {
      offset_start++;
    }
    
    offset_end = offset_start;
    
    while(frame_strs[i][offset_end] != '\0' &&
	  frame_strs[i][offset_end] != ')') {
      offset_end++;
    }

    if(frame_strs[i][offset_start] == '\0' ||
       frame_strs[i][offset_end] == '\0') {
      fprintf(stderr, "\tCant print line\n");
    }
    else {
      frame_strs[i][offset_end] = '\0';
      sprintf(cmdline_addr2line,
	      "addr2line -e %s -j .text %s",
	      progname + strlen("./"), 
	      frame_strs[i] + offset_start);
      
      //think makes the printout cleaner to have line indented
      fprintf(stderr, "\t"); 
      if(system(cmdline_addr2line) == -1){
	fprintf(stderr, "\tCant print line\n");
      }
    }
#endif
  }
  myFree(frame_strs);
}




//error functions
void dieOnErrno(const char* fn, int32_t ln, int32_t en, const char* msg, ...) {
  va_list ap;
  va_start(ap, msg);
  fprintf(stderr, "%s:%d:", __FILE__, __LINE__);
  vfprintf(stderr, msg, ap);
  va_end (ap);
  fprintf(stderr, "\t%d:%s\n", en, strerror(en));
  fprintf(stderr, "------------- Stack Trace Start ---------------\n");
  stackTrace();
  fprintf(stderr, "------------- Stack Trace End ---------------\n");
  exit(-1);
}

  
void die(const char* fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "%s: ", progname);
  vfprintf(stderr, fmt, ap);
  va_end (ap);
  fprintf(stderr, "\n");
  exit(-1);
}

//////////////////////////////////////////////////////////////////////
//alloc stuff
void* myCalloc(size_t nmemb, size_t size, const char* fname, const int32_t ln) {
  void* p = calloc(nmemb, size);
  if(!p){
    errdie("Failed to allocate memory at %s:%d", fname, ln);
  }
  return p;
}

void* myMalloc(size_t size, const char* fname, const int32_t ln) {
  void* p = malloc(size);
  if(!p){
    errdie("Failed to allocate memory at %s:%d", fname, ln);
  }
  return p;
}

void* myAAlloc(size_t alignment, size_t size, const char* fname, const int32_t ln){
  void* p = aligned_alloc(alignment, size);
  if(!p){
    errdie("Failed to allocate memory at %s:%d", fname, ln);
  }
  return p;
}

void* myACalloc(size_t alignment, size_t nmemb, size_t size, const char* fname, const int32_t ln){
  void* p = aligned_alloc(alignment, nmemb * size);
  if(!p){
    errdie("Failed to allocate memory at %s:%d", fname, ln);
  }
  fast_memset(p, 0, nmemb*size);
  return p;
}

void myFree(void* ptr){
  if(ptr){
    free(ptr);
  }
}

//////////////////////////////////////////////////////////////////////
//thread stuff
void mySet_Core(pthread_attr_t* attr,
		size_t core,
		const char* fname,
		const int32_t ln){
  if(pthread_attr_init(attr)){
    errdie("Failed to init thread attr %s:%d\n", fname, ln);
  }
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core, &cpuset);
  if(pthread_attr_setaffinity_np(attr, sizeof(cpu_set_t), &cpuset)){
    errdie("Failed to set core affinity %s:%d\n", fname, ln);
  }
}

void myPthread_Create(pthread_t* tid,
		      pthread_attr_t* attr,
		      void* (fun)(void*),
		      void* args,
		      const char* fname,
		      const int32_t ln){
  if(pthread_create(tid, attr, fun, args)){
    errdie("Failed to create thread at %s:%d\n", fname, ln);
  }
}

void myBarrierInit(pthread_barrier_t* barrier,
		   int32_t nthreads,
		   const char* fname,
		   const int32_t ln){
  if(pthread_barrier_init(barrier, NULL, nthreads)){
    errdie("Failed to init barrier at %s:%d\n", fname, ln);
  }
}

//C IO fd
int32_t myOpen2(const char* path, int32_t flags, const char* fname, const int32_t ln){
  int32_t fd = open(path, flags);
  if(fd == -1){
    errdie("Failed to open %s at %s:%d\n", path, fname, ln);
  }
  return fd;
}

int32_t myOpen3(const char* path, int32_t flags, mode_t mode, const char* fname, const int32_t ln){
  int32_t fd = open(path, flags, mode);
  if(fd==-1){
    errdie("Failed to open %s at %s:%d\n", path, fname, ln);
  }
  return fd;
}

int32_t myRead(int32_t fd, void* buf, size_t count, const char* fname, const int32_t ln){
  int32_t result = read(fd, buf, count);
  if(result == -1){
    errdie("Failed to read at %s:%d\n", fname, ln);
  }
  return result;
}

int32_t myWrite(int32_t fd, void* buf, size_t nbytes, const char* fname, const int32_t ln){
  int32_t result = write(fd, buf, nbytes);
  if(result == -1){
    errdie("Failed to write at %s:%d\n", fname, ln);
  }
  return result;
}

//C IO fp
FILE* myFOpen(const char* path, const char* mode, const char* fname, const int32_t ln){
  FILE* fp = fopen(path, mode);
  if(!fp){
    errdie("Failed to open %s at %s:%d\n", path, fname, ln);
  }
  return fp;
}

int32_t myFRead(void* ptr, size_t size, size_t nmemb, FILE* fp, const char* fname, const int32_t ln){
  int32_t result = fread(ptr, size, nmemb, fp);
  if(!result){
    errdie("Failed to read at %s:%d\n", fname, ln);
  }
  return result;
}

int32_t myFWrite(void* ptr, size_t size, size_t nmemb, FILE* fp, const char* fname, const int32_t ln){
  int32_t result = fwrite(ptr, size, nmemb, fp);
  if(!result){
    errdie("Failed to read at %s:%d\n", fname, ln);
  }
  return result;
}

//////////////////////////////////////////////////////////////////////
//strings stuff
int32_t
myStrCopy(char * dst, char * src) {
  int32_t i = 0;
  while(src[i]) {
    dst[i] = src[i];
    i++;
  }
  return i;
}

//////////////////////////////////////////////////////////////////////
//stats stuff
static int32_t dblcomp(const void* a, const void *b) {
  return *(double*)b - *(double*)a;
}

double getMedian(uint64_t* arr, int len){
#ifdef USAGE_CHECK
  if((!len) || (!arr)){
    die("Bad len or array: %p[%d]\n", arr, len);
  }
#endif
  double* arr_dbl = (double*)mycalloc(len, sizeof(double));
  for(int32_t i=0;i<len;i++){
    arr_dbl[i] = (double)arr[i];
  }
  qsort(arr_dbl, len, sizeof(double), dblcomp);
  double median;
  if (len&0x1) {
    median = arr_dbl[len >> 1];
  }
  else {
    median = (arr_dbl[(len-1) >> 1] + arr_dbl[((len-1) >> 1)+1])/2.0;
  }
  free(arr_dbl);
  return median;
}

double getMean(uint64_t* arr, int len){
#ifdef USAGE_CHECK
  if((!len) || (!arr)){
    errdie("Bad len or array: %p[%d]\n", arr, len);
  }
#endif
  double total = 0.0;
  for (int32_t i=0; i<len; i++) {
    total += (double)arr[i];
  }
  return total/(double)len;
}

double getSD(uint64_t* arr, int len){
#ifdef USAGE_CHECK
  if((!len) || (!arr)){
    errdie("Bad len or array: %p[%d]\n", arr, len);
  }
#endif
  if(len==1){
    return 0.0;
  }
  double sum = 0.0;
  double mean;
  double sd = 0.0;
  for(int32_t i=0; i<len; i++){
    sum += (double)arr[i];
  }
  mean = sum/(double)len;

  for(int32_t i=0; i<len; i++){
    sd += pow(arr[i] - mean, 2);
  }
  return sqrt(sd/(len-1));
}

double getVar(uint64_t* arr, int len){
#ifdef USAGE_CHECK
  if((!len) || (!arr)){
    errdie("Bad len or array: %p[%d]\n", arr, len);
  }
#endif

  double sum = 0.0;
  double mean;
  double sd = 0.0;
  for(int32_t i=0; i<len; i++){
    sum += (double)arr[i];
  }
  mean = sum/(double)len;

  for(int32_t i=0; i<len; i++){
    sd += pow(arr[i] - mean, 2);
  }
  return sqrt(sd/(len));
}

  
double getMin(uint64_t* arr, int len) {
#ifdef USAGE_CHECK
  if(!len || !arr){
    errdie("Bad len or array: %p[%d]\n", arr, len);
  }
#endif
  double m = arr[0];
  for(int32_t i=0; i<len; i++)
    if (m > (double)arr[i]){
      m = (double)arr[i];
    }
  return m;
}


double getMax(uint64_t* arr, int len) {
#ifdef USAGE_CHECK
  if(!len || !arr){
    errdie("Bad len or array: %p[%d]\n", arr, len);
  }
#endif
  double m = arr[0];
  for(int32_t i=0; i<len; i++)
    if (m < (double)arr[i]){
      m = (double)arr[i];
    }
  return m;
}

//////////////////////////////////////////////////////////////////////
//Timing unit conversion stuff

uint64_t to_nsecs(struct timespec t) {
  return (t.tv_sec * ns_per_sec + (uint64_t)t.tv_nsec);
}

uint64_t ns_diff(struct timespec t1, struct timespec t2) {
  return (to_nsecs(t1) - to_nsecs(t2));
}


uint64_t to_usecs(struct timespec t) {
  return to_nsecs(t)/unit_change;
}

uint64_t us_diff(struct timespec t1, struct timespec t2) {
  return (to_usecs(t1) - to_usecs(t2));
}


uint64_t to_msecs(struct timespec t) {
  return to_nsecs(t)/(unit_change*unit_change);
}

uint64_t ms_diff(struct timespec t1, struct timespec t2) {
  return (to_msecs(t1) - to_msecs(t2));
}


uint64_t to_secs(struct timespec t) {
  return to_nsecs(t)/(unit_change*unit_change*unit_change);
}

uint64_t s_diff(struct timespec t1, struct timespec t2) {
  return (to_secs(t1) - to_secs(t2));
}

double unit_convert(double time_ns, enum time_unit desired){
  double conversion = (((double)ns_per_sec)/(double)desired);
  return time_ns/conversion;
}

const char* unit_to_str(enum time_unit u){
  int32_t index = 0;
  uint64_t s = u;
  while(s/unit_change){
    s = s/unit_change;
    index++;
  }
  return time_unit_str[index];
}

//////////////////////////////////////////////////////////////////////

