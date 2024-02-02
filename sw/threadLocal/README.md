# Thread Local Storage
As you know, threads in a same process should share one virtual address space. Therefore, if you have, for example,
a global variable that is stored at a specific address. It can be easily modified by multiple threads in unpredictable
ways. For some convinient purposes, we will want to have own data for each thread. And you can also manage those blocks
of data via TLS keys (Thread Local Storage keys). One unique key is associated with one object allocated in the heap.

## To achieve this, we can use 2 approaches as below:
1. **__thread keyword**: only available for POD (Plain Old Data) data type and not support class. Support infinite keys?
Using floating point registers to address data -> higher performance.
2. **pthread get/set_specific API**: support all data types. Support only up to PTHREAD_KEYS_MAX.

## References:
1. [Linux Thread Local Storage](https://www.sobyte.net/post/2022-02/linux-thread-local-storage/)
2. [pthread_getspecific() man7](https://man7.org/linux/man-pages/man3/pthread_getspecific.3p.html)