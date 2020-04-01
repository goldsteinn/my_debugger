#include "opt.h"


void fast_memset(void* loc, size_t val, size_t len){
  size_t len_8 = (len>>3);
  
  uint64_t* loc_ul = (uint64_t*)loc;
  int32_t i;
  for(i=0;i<len_8;i++){
    loc_ul[i] = val;
  }
  i <<= 3;
  //if mem for some reason is not 8 byte aligned
  unsigned char* loc_ch = (unsigned char*)loc;
  char* val_bytes = (char*)(&val);
  for(;i<len;i++){
    loc_ch[i] = val_bytes[i&0x7];
  }
}



void fast_bytecopy(void* dst, void* src, size_t len){
  uint64_t* dst_ul = (uint64_t*)dst;
  uint64_t* src_ul = (uint64_t*)src;
  size_t len_8 = len>>3;
  int32_t i=0;
  
  for(i=0;i<len_8;i++){
    dst_ul[i]=src_ul[i];
  }
  i <<= 3;
  unsigned char* dst_ch = (unsigned char*)dst;
  unsigned char* src_ch = (unsigned char*)src;
  for(;i<len;i++){
    dst_ch[i]=src_ch[i];
  }
}

int32_t fast_bytecmp_u(const void* a, const void* b, size_t len){
  uint64_t* a_ul = (uint64_t*)a;
  uint64_t* b_ul = (uint64_t*)b;
  int32_t i;
  size_t len_8 = len>>3;
  for(i=0;i<len_8;i++){
    if(a_ul[i]!=b_ul[i]){
      return a_ul[i] > b_ul[i] ? 1 : -1;

    }
  }
  i <<= 3;
  unsigned char* a_ch = (unsigned char*)a;
  unsigned char* b_ch = (unsigned char*)b;
  for(;i<len;i++){
    if(a_ch[i]!=b_ch[i]){
      return a_ch[i] > b_ch[i] ? 1 : -1;
      }
  }
  return 0;
}


int32_t fast_bytecmp(const void* a, const void* b, size_t len){
  int64_t* a_ul = (int64_t*)a;
  int64_t* b_ul = (int64_t*)b;
  int32_t i;
  size_t len_8 = len>>3;
  for(i=0;i<len_8;i++){
    if(a_ul[i]!=b_ul[i]){
      return a_ul[i] > b_ul[i] ? 1 : -1;

    }
  }
  i <<= 3;
  char* a_ch = (char*)a;
  char* b_ch = (char*)b;
  for(;i<len;i++){
    if(a_ch[i]!=b_ch[i]){
      return a_ch[i] > b_ch[i] ? 1 : -1;
      }
  }
  return 0;
}

#ifdef x86_64
int32_t ff1_asm(int32_t x){
  int32_t loc;
  __asm__("bsf %1, %0" : "=r" (loc) : "rm" (x));
  return loc;
}
int32_t ff0_asm(int32_t x){
  int32_t loc;
  __asm__("bsf %1, %0" : "=r" (loc) : "rm" ((~x)));
  return loc;
}

int32_t fl1_asm(int32_t x){
  int32_t loc;
  __asm__("bsr %1, %0" : "=r" (loc) : "rm" (x));
  return loc;
}

int32_t fl0_asm(int32_t x){
  int32_t loc;
  __asm__("bsr %1, %0" : "=r" (loc) : "rm" ((~x)));
  return loc;  
}


int32_t ff1_64_asm(uint64_t x){
  int32_t bot_half = x, top_half = (x >> 32);
  return bot_half ? ff1_asm(bot_half) : (32 + ff1_asm(top_half));
}
int32_t ff0_64_asm(uint64_t x){
  x = ~x;
  int32_t bot_half = x, top_half = (x >> 32);
  return bot_half ? ff1_asm(bot_half) : (32 + ff1_asm(top_half));
}

int32_t fl1_64_asm(uint64_t x){
  int32_t bot_half = x, top_half = (x >> 32);
  return top_half ? fl1_asm(top_half) : (32 + fl1_asm(bot_half));
}

int32_t fl0_64_asm(uint64_t x){
  x = ~x;
  int32_t bot_half = x, top_half = (x >> 32);
  return top_half ? fl1_asm(top_half) : (32 + fl1_asm(bot_half));
}
#endif

#ifdef armv7l
int32_t ff1_asm(int32_t x){
  return fl1_asm(x & (~x + 1));
}

int32_t ff0_asm(int32_t x){
  return ff1_asm(~x);
}

int32_t fl1_asm(int32_t x){
  uint32_t s, t, n = x;
  t = (n > 0xffff) << 4; n >>= t;
  s = (n > 0xff  ) << 3; n >>= s, t |= s;
  s = (n > 0xf   ) << 2; n >>= s, t |= s;
  s = (n > 0x3   ) << 1; n >>= s, t |= s;
  return (t | (n >> 1));
}

int32_t fl0_asm(int32_t x){
  return fl1_asm(~x);
}
#endif



uint32_t ulog2(uint32_t n){
  uint32_t s, t;
  t = (n > 0xffff) << 4; n >>= t;
  s = (n > 0xff  ) << 3; n >>= s, t |= s;
  s = (n > 0xf   ) << 2; n >>= s, t |= s;
  s = (n > 0x3   ) << 1; n >>= s, t |= s;
  return (t | (n >> 1));
}

uint32_t bitcount(uint64_t v){
  uint64_t c; 
  c = v - ((v >> 1) & 0x5555555555555555UL);
  c = ((c >> 2) & 0x3333333333333333UL) + (c & 0x3333333333333333UL);
  c = ((c >> 4) + c) & 0x0F0F0F0F0F0F0F0FUL;
  c = ((c >> 8) + c) & 0x00FF00FF00FF00FFUL;
  c = ((c >> 16) + c) & 0x0000FFFF0000FFFFUL;
  c = ((c >> 32) + c) & 0x00000000FFFFFFFFUL;
  return c;
}

