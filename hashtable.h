#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

#include "table-params.h"
#include "bits.h"
#include "opt.h"
#include "util.h"




//////////////////////////////////////////////////////////////////////
//arbitrary constants
#define EQUALS 1
#define NOT_EQUALS 0


//return values for checking table.  Returned by lookupQuery
#define not_in -3 //item is not in (used in query/delete)
#define in -1 //item is already in
#define unknown -2 //unkown keep looking

#define copy 0x1 //sets 1s bit of ent ptr for resizing


#define getCopy(X) lowBitsGet((void*)(X))
#define setCopy(X) lowBitsSet_atomic((void**)(&(X)), lowBitsGetPtr((void*)(X)), copy)


#define set_return(X, Y) ((node *)(((uint64_t)get_ptr(X)) | (Y)))

#define getNodePtr(X) (getPtr((void*)(X)))

#define getCounter(X) (highBitsGet((void*)(X))&(counter_bits_mask))
#define decrCounter(X) (highBitsSetDECR((void**)(&(X))))
#define incrCounter(X) (highBitsSetINCR((void**)(&(X))))
#define subCounter(X, Y) (highBitsSetSUB((void**)(&(X)), (Y)))
#define setCounter(X, Y) (highBitsSetMASK((void**)(&(X)), (Y), counter_bits_mask))

#define getNH(X, Y) ((highBitsGet((void*)(X)))>>(counter_bits+(Y)))
#define setNH(X, Y) (highBitsSet((void**)(&(X)), (Y)))


//table config is necessary
//////////////////////////////////////////////////////////////////////
//config for type hashtable will use

typedef struct frame_node{
  pthread_t key;
  void * val; //this is a frame_data_t ** defined in debug
}frame_node;

//define node and key type
typedef pthread_t key_type;
typedef struct frame_node node;


uint16_t genTag(pthread_t t);

//nothing for ints
#define hashTag(X) (genTag(X))

//compare key comparison
#define compare_keys(X, Y) ((X)==(Y))

#define getKey(X) (((node *)getNodePtr(X))->key)
#define getVal(X) ((node *)(getNodePtr(X))->thread_frames)
#define getKeyLen(X) sizeof(pthread_t)

//hash function for int (key is passed as a ptr)
#define hashFun(X, Y) murmur3_32((const uint8_t*)(&(X)), sizeof(pthread_t), (Y));
#define getKeyTag(X) genTag(X)


//////////////////////////////////////////////////////////////////////
//Config for hash function table will use
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed);
//////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////
//table structs
//a sub table (this should be hidden)
typedef struct subTable {
  node ** innerTable; //rows (table itself)

  //for keeping track of when all items from min sub table have been moved
  uint32_t * threadCopy;
  
  uint32_t tableSize; //size
  uint32_t logTableSize;
} subTable;


// head of cache: this is the main hahstable
typedef struct hashTable{
  subTable ** tableArray; //array of tables
  uint64_t start;
  uint64_t end; //current max index (max exclusive)
  const uint32_t seeds[DEFAULT_HASH_ATTEMPTS];
} hashTable; //worth it to aligned alloc this


//////////////////////////////////////////////////////////////////////
#ifdef cache
#define incr(X, Y, Z) (X)[((Y)<<log_int_ca)]+=(Z)
#define decr(X, Y, Z) (X)[((Y)<<log_int_ca)]-=(Z)
#else
#define incr(X, Y, Z) (X)[(Y)]+=(Z)
#define decr(X, Y, Z) (X)[(Y)]-=(Z)
#endif //cache



//wrapper fro resizing node, will call add resize if necessary and
//possible increment head's table index
static void
resize_node(hashTable * head, subTable * ht, uint32_t slot, uint32_t tid);
  
//lookup function in insertTrial to check a given inner table
static uint32_t
lookup(hashTable * head, subTable * ht, node * n,uint32_t s, uint32_t tid
#ifdef lazy_resize
       , uint32_t doCopy, uint32_t start
#endif //lazy_resize
       );

//lookup for queries
static uint32_t
lookupQuery(subTable * ht, key_type key, uint32_t s);



//addnode resize we have different set of conditions that we can
// optimize around i.e should never find self deleted, dont need to
// recompute tag, and later will remove need for rehashing
uint32_t
addNode_resize(hashTable * head,  uint32_t start, node * n, uint32_t tid, uint16_t tag
#ifdef next_hash
	       , uint32_t from_slot
#endif //next_hash
	       );
//////////////////////////////////////////////////////////////////////
//general api start
// return 1 if inserted, 0 if already there

node *
addNode(hashTable * head,
	node * n,
	uint32_t tid);

// see if node is in the table
node *
findNode(hashTable * table,
	 key_type key,
	 uint32_t tid);

//free all nodes. returns num nodes in table
double
freeAll(hashTable * table,
	uint32_t verbose,
	uint32_t full);

// initialize a new main hashtable
hashTable *
initTable();


uint64_t
getStart(hashTable * head);

//////////////////////////////////////////////////////////////////////
//general api end



#endif
