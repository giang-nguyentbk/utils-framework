# commonApis


## [Active Object](https://github.com/giang-nguyentbk/commonApis/tree/activeObject/sw/activeObject)
Let's think of an AO as a worker in which our main thread (client) wanted to do some functions asynchronously
(without blocking main thread) and would then pass those functions to its AO which has an event queue of functions
to be performed based on priorities.

![AO.png](./assets/AO.png?raw=true)


// To implement Active Object we need Event Loop APIs, so pending for now after AO APIs have been done. Will continue
after Event Loop APIs are ready to use.

## Event Loop