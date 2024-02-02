# Active Object
An Active Object basically consists of six elements: <br />
1. A proxy, which provides an interface towards clients with publicly accessible methods. <br />
2. An **interface** which defines the method request on an active object. <br />
3. A list (**event queue**) of pending requests from clients. <br />
4. A **scheduler**, which decides which request to execute next. <br />
5. The **implementation** of the active object method. <br />
6. A **callback** or **variable** for the client to receive the result. <br />

![AO-1.png](../../assets/AO-1.png?raw=true)

## 1. First let's define what we should offer to end users who are supposed to use Active Object, aka AO interfaces.
Define activeObjectApi.h with some functions: <br />
1. create(): Creates a new Active Object for current calling thread. <br />
2. executeFunction(): Executes asynchronously a function which is given by master thread in
    the context of AO thread. <br />