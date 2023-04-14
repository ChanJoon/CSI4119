#ifndef _BETTER_LOCKED_HASH_TABLE_H_
#define _BETTER_LOCKED_HASH_TABLE_H_


#include <iostream>
#include <mutex>
#include <thread>
//
#include <vector>
#include <atomic>
#include <shared_mutex>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>
//
#include "hash_table.h"
#include "bucket.h"

class better_locked_probing_hash_table : public hash_table {

  private:
    Bucket* table;
    const int TABLE_SIZE; //we do not consider resizing. Thus the table has to be larger than the max num items.
    std::mutex global_mutex; 

    static constexpr int NUM_STRIPES = 256;
    std::vector<std::mutex> stripe_mutexes;
    std::vector<boost::shared_mutex> shared_stripe_mutexes;

    int get_stripe_index(uint64_t table_index)
    {
      return table_index % NUM_STRIPES;
    }

    public:

    better_locked_probing_hash_table(int table_size):TABLE_SIZE(table_size), stripe_mutexes(NUM_STRIPES), shared_stripe_mutexes(NUM_STRIPES) {
      this->table = new Bucket[TABLE_SIZE]();
      for(int i=0;i<TABLE_SIZE;i++) {
        this->table[i].valid=0; //means empty
      }
    }

    virtual uint32_t hash(uint32_t x) override 
    {
      //https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      return (x % TABLE_SIZE); 
    }

    // uint32_t hash(uint32_t x, std::mutex* m)  
    // {
    //   m->lock();
    //   //https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    //   x = ((x >> 16) ^ x) * 0x45d9f3b;
    //   x = ((x >> 16) ^ x) * 0x45d9f3b;
    //   x = (x >> 16) ^ x;
    //   m->unlock();
    //   return (x % TABLE_SIZE); 
    // }

    // uint32_t hash(std::atomic<uint32_t> *x)  
    // {
    //   uint32_t val = x->load();
    //   //https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
    //   val = ((val >> 16) ^ val) * 0x45d9f3b;
    //   val = ((val >> 16) ^ val) * 0x45d9f3b;
    //   val = (val >> 16) ^ val;

    //   x->store(val);
    //   return (val % TABLE_SIZE); 
    // }

    uint32_t hash2(uint32_t x)
    {
      // Which one is more effective?
      //https://stackoverflow.com/a/41537995
      const int p = 23;
      const uint32_t knuth = 2654435769;
      const uint32_t y = x;

      x =  (y * knuth) >> (32 - p);
      return (x % TABLE_SIZE);
    }

    virtual uint32_t hash_next(uint32_t key, uint32_t prev_index) override
    {
      //linear probing. no special secondary hashfunction
      return ((prev_index + 1)% TABLE_SIZE); 
    }

    uint32_t hash_next(uint32_t key, uint32_t prev_index, int probe_count)
    {
      //quadratic probing. no special secondary hashfunction
      // std::cout << "We got probe_count: " << probe_count << std::endl;
      return ((prev_index + probe_count * probe_count + probe_count)% TABLE_SIZE);
    }

    //the buffer has to be allocated by the caller
    bool read(uint32_t key, uint64_t* value_buffer){
      uint64_t index = this->hash(key);
      // boost::shared_lock<boost::shared_mutex> lock(shared_stripe_mutexes[get_stripe_index(index)]);
      int probe_count=0;

      while(table[index].valid == true) {
        if(table[index].key == key) {
          *value_buffer = table[index].value;
          return true;
        } else {
          probe_count++;
          index = this->hash_next(key, index, probe_count);
          if(probe_count >= TABLE_SIZE) break;
        }
      }//end while

      //If you reached here, you either encountered an empty slot or the table is full. In any case, the item you're looking for is not here 
      return false;

    }


    bool insert(uint32_t key, uint64_t value) {
      // std::lock_guard<std::mutex> lock(global_mutex);
      uint64_t index = this->hash(key);

      int probe_count=0;

      while (true) {
        std::lock_guard<std::mutex> lock(stripe_mutexes[get_stripe_index(index)]);
        // boost::unique_lock<boost::shared_mutex> lock(shared_stripe_mutexes[get_stripe_index(index)]);
        if (table[index].valid == false) {
          // Found an empty bucket
          table[index].valid = true;
          table[index].key = key;
          table[index].value = value;
          return true;
        } else if (table[index].key == key) {
          // Found a bucket with the same key, modify its value
          table[index].value = value;
          return true;
        } else {
          // Bucket is occupied by a different key, continue probing
          probe_count++;
          index = this->hash_next(key, index, probe_count);
          if (probe_count >= TABLE_SIZE) {
            // Could not add because the table is full
            return false;
          }
        }
      }

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
