# Photo-player-Server

this application will be at the backend and will save information from client that want to fectch and push information
to the database of the Photogram API.


the requirement for this server are the following list.

- bind specific port http & https
- create both thread in order to handle ssl and single http connection
- every connection will be launch as child thread
- in case of the server should stop there must a join or broadcast msg to all connected client.

