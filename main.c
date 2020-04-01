#include "util.h"
#include "opt.h"
#include "arg.h"
#include "debug.h"

int32_t verbose = 0;
#define Version "0.1"
static ArgOption args[] = {
  // Kind, 	  Method,		name,	    reqd,  variable,		help
  { KindOption,   Integer, 		"-v", 	    0,     &verbose, 		"Set verbosity level" },
  { KindHelp,     Help, 	"-h" },
  { KindEnd }
};
static ArgDefs argp = { args, "test", Version, NULL };


int main() {
  INIT_DEBUGGER;
  int64_t a = 7, b = 9, c = 3, d = 11;
  const char * ab = "hello";
  double f = 1.43, f2 = 43;
  int64_t aa = 55;

  for(int i = 0; i<100; i++){
    NEW_FRAME(FORMATS("%d", "%lf", "%d", "%d", "%d"),
	      VARS(a, f2, d, b, i));
  }
  GET_FRAMES;
}
