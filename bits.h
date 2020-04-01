#ifndef _BITS_H_
#define _BITS_H_

#include <stdint.h>


//assuming 48 bt VM address table
#define highBits 48
#define highBitsPtrMask ((1UL << highBits)-1)
//assuming 8 byte alignment
#define lowBits 3
#define lowBitsPtrMask (~((1UL<<lowBits)-1))
#define lowBitsMask 0x7
#define ptrMask (highBitsPtrMask&lowBitsPtrMask)


#define set_ptr(X, Y) setPtr((void **)(X), (void *)(Y))
void setPtr(void ** n, void * new_ptr);

#define get_ptr(X) getPtr((void *)(X))
void * getPtr(void * n);

#define low_bits_set_MASK(X, Y, Z) lowBitsSetMASK((void **)(X), (Y), (Z))
void lowBitsSetMASK(void ** n, uint32_t  bits, uint64_t mask);

#define low_bits_set_AND(X, Y) lowBitsSetAND((void **)(X), (Y))
void lowBitsSetAND(void ** n, uint32_t  bits);

#define low_bits_set_OR(X, Y) lowBitsSetOR((void **)(X), (Y))
void lowBitsSetOR(void ** n, uint32_t  bits);

#define low_bits_set_XOR(X, Y) lowBitsSetXOR((void **)(X), (Y))
void lowBitsSetXOR(void ** n, uint32_t  bits);

#define low_bits_set(X, Y) lowBitsSet((void **)(X), (Y))
void lowBitsSet(void ** n, uint32_t  bits);

#define low_bits_get(X) lowBitsGet((void *)(X))
uint32_t  lowBitsGet(void * n);

#define low_bits_get_ptr(X) lowBitsGetPtr((void *)(X))
void * lowBitsGetPtr(void * n);

#define high_bits_decr(X) highBitsDecr((void **)(X))
void highBitsDecr(void ** n);

#define high_bits_incr(X) highBitsIncr((void **)(X))
void highBitsIncr(void ** n);

#define high_bits_set_MASK(X, Y, Z) highBitsSetMASK((void **)(X), (Y), (Z))
void highBitsSetMASK(void ** n, uint64_t bits, uint64_t mask);

#define high_bits_set_AND(X, Y) highBitsSetAND((void **)(X), (Y))
void highBitsSetAND(void ** n, uint64_t bits);

#define high_bits_set_OR(X, Y) highBitsSetOR((void **)(X), (Y))
void highBitsSetOR(void ** n, uint64_t bits);

#define high_bits_set_XOR(X, Y) highBitsSetXOR((void **)(X), (Y)
void highBitsSetXOR(void ** n , uint64_t bits);

#define high_bits_set(X, Y) highBitsSet((void **)(X), (Y)
void highBitsSet(void ** n, uint16_t bits);

#define high_bits_get(X) highBitsGet((void *)(X))
uint16_t highBitsGet(void * n);

#define high_bits_get_ptr(X) highBitsGetPtr((void *)(X))
void * highBitsGetPtr(void * n);


//////////////////////////////////////////////////////////////////////
//atomic declarations

#define set_ptr_atomic(X, Y, Z) setPtr_atomic((void **)(&(X)), (void *)(Y), (void *)(Z))
uint32_t setPtr_atomic(void ** n, void * expec, void * new_ptr);

#define low_bits_set_atomic(X, Y, Z) lowBitsSet_atomic((void **)(&(X)), (void *)(Y), (Z))
uint32_t lowBitsSet_atomic(void ** n, void * expec, uint32_t bits);

#define low_bits_set_ADD_atomic(X, Y) lowBitsSetADD_atomic((void **)(&(X)), (Y))
uint64_t lowBitsSetADD_atomic(void ** n, uint32_t bits);

#define low_bits_set_INCR_atomic(X) lowBitsSetINCR_atomic((void **)(&(X)))
uint64_t lowBitsSetINCR_atomic(void ** n);

#define low_bits_set_DECR_atomic(X) lowBitsSetDECR_atomic((void **)(&(X)))
uint64_t lowBitsSetDECR_atomic(void ** n);

#define high_bits_set_atomic(X, Y, Z) highBitsSet_atomic((void **)(&(X)), (void *)(Y), (Z))
uint32_t highBitsSet_atomic(void ** n, void * expec, uint16_t bits);

#define high_bits_set_ADD_atomic(X, Y) highBitsSetADD_atomic((void **)(&(X)), (Y))
uint64_t highBitsSetADD_atomic(void ** n, uint64_t bits);

#define high_bits_set_INCR_atomic(X) highBitsSetINCR_atomic((void **)(&(X)))
uint64_t highBitsSetINCR_atomic(void ** n);

#define high_bits_set_DECR_atomic(X) highBitsSetDECR_atomic((void **)(&(X)))
uint64_t highBitsSetDECR_atomic(void ** n);

#endif
