# Event Loop
Event Loop is basically a infinite loop. Our Event Loop implementation bases on epoll and File Descriptor.
1. Responsible for managing a list of File Descriptors.
2. Continuously wait for some epoll_event using epoll_wait() system call.
3. Handle those epoll_event by calling callback functions on correct FDs.

![eventLoop1](../../assets/eventLoop.png?raw=true)

![eventLoop2](../../assets/eventLoopp.png?raw=true)