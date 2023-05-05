#ifndef _BETTER_LOCKED_HASH_TABLE_H_
#define _BETTER_LOCKED_HASH_TABLE_H_

#include <iostream>
#include <mutex>
#include <thread>

#include "bucket.h"
#include "hash_table.h"

class better_locked_probing_hash_table : public hash_table {
 private:
  Bucket* table;
  const int TABLE_SIZE;  // we do not consider resizing. Thus the table has to
                         // be larger than the max num items.
  std::mutex global_mutex;

  /* TODO: put your own code here  (if you need something)*/
  /****************/
  const int num_mutex = 10000;
  std::mutex global_mutex_list[10000];
  const int RANGE = TABLE_SIZE/num_mutex;

  bool run(uint32_t key, uint64_t value, uint64_t index){
    if (table[index].valid == true){
      if (table[index].key == key) {
        return true;
      } else {
        return false;
      }
    } else {
      table[index].valid = true;
      table[index].key = key;
      table[index].value = value;
      return true;
    }
  }

  bool critical(uint32_t key, uint64_t value, uint64_t index, int part, int iter, std::mutex* mutex){
    std::lock_guard<std::mutex> lock(global_mutex_list[part]);
    if (iter==0){
      for (uint64_t i=index; i<RANGE*(part+1); i++){
        if (run(key, value, i)){
          return true;
        }
      }
      return false;
    } else if (iter==num_mutex){
      for (uint64_t i=RANGE*part; i<index; i++){
        if (run(key, value, i)){
          return true;
        }
      }
      return false;
    } else {
      for (uint64_t i=RANGE*part; i<RANGE*(part+1); i++){
        if (run(key, value, i)){
          return true;
        }
      }
      return false;
    }
  }

  /****************/
  /* TODO: put your own code here */

 public:
  better_locked_probing_hash_table(int table_size) : TABLE_SIZE(table_size) {
    this->table = new Bucket[TABLE_SIZE]();
    for (int i = 0; i < TABLE_SIZE; i++) {
      this->table[i].valid = 0;  // means empty
    }
  }

  virtual uint32_t hash(uint32_t x) {
    // https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return (x % TABLE_SIZE);
  }

  virtual uint32_t hash_next(uint32_t key, uint32_t prev_index) {
    // linear probing. no special secondary hashfunction
    return ((prev_index + 1) % TABLE_SIZE);
  }

  // the buffer has to be allocated by the caller
  bool read(uint32_t key, uint64_t* value_buffer) {
    /* TODO: put your own read function here */
    /****************/

    uint64_t index = this->hash(key);
    int probe_count = 0;
    while (table[index].valid == true) {
      if (table[index].key == key) {
        *value_buffer = table[index].value;
        return true;
      } else {
        probe_count++;
        index = (index+1) % TABLE_SIZE;
        if (probe_count >= TABLE_SIZE) break;
      }
    }
    return false;

    /****************/
    /* TODO: put your own read function here */
  }

  bool insert(uint32_t key, uint64_t value) {
    /* TODO: put your own insert function here */
    /****************/

    uint64_t index = this->hash(key);
    int part = index/RANGE;
    std::mutex* mutex;
    for (int i = 0; i<num_mutex+1; i++){
      if (critical(key, value, index, part, i, mutex)){
        return true;
      }
      part = (part+1)%num_mutex;
    }
    return false;

    /****************/
    /* TODO: put your own insert function here */
  }

  int num_items() {
    int count = 0;
    for (int i = 0; i < TABLE_SIZE; i++) {
      if (table[i].valid == true) count++;
    }
    return count;
  }
};

#endif
