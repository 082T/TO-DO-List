# TO-DO-List

Joint to-do list with login written in C for Unix/Linux environment. Homework for "computer networks" class.
```
Options:

1: see list
2: add element
3: change element*
4: delete element*
*if the element was created by that person
```


Compile and run server:

>gcc servertxt.c protokoltxt.c -lpthread -o server

>./server [port number]

Compile and run client:

>gcc klijenttxt.c protokoltxt.c -lpthread -o klijent

>./klijent [name] [password] [server_ip] [server_port]
