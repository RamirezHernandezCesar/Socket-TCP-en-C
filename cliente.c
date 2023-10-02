/*
----------Cliente TCP------------

Autor:Cesar Ramirez Hernandez

Forma de compilar: gcc cliente.c -o cliente

Forma de correr: ./cliente Ip_Servidor Puerto_Servidor

./cliente 127.0.0.1 4950

NOTA: Al momento de ejecutar el cliente es necesario especificar la ip y el puerto donde se encuentra el servidor.

*/


// Ficheros de cabecera
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
//netbd.h es necesitada por la estructura hostent


#define MYPORT 4950 /* el puerto donde se enviaran los datos */ 
#define MAXBUFLEN 50000 /* Max. cantidad de bytes que podra recibir en una llamada a recv*/ 

typedef struct segment SEGMENT;

/* Estructura que funcionara como el paquete a enviar */
struct segment{
        int segNum;
        int control;
        int lineLen;
        char line[MAXBUFLEN];
};
 
int main(int argc, char *argv[]){
 
  //Definir variables 
  int sockfd; 
  int port=MYPORT;
  char msg[MAXBUFLEN];
  char * ip;
  int numbytes;


 /* estructura que recibira informacion sobre el nodo remoto */
 /* informacion sobre la direccion del servidor */
  struct sockaddr_in server;   /* Almacenara la direccion IP y numero de puerto del servidor */ 
  int addr_len; 
  char buf[MAXBUFLEN]; 
  
  
  /* Tratamiento de la linea de comandos. */
  if (argc >= 2){
        ip = argv[1];
    
          if (argc == 3){
                port = atoi(argv[2]);
          }
  
  }else{
        fprintf(stderr,"Uso: cteTCP <ip> [puerto]\n");
        exit(1);
  }
 
 
 /* Creamos el socket */ 
 if (( sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
        printf("socket() error\n");
        exit(-1);
 }
 
/* Se establece la estructura server con la direccion ip y puerto del servidor a la cual el cliente se conectara*/ 
 server.sin_family = AF_INET;           /* Familia del socket */ 
 server.sin_port = htons(MYPORT);       /* Puerto convertido a network byte order - (BigEndian) */ 
 server.sin_addr.s_addr=inet_addr(ip);  /* convierte la IP de notación de números y puntos a la de datos binarios */
 bzero(&(server.sin_zero),8);           /* Se rellena con ceros el resto de la estructura */
 
 addr_len = sizeof(struct sockaddr);
 
 
 //conectarnos al servidor
 if(connect( sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
 {
        printf("connect() error\n");
        exit(-1);
 } 
 
 do{
  	/* Solicitamos mensaje */ 
  	printf("\nMensaje a enviar: ");
  	gets(msg);   
  
  	/* enviamos el mensaje, esta linea contiene una barra invertida al final, indicando que sigue abajo*/ 
  	if (( numbytes = send(sockfd, msg, strlen(msg), 0)) == -1 ){
    	        perror("Error al enviar mensaje con: sendto"); 
    	        exit(1);
  	}

  	printf("\tenviados %d bytes hacia %s\n",numbytes,inet_ntoa(server.sin_addr));
	
	
	// Para manejo de comandos
	char comando[25];
	char param1[25];
	char * cm=comando;
	char * par=param1;
  
	//Que contiene buf??
	sscanf(msg,"%s %s",comando, param1);	// Tiene forma de comando?
        
        if(strcmp(comando,"cp")==0)
	{
	    
	    printf("\tComando:%s Parámetro:%s",comando, param1);	// Imprime comando y parámetros
	    
	    
	    //Hay que crear el archivo de nombre param1 a recibir línea por línea	    
	    FILE *archivo;
	    SEGMENT seg;	    	    	    
	    
	    //concatena el nombre del archivo con .copia	    
	    strcat(param1,".copia");
	    
            if ((archivo = fopen(param1,"wb")) == NULL){		// No se pudo crear el archivo.
                printf("\n\tError: no se pudo crear el archivo: %s. \n",param1);
		 
	    }	
	    else{	// Archivo creado OK
 		printf("\n\tArchivo %s creado y listo para recibir...\n",param1);
		int i=0;
		char * data=0;
		int datalen=0;
		
		do
		{
		
		   /* Sirve para limpiar el buf */
		  memset (buf, '\0', MAXBUFLEN);
		   
		  sleep(1); 
		  
		  
		  //if ((numbytes = recv(sockfd, buf, sizeof(SEGMENT), 0)) == -1){
		  /* Se reciben los datos enviados del servidor */
		  if ((numbytes = read(sockfd, buf, sizeof(SEGMENT))) == -1){
		        perror("\n\tError: No se pudo realizar la recepción de datos con: recvfrom\n"); 
		        exit(1);
		  }
		  		   
 
                 printf("\nlongitud del paquete en bytes : %d",numbytes);
                                  
                  /* Se convierten los datos a host recibidos por la red */                 
                 seg.segNum = ntohs(((SEGMENT *)buf)->segNum);
		  seg.control = ntohs(((SEGMENT *)buf)->control);
		  seg.lineLen = ntohl(((SEGMENT *)buf)->lineLen);
 
                  printf("\n\t[%d] Segmento recibido del servidor :[%d], Control:[%d]\t\n",seg.lineLen,seg.segNum,seg.control);
 
 
                  if(seg.control==0){	
		    //fputs(seg.line, archivo);
		    fwrite( ((SEGMENT *)buf)->line,1,seg.lineLen,archivo);		    		  
		        		    
		  }
		  else if(seg.control==2){
		        printf("\n\tArchivo no encontrado en el servidor...\n");
		        break;
		  }
		  else if(seg.control==1){
		        printf("\n\t[%d] Fin de archivo recibido en el segmento:[%d]\n",seg.lineLen,seg.segNum);
		    break;
		  }
		  
		  
                 }while (seg.control==0);
                 
                  fclose(archivo);
                }
                
                }
                  
                  
                 }while(strcmp("salir",msg));
  
                 close(sockfd); 
  
                 exit(1);

 
 
}






