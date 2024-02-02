# Active Object
An Active Object basically consists of six elements:
    + A proxy, which provides an interface towards clients with publicly accessible methods.
    + An **interface** which defines the method request on an active object.
    + A list (**event queue**) of pending requests from clients.
    + A **scheduler**, which decides which request to execute next.
    + The **implementation** of the active object method.
    + A **callback** or **variable** for the client to receive the result.

![AO-1.png](../../assets/AO-1.png?raw=true)

## 1. First let's define what we should offer to end users who are supposed to use Active Object, aka AO interfaces.
Define activeObjectApi.h with some functions:
    + create(): Creates a new Active Object for current calling thread.
    + executeFunction(): Executes asynchronously a function which is given by master thread in the context of AO thread.