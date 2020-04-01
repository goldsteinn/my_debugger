#ifndef _TABLE_PARAMS_H
#define _TABLE_PARAMS_H_

//setting cache info
#define L1_Cache_Line_Size 64
#define L1_Log_Cache_Line_size 6

#define cache_line_size L1_Cache_Line_Size
#define log_cls L1_Log_Cache_Line_size

#define ent_per_line cache_line_size/sizeof(node*)
//this is step length for iterating through
//int array where each index is on its own cache line
#define int_ca (cache_line_size>>2)
#define log_int_ca (log_cls-2)

#define num_high_bits 16

#define cache //turns on cache optimizations
#define kway //turns on multiple hash functions
#define next_hash //turns on storing next hash optimization
#define mark_null //table being copied will not add to null items (if they can mark it)
#define lazy_resize //turns on lazy resizing (necessary for functioning delete)

#define DEFAULT_INIT_SIZE (1024)
#define DEFAULT_SPREAD (16)
#define DEFAULT_HASH_ATTEMPTS (1)
#define DEFAULT_LINES (1)
#define DEFAULT_READS_PER_LINE (ent_per_line * DEFAULT_LINES)
#define DEFAULT_LOG_READS_PER_LINE (3)



#define counter_bits 4
#define counter_bits_mask 0xf
#define slot_bits 12
#define slot_bits_mask 0xfff

#define RESIZE_THRESHOLD 2
#define max_tables 64 //max tables to create


#endif
