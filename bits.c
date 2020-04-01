#include "bits.h"

void * highBitsGetPtr(void * n){
  return (void *)((((uint64_t)n)&highBitsPtrMask));
}



uint16_t highBitsGet(void * n){
  return (uint16_t)(((uint64_t)n)>>highBits);
}


void highBitsSet(void ** n, uint16_t bits){
  uint64_t newPtr=bits;
  newPtr=newPtr<<highBits;
  newPtr|=(uint64_t)(highBitsGetPtr(*n));
  *n=(void *)newPtr;
}


void highBitsSetXOR(void ** n, uint64_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr^=(bits<<highBits);
  *n=(void *)newPtr;
}


void highBitsSetOR(void ** n, uint64_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr|=(bits<<highBits);
  *n=(void *)newPtr;
}


void highBitsSetAND(void ** n, uint64_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr&=(bits<<highBits);
  *n=(void *)newPtr;
}


void highBitsSetMASK(void ** n, uint64_t bits, uint64_t mask){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr&=~((mask)<<highBits);
  newPtr|=bits<<highBits;
  *n=(void *)newPtr;
}


void highBitsSetADD(void ** n, uint64_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr+=(bits<<highBits);
  *n=(void *)newPtr;
}


void highBitsSetINCR(void ** n){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr+=(1UL<<highBits);
  *n=(void *)newPtr;
}


//decrements highBits, this is unsafe
void highBitsSetDECR(void ** n){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr-=(1UL<<highBits);
  *n=(void *)newPtr;
}


void * lowBitsGetPtr(void * n){
  return (void *)(((uint64_t)(n))&lowBitsPtrMask);
}



uint32_t lowBitsGet(void * n){
  return ((uint64_t)n)&lowBitsMask;
}




//sets low bits of n to b, this is unsafe
void lowBitsSet(void ** n, uint32_t bits){
  *n=(void *)(((uint64_t)(lowBitsGetPtr(*n)))|bits);
}


void lowBitsSetXOR(void ** n, uint32_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr^=bits;
  *n=(void *)newPtr;
}


void lowBitsSetOR(void ** n, uint32_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr|=bits;
  *n=(void *)newPtr;
}


void lowBitsSetAND(void ** n, uint32_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr&=bits;
  *n=(void *)newPtr;
}


void lowBitsSetMASK(void ** n, uint32_t bits, uint64_t mask){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr&=(~(mask));
  newPtr|=bits;
  *n=(void *)newPtr;
}


void lowBitsSetADD(void ** n, uint32_t bits){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr+=bits;
  *n=(void *)newPtr;
}


void lowBitsSetINCR(void ** n){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr++;
  *n=(void *)newPtr;
}


void lowBitsSetDECR(void ** n){
  uint64_t newPtr=(uint64_t)(*n);
  newPtr++;
  *n=(void *)newPtr;
}


//gets valid ptr, use if both low and high bits in use
void * getPtr(void * n){
  return (void *)((((uint64_t)n)&ptrMask));
}


void setPtr(void ** n, void * new_ptr){
  uint32_t hb = highBitsGet(*n);
  uint32_t lb = lowBitsGet(*n);
  *n = new_ptr;
  highBitsSet(n, hb);
  lowBitsSet(n, lb);
}



//////////////////////////////////////////////////////////////////////
//starting atomic version, return 1 if successfully set, otherwise 0
//drops high bits info returns ptr


uint32_t highBitsSet_atomic(void ** n, void * expec, uint16_t bits){
  uint64_t newPtr=bits;
  newPtr=newPtr<<highBits;
  newPtr|=(uint64_t)(highBitsGetPtr(*n));
  return __atomic_compare_exchange(n, &expec, (void **)(&newPtr),
				   1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
}


uint64_t highBitsSetADD_atomic(void ** n, uint64_t bits){
  return (__atomic_add_fetch((uint64_t *)n, (bits<<highBits),
			     __ATOMIC_RELAXED)) >> highBits;
  

}


uint64_t highBitsSetINCR_atomic(void ** n){
  return (__atomic_add_fetch((uint64_t *)n, (1UL<<highBits),
			     __ATOMIC_RELAXED)) >> highBits;
}


//decrements highBits, this is unsafe
uint64_t highBitsSetDECR_atomic(void ** n){
  return (__atomic_sub_fetch((uint64_t *)n, (1UL<<highBits),
			     __ATOMIC_RELAXED)) >> highBits;

}





//sets low bits of n to b, this is unsafe
uint32_t lowBitsSet_atomic(void ** n, void * expec, uint32_t bits){
  uint64_t newPtr=bits;
  newPtr|=(uint64_t)(lowBitsGetPtr(*n));
  return __atomic_compare_exchange(n, &expec, (void **)(&newPtr),
				   1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);

}


uint64_t lowBitsSetADD_atomic(void ** n, uint32_t bits){
  return (__atomic_add_fetch((uint64_t *)n, (bits),
			     __ATOMIC_RELAXED))&lowBitsMask;
}

uint64_t lowBitsSetINCR_atomic(void ** n){
  return (__atomic_add_fetch((uint64_t *)n, (1UL),
		     __ATOMIC_RELAXED))&lowBitsMask;;
}


uint64_t lowBitsSetDECR_atomic(void ** n){
  return (__atomic_sub_fetch((uint64_t *)n, (1UL),
			     __ATOMIC_RELAXED))&lowBitsMask;;
}


uint32_t setPtr_atomic(void ** n, void * expec, void * new_ptr){
  uint32_t hb = highBitsGet(*n);
  uint32_t lb = lowBitsGet(*n);
  highBitsSet(&new_ptr, hb);
  lowBitsSet(&new_ptr, lb);
  return __atomic_compare_exchange(n, &expec, &new_ptr,
				   1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
}

