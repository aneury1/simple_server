# AHTTPServer

the requirement for this server are the following list.

- bind specific port http & https
- create both thread in order to handle ssl and single http connection
- every connection will be launch as child thread
- in case of the server should stop there must a join or broadcast msg to all connected client.

#### action
- server already start on http connection
- server already accept middleware connection
- server serve JSON is request and is settle by the server.


#### todo
- this server need to be more clear on thread( thread need to be improve)
- need to be create a test suite
- need to be create static file service
- need to be refix the documentation and eliminate some code that does not belong to this space.
