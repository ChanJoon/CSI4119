# Homework 2

## Summary

You should improve a performance of locked open addressing hash table.

Go to `better_locked_hash_table.h` and implement `TODO`

```
$ make remote
$ cat result/better.txt
TABLE_SIZE 10000000 init: 4000000 new: 4000000 NT: 16 additional_reads: 9 use_custom: 1
user-defined HT1 1
start filling
init hash table took 0.118376 sec
start test
test 36000000 ops took @@@@@@@ sec
sanity check PASSED: 
```

## Functions

```
# Run locally
make
make run
# Compile
make HTtest
# Clean
make clean
# Evaluate performance
make remote
# Remove
make remove
```

## References

- [std::thread - cplusplus.com](http://www.cplusplus.com/reference/thread/thread/)
