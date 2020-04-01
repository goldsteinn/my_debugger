#include "hashtable.h"

//////////////////////////////////////////////////////////////////////
//hash function defined here
uint32_t murmur3_32(const uint8_t* key, size_t len, uint32_t seed){
  uint32_t h = seed;
  if (len > 3) {
    const uint32_t* key_x4 = (const uint32_t*) key;
    size_t i = len >> 2;
    do {

      uint32_t k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h = h * 5 + 0xe6546b64;
    } while (--i);
    key = (const uint8_t*) key_x4;
  }
  if (len & 3) {
    size_t i = len & 3;
    uint32_t k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}


uint16_t genTag(key_type t){
  t ^= t>>32;
  t ^= t>>16;
  return t&0xffff;
}


//helper function that returns sum of array up to size
static int
sumArr(int* arr, uint32_t size){
  uint32_t sum=0;
  size<<=log_int_ca;
  for(uint32_t i =0;i<size;i+=int_ca){

    sum+=arr[i];
  }

  return sum;
}

//creates short to store in high bits of node * with info on next slot locations
static uint16_t
createNHMask(uint32_t hv, uint32_t ltsize){
  hv >>= ltsize;
  uint32_t nbits = MIN(32-ltsize, slot_bits);
  hv &= ((1<<nbits)-1);
  //idea here is that if less than 12 bits need to have them start at
  //offset and change so that can use lower counter value (because
  //which bit to use is calculated inversely)
  hv <<= (slot_bits - nbits);
  uint16_t hb = nbits;
  hb |= (hv<<counter_bits);
  return hb;
}

//creates a subtable 
static subTable * 
createST(hashTable * head, uint32_t tsize){
  subTable * ht=(subTable *)myacalloc(cache_line_size, 1,sizeof(subTable));

  //calloc better for large allocations and not important for this one to be aligned
  ht->innerTable=(node **)calloc(tsize,sizeof(node *));
  ht->threadCopy=(uint32_t *)calloc(DEFAULT_SPREAD,cache_line_size);

  ht->logTableSize = ulog2(tsize);
  ht->tableSize=tsize;
  return ht;
}

  //frees a given subtable that was created for adddrop (that failed)
  static void 
    freeST(subTable * table){
  free(table->innerTable);
  free(table->threadCopy);
  free(table);
}


  //returns whether an node is found in a given subtable/overall hashtable. notIn means not
  //in the hashtable, s means in the hashtable and was found (return s so can get the value
  //of the item). unk means unknown if in table.
  static uint32_t 
    lookupQuery(subTable * ht, key_type key, uint32_t slot){

  //if find null slot know item is not in hashtable as would have been added there otherwise
  if(getNodePtr(ht->innerTable[slot])==NULL){
#ifdef mark_null
  if(getCopy(ht->innerTable[slot])){
  return unknown;
}
#endif //mark_null
  return not_in;
}

  //values equal return index so can access later
  else if(compare_keys(getKey(ht->innerTable[slot]), key)){
  return slot;
}
  
  //go another value, try next hash function
  return unknown;
}


  //returns whether an node is found in a given subtable/overall hashtable. notIn means not
  //in the hashtable, s means in the hashtable and was found (return s so can get the value
  //of the item). unk means unknown if in table.
  static uint32_t 
    lookupDelete(subTable * ht, key_type key, uint32_t slot){

  //if find null slot know item is not in hashtable as would have been added there otherwise
  if(getNodePtr(ht->innerTable[slot])==NULL){
#ifdef mark_null
  if(getCopy(ht->innerTable[slot])){
  return unknown;
}
#endif //mark_null
  return not_in;
}

  //values equal return index so can access later
  else if(compare_keys(getKey(ht->innerTable[slot]), key)){

  return slot;
}

  //go another value, try next hash function
  return unknown;
}


  //check if node for a given hashing vector is in a table. Returns in if node is already
  //in the table, s if the value is not in the table, and unk to try the next hash function
  static uint32_t
    lookup(hashTable * head, subTable * ht, node * n, uint32_t slot, uint32_t tid
#ifdef lazy_resize
    , uint32_t doCopy, uint32_t start
#endif //lazy_resize //
    ){
  
  //if found null slot return index so insert can try and put the node in the index
  if(getNodePtr(ht->innerTable[slot])==NULL){
#if defined mark_null && defined lazy_resize
  if(doCopy){
  if(setCopy(ht->innerTable[slot])){
  resize_node(head, ht, slot, tid);
}
}
#endif //mark_null && lazy_resize
  return slot;
}

  //if values equal case
  else if(compare_keys(getKey(ht->innerTable[slot]), getKey(n))){

  return in;
}

#ifdef lazy_resize
  //neither know if value is in or not, first check if this is smallest subtable and 
  //resizing is take place. If so move current item at subtable to next one.
  if(doCopy){
  if(setCopy(ht->innerTable[slot])){
  //succesfully set by this thread
  resize_node(head, ht, slot, tid);
}
}
#endif //lazy_resize
  return unknown;
}

  //function to add new subtable to hashtable if dont find open slot 
  static void
    addDrop(hashTable * head, subTable * toadd, uint64_t addSlot, node * n, uint32_t tid, uint32_t start){

  //make the array circular so add/delete continuous will always have space. Assumed that
  //resizer will keep up with insertion rate (this is probably provable as when resize is active
  //(which occurs when num subtables > threshhold each insert brings an old item with it). Also
  //if the diff between max/min subtables exceed a certain bound will start doubling again
  //irrelivant of delete/insert ratio

  //try and add new preallocated table (CAS so only one added)
  subTable * expected=NULL;
  uint32_t res = __atomic_compare_exchange(&head->tableArray[addSlot&(max_tables-1)],
    &expected,
    &toadd,
    1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
  if(res){
  //if succeeded try and update new max then insert item
  uint64_t newSize=addSlot+1;
  __atomic_compare_exchange(&head->end,
    &addSlot,
    &newSize,
    1,__ATOMIC_RELAXED, __ATOMIC_RELAXED);
}
  else{
  //if failed free subtable then try and update new max then insert item
  freeST(toadd);
  uint64_t newSize=addSlot+1;
  __atomic_compare_exchange(&head->end,
    &addSlot,
    &newSize,
    1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);

}
}


  //wrapper fro resizing node, will call add resize if necessary and
  //possible increment head's table index
  static void
    resize_node(hashTable * head, subTable * ht, uint32_t slot, uint32_t tid){

  //exstart/newstart for CAS that might take place
  uint64_t exStart=head->start;
  uint64_t newStart=exStart+1;

  //if item is not deleted copy up (this is how deleted items are culled as
  //they will not be copied)
#ifdef mark_null
  if(getNodePtr(ht->innerTable[slot])) {
#endif //mark_null

  addNode_resize(head,
    head->start+1,
    lowBitsGetPtr(ht->innerTable[slot]),
    tid,
    getKeyTag(getKey(ht->innerTable[slot]))
#ifdef next_hash
    ,slot
#endif //next_hash
    );
	
}
  //increment thread index
  incr(ht->threadCopy, tid, 1);
  //if all slots have been copied increment min subtable
if(ht->tableSize==sumArr(ht->threadCopy, DEFAULT_SPREAD)){
  if(__atomic_compare_exchange(&head->start,&exStart, &newStart,
			       1, __ATOMIC_RELAXED, __ATOMIC_RELAXED)){
    //this is the potential race condition. We use to arrays,
    //the toFree array will store old version that must be out
    //of use by the time it will be freed.  we know this because
    //before addrop if table diff > max_tables>>1 will throw
    //error let me know if you have a more elegant way of
    //handling it (we obviously can just have it spin there, but
    //is a SUPER SUPER SUPER unlikely event. I.e a thread would
    //have to be dead for the time it takes the hashtable to
    //inserts >2^32 * initsize entries.

  }
 }
}

//addnode resize we have different set of conditions that we can
// optimize around i.e should never find self deleted, dont need to
// recompute tag, and later will remove need for rehashing
uint32_t
addNode_resize(hashTable * head, uint32_t start, node * n, uint32_t tid, uint16_t tag
#ifdef next_hash
	       , uint32_t from_slot
#endif //next_hash
	       ){

  
  const uint32_t logReadsPerLine=DEFAULT_LOG_READS_PER_LINE;
  const uint32_t uBound=DEFAULT_READS_PER_LINE;
  const uint32_t ha=DEFAULT_HASH_ATTEMPTS;
  
  uint32_t localEnd=head->end;
  subTable * ht=NULL;


  //////////////////////////////////////////////////////////////////////
  //next hash optimization start
#ifdef next_hash
  from_slot >>= logReadsPerLine;
  from_slot <<= logReadsPerLine;

  uint32_t amount_next_bits = getCounter(n);
  uint32_t hb_index = slot_bits - amount_next_bits;
  amount_next_bits += start;
  subTable * prev = NULL;
  ht=head->tableArray[(start-1)&(max_tables-1)];
  for(uint32_t j=start;j<amount_next_bits;j++){
    //prev for checking if new slot bit is needed
    prev = ht;
    ht=head->tableArray[j];

    //tables wont grow in size if delete ratio is high enough this
    //means just reused old slot
    if(ht->tableSize != prev->tableSize){
      //	fprintf(stderr, "%d:: %x |= ((%x>>%d (%x) )&0x1) << %d -> ", j, from_slot, getNH(n, 0), hb_index, getNH(n, hb_index), (ht->logTableSize-1));
      from_slot |= (getNH(n, hb_index)&0x1)<<(ht->logTableSize-1);
      hb_index++;
    }
      
    //iterate through hash functions
    for(uint32_t i =0; i<ha; i++) {
#ifdef kway
      uint32_t slot=from_slot;
#endif //kway
      for(uint32_t c=tag;c<uBound+tag;c++){
	uint32_t res=lookup(head, ht, n,  slot+(c&(uBound-1)), tid
#ifdef lazy_resize
			    , 0, j
#endif //lazy_resize
			    );

	//lookup value in sub table
	if(res==unknown){ //unkown if in our not
	  continue;
	}
	else if(res==in){ //is in
	  return 0;
	}
	else{
	    
	  //if return was null (is available slot in sub table) try and add with CAS.
	  //if succeed return 1, else if value it lost to is item itself return. If neither
	  //continue trying to add

	  //update counter based on bits used
	  setCounter(n, slot_bits - hb_index);
	  node * expected=NULL;
	  uint32_t cmp= __atomic_compare_exchange((ht->innerTable+res),
						  &expected,
						  &n,
						  1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
	  if(cmp){
	    //if we win CAS increment insert count and return 1

	    return 1;
	  }
	  else{
	    //else check if value that was lost to is same, if not keep going, if is
	    //turn 0
	    if(compare_keys(getKey(ht->innerTable[res]),getKey(n))){
	      return 0;
	    }
	  }
	}
      } 
    }
      
    //update locacur

    localEnd=head->end;
    //could just drop through here but imo
    //should keep trying to reused bits
    if(j>=(head->end-1)){

      //if found no available slot in hashtable create new subtable and add it to hashtable
      //then try insertion again

      //next table size defaults to same size
      uint32_t nextTableSize = head->tableArray[(localEnd-1)&(max_tables-1)]->tableSize << 1;

      //create next subtables
      subTable * new_table=createST(head, nextTableSize);
      addDrop(head, new_table, localEnd, n, tid, start+1);
    }
  }

#endif //next_hash
  //////////////////////////////////////////////////////////////////////
  //next hash optimization end






  
#ifdef kway
  uint32_t buckets[DEFAULT_HASH_ATTEMPTS];
  for(uint32_t i =0;i<ha;i++){
    buckets[i]=hashFun(getKey(n), head->seeds[i]);
  }
#endif //kway
  while(1){
    //iterate through subtables
    //again is mod max_subtables
    for(uint32_t iter_j=start;iter_j<head->end;iter_j++){
      uint32_t j = iter_j&(max_tables-1);
      ht=head->tableArray[j];

      uint32_t tsizeMask=((ht->tableSize-1)>>logReadsPerLine)<<logReadsPerLine;
      

      //iterate through hash functions
      for(uint32_t i =0; i<ha; i++) {

#ifdef kway
	uint32_t slot=(buckets[i]&tsizeMask);
#endif //kway
	for(uint32_t c=tag;c<uBound+tag;c++){
	  uint32_t res=lookup(head, ht, n,  slot+(c&(uBound-1)), tid
#ifdef lazy_resize
			      , 0, j
#endif //lazy_resize
			      );


	  //lookup value in sub table
	
	  if(res==unknown){ //unkown if in our not
	    continue;
	  }
	  else if(res==in){ //is in
	    return 0;
	  }

	  else{
	    
	    //if return was null (is available slot in sub table) try and add with CAS.
	    //if succeed return 1, else if value it lost to is item itself return. If neither
	    //continue trying to add

#ifdef next_hash
#ifdef kway
	    uint16_t next_bits = createNHMask(buckets[0], ht->logTableSize);
#endif //kway
	    setNH(n, next_bits);
#endif //next_hash
	    
	    node * expected=NULL;
	    uint32_t cmp= __atomic_compare_exchange((ht->innerTable+res),
						    &expected,
						    &n,
						    1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
	    if(cmp){
	      //if we win CAS increment insert count and return 1

	      return 1;
	    }
	    else{
	      //else check if value that was lost to is same, if not keep going, if is
	      //turn 0
	      if(compare_keys(getKey(ht->innerTable[res]),getKey(n))){
		return 0;
	      }
	    }
	  }
	} //this is from the loop in ubound
      }
      //update locacur
      localEnd=head->end;
    }

    //if found no available slot in hashtable create new subtable and add it to hashtable
    //then try insertion again

    //next table size defaults to same size
    uint32_t nextTableSize = head->tableArray[(localEnd-1)&(max_tables-1)]->tableSize << 1; 

    //create next subtables
    subTable * new_table=createST(head, nextTableSize);
    addDrop(head, new_table, localEnd, n, tid, start+1);
    start=localEnd;
  }
}

//////////////////////////////////////////////////////////////////////
//end helpers


//api function user calls to query the table for a given node. Returns
//1 if found, 0 otherwise.
node *
findNode(hashTable * head, key_type key, uint32_t tid){
  subTable * ht=NULL;
  const uint32_t logReadsPerLine=DEFAULT_LOG_READS_PER_LINE;
  const uint32_t uBound=DEFAULT_READS_PER_LINE;

  const uint32_t ha=DEFAULT_HASH_ATTEMPTS;

#ifdef kway
  uint32_t buckets[DEFAULT_HASH_ATTEMPTS];
  for(uint32_t i =0;i<ha;i++){
    buckets[i]=hashFun(key, head->seeds[i]);
  }
#endif //kway

  uint16_t tag = hashTag(key);
  for(uint32_t iter_j=head->start;iter_j<head->end;iter_j++){
    uint32_t j = iter_j&(max_tables-1);
    ht=head->tableArray[j];

    uint32_t tsizeMask=((ht->tableSize-1)>>logReadsPerLine)<<logReadsPerLine;
    for(uint32_t i =0; i<ha; i++) {

#ifdef kway
      uint32_t slot=(buckets[i]&tsizeMask);
#endif //kway
      for(uint32_t c=tag;c<uBound+tag;c++){
	uint32_t res=lookupQuery(
				 ht, key,  slot+(c&(uBound-1)));
      
	if(res==unknown){ //unkown if in our not
	  continue;
	}
	if(res==not_in){
	  return NULL;		/* indicate not found */
	}
	return getNodePtr(ht->innerTable[res]);
      }
    }
  }
  //was never found
  return NULL;
}



//insert a new node into the table. Returns 0 if node is already present, 1 otherwise.
node *
addNode(hashTable * head, node * n, uint32_t tid){
  //use local max for adddroping subtables (otherwise race condition where 2 go to add
  //to slot n, one completes addition and increments head->end and then second one goes
  //and double adds. Won't affect correctness but best not have that happen.

  

  const uint32_t logReadsPerLine=DEFAULT_LOG_READS_PER_LINE;
  const uint32_t uBound=DEFAULT_READS_PER_LINE;
  const uint32_t ha=DEFAULT_HASH_ATTEMPTS;
  
#ifdef kway
  uint32_t buckets[DEFAULT_HASH_ATTEMPTS];
  for(uint32_t i =0;i<ha;i++){
    buckets[i]=hashFun(getKey(n), head->seeds[i]);
  }
#endif //kway
  
  uint16_t tag = hashTag(getKey(n));
  uint32_t localEnd=head->end;
  uint32_t start = head->start;
  subTable * ht=NULL;
  while(1){
    //iterate through subtables
    //again is mod max_subtables
    for(uint32_t j=start;j<head->end;j++){
      ht=head->tableArray[j];
      uint32_t tsizeMask=((ht->tableSize-1)>>logReadsPerLine)<<logReadsPerLine;
      

      //do copy if there is a new bigger subtable and currently in smallest subtable
      uint32_t doCopy=(j==(head->start&(max_tables-1)))&&(head->end-head->start>RESIZE_THRESHOLD);

      //iterate through hash functions
      for(uint32_t i =0; i<ha; i++) {

#ifdef kway
	uint32_t slot=(buckets[i]&tsizeMask);
#endif //kway
	for(uint32_t c=tag;c<uBound+tag;c++){
	uint32_t res=lookup(head, ht, n,  slot+(c&(uBound-1)), tid
#ifdef lazy_resize
			    , doCopy, j
#endif //lazy_resize
			    );
	//lookup value in sub table
	
	if(res==unknown){ //unkown if in our not
	  continue;
	}
	else if(res==in){ //is in
	  return set_return(ht->innerTable[slot+(c&(uBound-1))], 1);
	}

	else{
	  //start with absolutely known place for safe next bit
	  //calculation note that has to be before CAS otherwise can
	  //create race condition (can for example cause setCopy to
	  //fail spuriously)
	  //next hash stuff here
#ifdef next_hash
#ifdef kway
	  uint16_t next_bits = createNHMask(buckets[0], ht->logTableSize);
#endif //kway
	  setNH(n, next_bits);
#endif //next_hash
	    
	  //if return was null (is available slot in sub table) try and add with CAS.
	  //if succeed return 1, else if value it lost to is item itself return. If neither
	  //continue trying to add
	  node * expected=NULL;
	  uint32_t cmp= __atomic_compare_exchange((ht->innerTable+res),
						  &expected,
						  &n,
						  1, __ATOMIC_RELAXED, __ATOMIC_RELAXED);
	  if(cmp){
	    //if we win CAS increment insert count and return 1
	    return set_return(ht->innerTable[res], 1);
	  }
	  else{
	    //else check if value that was lost to is same, if not keep going, if is
	    //turn 0
	    if(
#ifdef mark_null
	       getNodePtr(ht->innerTable[res]) &&
#endif //mark_null
	       compare_keys(getKey(ht->innerTable[res]),getKey(n))){
	      return set_return(ht->innerTable[res], 0);
	    }
	  }
	}
      } //this is from the loop in ubound
    }
      //update locacur
      localEnd=head->end;
  }

    //if found no available slot in hashtable create new subtable and add it to hashtable
    //then try insertion again

    //next table size defaults to same size
    uint32_t nextTableSize = head->tableArray[(localEnd-1)&(max_tables-1)]->tableSize << 1;

    //create next subtables
    subTable * new_table=createST(head, nextTableSize);
    addDrop(head, new_table, localEnd, n, tid, start+1);
    start=localEnd;
}
  }

//initial hashtable. First table head will be null, after that will
//just reinitialize first table returns a pointer to the hashtable
hashTable * 
initTable() {
  hashTable * head=(hashTable *)myacalloc(cache_line_size, 1,sizeof(hashTable));
  head->tableArray=(subTable **)myacalloc(cache_line_size, max_tables,sizeof(subTable *));
  head->tableArray[0]=createST(head, DEFAULT_INIT_SIZE);
  head->end=1;
  head->start=0;
  for(uint32_t i=0; i<DEFAULT_HASH_ATTEMPTS; i++) {
    uint32_t rand_var = rand();
    memcpy(((void *)head) + offsetof(hashTable, seeds) + (i * sizeof(uint32_t)),
	   &rand_var,
	   sizeof(rand_var));
  }
  return head;
}

uint64_t
getStart(hashTable * head){
  return head->start;
}
