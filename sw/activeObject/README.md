## Active Object
###### + A proxy, which provides an interface towards clients with publicly accessible methods.
###### + An **interface** which defines the method request on an active object.
###### + A list (**event queue**) of pending requests from clients.
###### + A **scheduler**, which decides which request to execute next.
###### + The **implementation** of the active object method.
###### + A **callback** or **variable** for the client to receive the result.

### First let's define what we should offer for end users who are supposed to use Active Object, aka AO interfaces.
###### Define activeObjectIf.h with some functions:
######      + 