# TO-DO-List
Server-client to-do list in c


Compile and run server:

gcc servertxt.c protokoltxt.c -lpthread -o server
./server [port number]

Compile and run client:

gcc klijenttxt.c protokoltxt.c -lpthread -o klijent
./klijent [name] [password] [server_ip] [server_port]
