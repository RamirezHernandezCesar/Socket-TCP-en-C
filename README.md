# Socket-TCP-en-C

Creacion de Sockets TCP en lenguaje de programacion C, se creo un archivo servidor y un archivo cliente que permiten la tranferencia de archivos desde el servidor hacia el cliente.

----------Servidor TCP------------

Forma de compilar: gcc servidor.c -o servidor

Forma de correr: ./servidor

NOTA: El servidor correra en la ip de la maquina donde se este ejecutando y el puerto donde escuchara sera el 4950.
----------Cliente TCP------------

Forma de compilar: gcc cliente.c -o cliente

Forma de correr: ./cliente Ip_Servidor Puerto_Servidor

./cliente 127.0.0.1 4950

NOTA: Al momento de ejecutar el cliente es necesario especificar la ip y el puerto donde se encuentra el servidor.

Autor: Cesar Ramirez Hernandez

Fecha de actualizacion: 02-10-2023
