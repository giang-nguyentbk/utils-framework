# utilsFramework


## [Active Object](https://github.com/giang-nguyentbk/utils-framework/tree/master/sw/activeObject)
Dependencies:
1. UtilsFramework::EventLoop::V1::IEventLoop
2. UtilsFramework::ThreadLocal::V1::IThreadLocal

Let's think of an AO as a worker in which our main thread (client) wanted to do some functions asynchronously
(without blocking main thread) and would then pass those functions to its AO which has an event queue of functions
to be performed based on priorities.

![AO.png](./assets/AO.png?raw=true)


// To implement Active Object we need Event Loop APIs, so pending for now after AO APIs have been done. Will continue
after Event Loop APIs are ready to use.

## [Event Loop](https://github.com/giang-nguyentbk/utils-framework/tree/master/sw/eventLoop)
Dependencies:
1. UtilsFramework::ThreadLocal::V1::IThreadLocal

An Event Loop is basically an infinite loop continuously waiting for and dispatching events or messages
within a program. Our Event Loop based on a well-known event driven architecture of Linux operating system,
which is epoll.

![eventLoop2](./assets/eventLoopp.png?raw=true)


## [Thread Local Linux](https://github.com/giang-nguyentbk/utils-framework/tree/master/sw/threadLocal)
As you know, threads in a same process should share one virtual address space. Therefore, if you have, for example,
a global variable that is stored at a specific address. It can be easily modified by multiple threads in unpredictable
ways.

For some convinient purposes, we will want to have own data for each thread. And you can also manage those blocks
of data via TLS keys (Thread Local Storage keys). One unique key is associated with one object allocated in the heap.