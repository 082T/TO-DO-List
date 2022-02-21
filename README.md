# TO-DO-List

Joint to-do list with login written in c for Unix/Linux
Options:
1: see list
2: add element
3: change element
4: delete element



Compile and run server:

>gcc servertxt.c protokoltxt.c -lpthread -o server

>./server [port number]

Compile and run client:

>gcc klijenttxt.c protokoltxt.c -lpthread -o klijent

>./klijent [name] [password] [server_ip] [server_port]
