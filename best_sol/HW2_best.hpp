#ifndef _BETTER_LOCKED_HASH_TABLE_H_
#define _BETTER_LOCKED_HASH_TABLE_H_


#include <iostream>
#include <mutex>
#include <thread>
#include "hash_table.h"
#include "bucket.h"

class better_locked_probing_hash_table : public hash_table {

  private:
    Bucket* table;
    const int TABLE_SIZE; //we do not consider resizing. Thus the table has to be larger than the max num items.
    std::mutex global_mutex; 

    /* TODO: put your own code here  (if you need something)*/
    /****************/

    // static const uint64_t LOCAL_MUTEX_SIZE = 3001;
    // std::mutex local_mutex[LOCAL_MUTEX_SIZE*2];
    static const uint64_t SUBSET = 30000u;
    // const uint64_t LOCAL_MUTEX_SIZE = (TABLE_SIZE-1)/SUBSET+1;
    const uint64_t LOCAL_MUTEX_SIZE = (TABLE_SIZE+SUBSET-1)/SUBSET;
    std::mutex local_mutex[SUBSET+1];
    
    virtual uint32_t better_hash_next_quad(uint32_t key, uint32_t prev_index, int probe_count)
    {
      return ((prev_index+11)%TABLE_SIZE); 
    }

    virtual uint32_t hash_2(uint32_t x)  
    {
      //https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      return (x % TABLE_SIZE-1); 
    }

    virtual uint32_t better_hash_next_double(uint32_t key, uint32_t prev_index_1, uint32_t prev_index_2, int probe_count)
    {
      //double hashing
      return ((prev_index_1 + prev_index_2)% TABLE_SIZE); 
    }

    /****************/
    /* TODO: put your own code here */

    public:

    better_locked_probing_hash_table(int table_size):TABLE_SIZE(table_size){
      this->table = new Bucket[TABLE_SIZE]();
      for(int i=0;i<TABLE_SIZE;i++) {
        this->table[i].valid=0; //means empty
      }
    }

    virtual uint32_t hash(uint32_t x)  
    {
      //https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      return (x % TABLE_SIZE); 
    }

    virtual uint32_t hash_next(uint32_t key, uint32_t prev_index)
    {
      //linear probing. no special secondary hashfunction
      return ((prev_index + 1)% TABLE_SIZE); 
    }

    //the buffer has to be allocated by the caller
    bool read(uint32_t key, uint64_t* value_buffer){
      /* TODO: put your own read function here */
      /****************/
      // std::lock_guard<std::mutex> lock(global_mutex);

      // uint64_t index = this->hash(key);
      // uint64_t mutex_index = index % LOCAL_MUTEX_SIZE;
      // std::lock_guard<std::mutex> lock(local_mutex[mutex_index]);
      // int probe_count=0;

      // uint64_t index_1 = this->hash(key);
      // uint64_t index_2 = this->hash_2(key);
      // uint64_t index = index_1;

      // std::lock_guard<std::mutex> lock(global_mutex);


      uint64_t index = this->hash(key);

      // uint64_t index = this->hash(key);
      int probe_count=0;
      
      while(table[index].valid == true){
        if(table[index].key == key){
          *value_buffer = table[index].value;
          return true;
        } else {
          probe_count++;
          // index = this->better_hash_next_quad(key, index, probe_count);
          // index = this->better_hash_next_double(key, index_1, index_2, probe_count);
          index = this->hash_next(key, index);
          if(probe_count >= TABLE_SIZE) break;
        }
      }
      // item not found
      return false;
      /****************/
      /* TODO: put your own read function here */
    }


    bool insert(uint32_t key, uint64_t value) {
      /* TODO: put your own insert function here */
      /****************/
      // std::lock_guard<std::mutex> lock(global_mutex);
      // std::cout << key << '\n';
      uint64_t index = this->hash(key);
      // uint64_t num = 3001u;
      uint64_t mutex_index = index / LOCAL_MUTEX_SIZE;
      // std::cout << mutex_index << '\n';
      std::lock_guard<std::mutex> lock(local_mutex[mutex_index]);
      int probe_count=0;

      // uint64_t index_1 = this->hash(key);
      // uint64_t index_2 = this->hash_2(key);

      // std::lock_guard<std::mutex> lock(global_mutex);
      // uint64_t index = this->hash(key);
      // int probe_count=0;

      // uint64_t index = index_1;
      while(table[index].valid == true) {
        if(table[index].key == key) {
          //found it already there. just modify
          break;
        } else {
          probe_count++;
          // index = this->better_hash_next_quad(key, index, probe_count);
          // index = this->hash_next(key, index);
          // index = this->better_hash_next_double(key, index_1, index_2, probe_count);
          index = this->hash_next(key, index);
          if(probe_count >= TABLE_SIZE) return false; //could not add because the table was full
        }
      }
      table[index].valid = true; 
      table[index].key   = key; 
      table[index].value = value;
      return true;
      /****************/
      /* TODO: put your own insert function here */
    }

    int num_items() {
      int count=0;
      for(int i=0;i<TABLE_SIZE;i++) {
        if(table[i].valid==true) count++;
      }
      return count;
    }
};

#endif
