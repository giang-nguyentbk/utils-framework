# Active Object
An Active Object basically consists of six elements: <br />
&emsp + A proxy, which provides an interface towards clients with publicly accessible methods. <br />
&emsp + An **interface** which defines the method request on an active object. <br />
&emsp + A list (**event queue**) of pending requests from clients. <br />
&emsp + A **scheduler**, which decides which request to execute next. <br />
&emsp + The **implementation** of the active object method. <br />
&emsp + A **callback** or **variable** for the client to receive the result. <br />

![AO-1.png](../../assets/AO-1.png?raw=true)

## 1. First let's define what we should offer to end users who are supposed to use Active Object, aka AO interfaces.
Define activeObjectApi.h with some functions: <br />
&emsp + create(): Creates a new Active Object for current calling thread. <br />
&emsp + executeFunction(): Executes asynchronously a function which is given by master thread in
    the context of AO thread. <br />