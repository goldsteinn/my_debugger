//#define FRAME_AT_POINT(F, V) frameAtPoint(__FILE__, __FUNCTION__, __LINE__, TOTAL_STR_LEN(F) + TOTAL_TYPE_LEN(V) + TOTAL_TYPE_LEN(VAR_NAMES(V)), VAR_SIZES(V), PP_NARG(V), F, VAR_NAMES(V), V);
/*void
  frameAtPoint(const char * f,
  const char * fn,
  int32_t ln,
  int32_t total_len,
  const size_t * sizes,
  int32_t nargs,
  ...) {
  for(int i=0; i<nargs; i++){
  fprintf(stderr, "Size(%d) -> %zu\n", i, sizes[i]);
  }
  void * start_args = &nargs;
  va_list ap;
  va_start(ap, nargs);
  void * frame_data = mycalloc(nargs * 30, sizeof(size_t));
  int64_t * frame_data_iter = frame_data;
  int i;
  for(i=0; i< (nargs); i++){
  frame_data_iter[i] = (int64_t)va_arg(ap, char *);
  fprintf(stderr, "%d: %s\n", i, (char *)frame_data_iter[i]);
  }
  for(; i< (nargs * 2); i++){
  frame_data_iter[i] = (int64_t)va_arg(ap, char *);
  fprintf(stderr, "%d: %s\n", i, (char *)frame_data_iter[i]);

  }
  uint8_t val8;
  uint16_t val16;
  uint32_t val32;
  uint64_t val64;
  for(; i< (nargs * 3); i++){
  size_t next_size = sizes[i - (nargs * 2)];
  fprintf(stderr, "Size: %zu\n", next_size);
  switch(next_size) {
  case sizeof(uint8_t):
  val8 = va_arg(ap, uint32_t);
  memcpy(frame_data_iter + i, &val8, sizeof(uint8_t));
  break;
  case sizeof(uint16_t):
  val16 = va_arg(ap, uint32_t);
  memcpy(frame_data_iter + i, &val16, sizeof(uint16_t));

  break;
  case sizeof(uint32_t):
  val32 = va_arg(ap, uint32_t);
  memcpy(frame_data_iter + i, &val32, sizeof(uint32_t));
  break;
  case sizeof(uint64_t):
  val64 = va_arg(ap, uint64_t);
  memcpy(frame_data_iter + i, &val64, sizeof(uint64_t));
  break;
  default:
  fprintf(stderr, "Invalid size: %zu\n", next_size);
  }

  }

  va_end(ap);
  start_args += sizeof(int32_t);




  i = 0;
  fprintf(stderr, "FORMATS\n");
  for(; i<nargs;i++){
  fprintf(stderr, "%s\n", (char *)(frame_data_iter[i]));
  }
  fprintf(stderr, "NAMES\n");
  for(; i<nargs * 2;i++){
  fprintf(stderr, "%s\n", (char *)(frame_data_iter[i]));
  }
  fprintf(stderr, "VALS\n");
  for(; i<nargs * 3;i++){
  char format_str[32] = "";
  sprintf(format_str, "%s\n", (char *)(frame_data_iter[i - (nargs * 2)]));
  fprintf(stderr, format_str, frame_data_iter[i]);
  }
  }*/
