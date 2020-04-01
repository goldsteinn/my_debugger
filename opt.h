#ifndef _MY_OPT_H_
#define _MY_OPT_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <x86intrin.h>

#define MIN(x,y) (((x) < (y) ? (x) : (y)))
#define MAX(x,y) (((x) > (y) ? (x) : (y)))


int32_t fast_bytecmp_u(const void* a, const void* b, size_t len);
int32_t fast_bytecmp(const void* a, const void* b, size_t len);
void fast_bytecopy(void* dst, void* src, size_t len);
void fast_memset(void* loc, size_t val, size_t len);

int32_t ff1_asm(int32_t x);
int32_t ff0_asm(int32_t x);
int32_t fl1_asm(int32_t x);
int32_t fl0_asm(int32_t x);

int32_t ff1_64_asm(uint64_t x);
int32_t ff0_64_asm(uint64_t x);
int32_t fl1_64_asm(uint64_t x);
int32_t fl0_64_asm(uint64_t x);


uint32_t ulog2(uint32_t n);
uint32_t bitcount(uint64_t v);


#endif
