#include "debug.h"

void init_debugger() {
  global_debug_table = initTable();
}

//for multiple types like %g and %G, capitol is double, lowercase is
//float
void printFrame(frame_data_t * frame_data_s){
  uint32_t nargs = frame_data_s->nargs;
  uint32_t format_len = frame_data_s->format_len;
  uint32_t var_name_len = frame_data_s->var_name_len;

  char * file_name = frame_data_s->data;
  char * func_name = frame_data_s->data + strlen(file_name) + 1;
  char * format_data = func_name + strlen(func_name) + 1;
  char * name_data = format_data + format_len;
  void * var_data = name_data + var_name_len;

  char print_buf[MED_BUF_LEN] = "";

  //just to make printing a bit fancies
  memset(print_buf,
	 '-',
	 (PRINT_ALIGN - strlen("START") + 1) / 2);
  memcpy(print_buf + (PRINT_ALIGN - strlen("START") + 1) / 2,
	 "START",
	 strlen("START"));
  memset(print_buf + (PRINT_ALIGN + strlen("START") + 1) / 2,
	 '-',
	 (PRINT_ALIGN - strlen("START") + 1) / 2);

  //print file/function/set header
  fprintf(stderr, "%*s %s : %s : %d %*s\n",
	  PRINT_ALIGN, print_buf,
	  file_name, func_name, frame_data_s->line_num,
	  PRINT_ALIGN, print_buf);

  //print all symbols
  for(int i=0; i<nargs; i++){

    uint32_t format_str_len = strlen(format_data);
    uint32_t name_str_len = strlen(name_data);

    //create format string for symbols
    sprintf(print_buf,
	    "\t%s""%*s: %s\n",
	    name_data,
	    PRINT_ALIGN - name_str_len, "",
	    format_data);

    //undecided if always printing bytes is worthwhile
    /*	    "\t%*s: %s\n",
	    PRINT_ALIGN, "",
	    "%#018lx"); */



    
    
    //we would want switch here but cant really do that cuz need
    //strcmp. The reason we need this is we need to know how to
    //interpret the bytes of string (mostly this is just int vs
    //float).
    if(!strcmp(format_data, "%c")){
      fprintf(stderr, print_buf, *((char *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%d")){
      fprintf(stderr, print_buf, *((int *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%e")){
      fprintf(stderr, print_buf, *((float *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%E")){
      fprintf(stderr, print_buf, *((double *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%f")){
      fprintf(stderr, print_buf, *((float *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%g")){
      fprintf(stderr, print_buf, *((float *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%G")){
      fprintf(stderr, print_buf, *((double *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%hi")){
      fprintf(stderr, print_buf, *((short *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%hu")){
      fprintf(stderr, print_buf, *((unsigned short *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%i")){
      fprintf(stderr, print_buf, *((int *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%l")){
      fprintf(stderr, print_buf, *((long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%ld")){
      fprintf(stderr, print_buf, *((long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%li")){
      fprintf(stderr, print_buf, *((long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%lf")){
      fprintf(stderr, print_buf, *((double *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%Lf")){
      fprintf(stderr, print_buf, *((long double *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%lu")){
      fprintf(stderr, print_buf, *((unsigned long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%lli")){
      fprintf(stderr, print_buf, *((long long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%lld")){
      fprintf(stderr, print_buf, *((long long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%llu")){
      fprintf(stderr, print_buf, *((unsigned long long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%o")){
      fprintf(stderr, print_buf, *((long *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%p")){
      fprintf(stderr, print_buf, *((void **)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%s")){
      fprintf(stderr, print_buf, *((char **)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%u")){
      fprintf(stderr, print_buf, *((unsigned int *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%x")){
      fprintf(stderr, print_buf, *((unsigned int *)var_data)); // *(uint64_t *)var_data));
    }
    else if(!strcmp(format_data, "%X")){
      fprintf(stderr, print_buf, *((unsigned long *)var_data)); // *(uint64_t *)var_data));
    }
    else {
      errdie("Unsupported format at (%d) -> %s:%s\n",
	     i,
	     name_data,
	     format_data);
    }
    
    //seperated by \0 so this works...
    //likewise + 1 is for the \0
    format_data += format_str_len + 1;
    name_data += name_str_len + 1;
    
    var_data += MAX_DATA_TYPE_SIZE;
  }
  memset(print_buf + PRINT_ALIGN, 0, 2);
  memset(print_buf,
	 '-',
	 (PRINT_ALIGN - strlen("END") + 1) / 2);
  memcpy(print_buf + (PRINT_ALIGN - strlen("END") + 1) / 2,
	 "END",
	 strlen("END"));
  memset(print_buf + (PRINT_ALIGN + strlen("END") + 1) / 2,
	 '-',
	 (PRINT_ALIGN - strlen("END") + 1) / 2);

  fprintf(stderr, "%*s %s : %s : %d %*s\n",
	  PRINT_ALIGN, print_buf,
	  file_name, func_name, frame_data_s->line_num,
	  PRINT_ALIGN, print_buf);

}

uint8_t to_8(uint64_t t){
  t ^= t >> 32;
  t ^= t >> 16;
  t ^= t >> 8;
  return t & (0xff);
}


void addFrame(pthread_t tid, frame_data_t * frame_data_s) {
  assert(global_debug_table);
  node * ret = findNode(global_debug_table,
			tid,
			to_8(tid));
  if(!ret){
    frame_data_t ** frame_list = (frame_data_t **)mycalloc(N_FRAMES, sizeof(frame_data_t *));
    ret = (node *)mycalloc(1, sizeof(node));
    ret->key = tid;
    ret->val = (void *)frame_list;
    node * ret_in = addNode(global_debug_table, ret, to_8(tid));
    if(!low_bits_get(ret_in)){
      myFree(ret->val);
      myFree(ret);
      ret = ret_in;
    }
  }
  
  uint64_t frame_index = high_bits_set_INCR_atomic(ret->val) - 1;
  if(frame_index == N_FRAMES){
    low_bits_set_INCR_atomic(ret->val);
  }
  frame_data_t ** frame_list = get_ptr(ret->val);
  frame_list[frame_index] = frame_data_s;
}

void
getFrames(pthread_t tid){
  node * ret = findNode(global_debug_table,
			tid,
			to_8(tid));
  assert(ret);
  int32_t wrapparound = low_bits_get(ret->val);
  int32_t frame_index = high_bits_get(ret->val);
  frame_data_t ** frames = (frame_data_t **)get_ptr(ret->val);

  int32_t lb = wrapparound ? frame_index : 0;
  int32_t hb = wrapparound ? frame_index + N_FRAMES : frame_index;
  fprintf(stderr, "%d %d %d %d\n", wrapparound, frame_index, lb, hb);
  for(int32_t i = lb; i < hb; i++){
    printFrame(frames[i&(N_FRAMES - 1)]);
  }
}
