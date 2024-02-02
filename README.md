# commonApis


## Active Object
Let's think of an AO as a worker in which our main thread (client) wanted to do some function asynchronously
(without blocking main thread) and would then pass the function to its AO which has an event queue of functions
to be performed based on priorities.

![AO.png](./assets/AO.png?raw=true)

![AO-1.png](./assets/AO-1.png?raw=true)

## Event Loop