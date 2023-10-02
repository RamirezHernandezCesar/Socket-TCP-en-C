/*----------Servidor TCP------------

Autor: Cesar Ramirez Hernandez

Forma de compilar: gcc servidor.c -o servidor

Forma de correr: ./servidor

NOTA: El servidor correra en la ip de la maquina donde se este ejecutando y el puerto donde escuchara sera el 4950.
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



#define MYPORT 4950 /* Puerto por donde se enviaran los elementos */
#define MAXBUFLEN 50000 /* Cantidad maxima de bytes que va a recibir */


typedef struct segment SEGMENT;

struct segment{
        int segNum;
        int control;
        int lineLen;
        char line[MAXBUFLEN];  
};


//Funcion principal
int main(int argc, char *argv[]){
 
 
int sockfd;
int port;       /*puerto a utilizar por el servidor*/ 
 
//Se necesitan dos estructuras del tipo sockaddr
//La primera guarda info del server
//La segunda del cliente
 struct sockaddr_in server;
 struct sockaddr_in client;   
 
//Primer paso, definir variables
//int fd,fd2,longitud_cliente,puerto;
//puerto=atoi(argv[1]);

int addr_len,numbytes,conexionAceptada;
char buf[MAXBUFLEN]; /*Bufer de recepcion*/
char msg[MAXBUFLEN];


/*Para manejo de comandos*/
char comando[25];
char param1[25];

if(argc < 2)
{
        port=MYPORT;
}
else
{
        /* Asignacion del puerto especifico */
        port=atoi(argv[1]);
}

//Se crea el socket
if ( (sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
{
        perror("Error de apertura de socket");
        exit(1);
}

 
//Se establece la estructura del servidor

server.sin_family= AF_INET;           /* Familia del socket */ 
server.sin_port = htons(port);        /* Puerto convertido a network byte order (BigEndian) */ 
server.sin_addr.s_addr = INADDR_ANY;  /* Se asigna automaticamente la direccion IP local (servidor) */ 
bzero(&(server.sin_zero),8);          /* Se rellena con ceros el resto de la estructura */
 
 
/* Se le da un nombre (IP y Puerto) al socket*/ 
if(bind(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1) 
{
         printf("error en bind() \n");
         exit(1);
}
 
 // establecer el socket en modo escucha
if(listen(sockfd,1) == -1) 
{
         printf("error en listen()\n");
         exit(-1);
}

printf("Servidor escucha por el puerto: %d\n",ntohs(server.sin_port));



addr_len = sizeof(struct sockaddr_in);

	 //aceptar conexiones
         if ((conexionAceptada = accept(sockfd,(struct sockaddr *)&client,&addr_len)) == -1) 
         {
                printf("error en accept()\n");
                exit(1);
         }
         
         printf("Conexion proveniente de: %s %d\n",inet_ntoa(client.sin_addr),ntohs(client.sin_port)); 
        

do{
         
        /* El servidor esta esperando la conexion de un cliente */      
        if ((numbytes = recv(conexionAceptada,buf,MAXBUFLEN,0))  == -1 ){
                perror("Error en el recv");
                exit(1);
        }
        
        buf[numbytes]='\0';
          
        //Que contiene buf??         
        sscanf(buf,"%s %s",comando,param1);  // Tiene forma de comando?               
        if(strcmp(comando,"cp") == 0){
        
                int i=0;
                char * data=0;
                int datalen=0;
                
                printf("\n\tComando Detectado:%s Parámetro:%s",comando, param1);	// Imprime comando y parámetros
               
                
                FILE *archivo;
                SEGMENT seg;
                
                
                
               if ((archivo = fopen(param1,"rb")) == NULL){  // Si no se pudo abrir el archivo --> Se avisa al Cte (control=2)
                 printf("\n\tError: no se pudo abrir el archivo: %s. \n",param1);
		 
		 seg.line[0]=0;		 
		 seg.segNum=0;
		 seg.control=htons(2);
		 seg.lineLen=0;
		 printf("\n\tEnviando Indicador de archivo no encontrado (control):[%d]\n",ntohs(seg.control));
		  
		 data=(char*)&seg;
		 datalen=sizeof(SEGMENT)-sizeof(seg.line)+strlen(seg.line);
		 
		 //send(conexionAceptada,"Bienvenido a mi servidor.\n",26,0);
		 
		 if (( numbytes = send(conexionAceptada, data, datalen, 0)) == -1 ){
		        perror("\n\tError al enviar indicador de archivo no encontrado: sendto\n"); 
		        exit(1);
		 }
		 
		 
	       }
	       else     // Se pudo abrir el archivo
	       {	
 		printf("\n\tArchivo %s abierto y listo para enviar...\n",param1);
		 
		 
		while ( !feof(archivo) )
		{		
		
		  seg.lineLen = fread(seg.line,1,MAXBUFLEN,archivo);  /* Se guarda el numero de bytes que leyo la funcion */
		  seg.segNum=i;
		  seg.control=0;
		  //seg.lineLen=strlen(seg.line);
	          printf("\n\t[%d] Enviando segmento al cliente :[%d], Control [%d]\t\n",seg.lineLen,seg.segNum,seg.control);
		  
		  
		  /* Se convierten los datos a network byte order para ser transmitidos por la red */
		  seg.segNum=htons(seg.segNum);
		  seg.control=htons(seg.control);
		  seg.lineLen=htonl(seg.lineLen);		  
		  data=(char*)&seg;
		  
                  /* Se calcula el tamaño del paquete a enviar */		  
		  datalen=sizeof(SEGMENT);
		  
		  printf("[%d] datalen=%d\n",i,datalen);		  		  
		  		
		  /* Se envia el paquete al cliente destino */  
		  if (( numbytes = send(conexionAceptada, data, datalen, 0)) == -1 ){
		        perror("\n\tError al enviar archivo: sendto\n"); 
		        exit(1);
		  }
		  
		  /* El servidor espera 1 segundo para continuar con el proceso de envio de paquetes */
		  sleep(1);
		  i++;		  
		}
		
		seg.line[0]=0;
		seg.control=htons(1); /* La variable se asigna en 1 para indicar al cliente que termino de enviarse el archivo */
		seg.segNum=0;
		seg.lineLen=0;
		printf("\n\tEnviando Indicador de fin del archivo (control):[%d]\n",ntohs(seg.control));
		  
		data=(char*)&seg;
		datalen=sizeof(SEGMENT)-sizeof(seg.line)+strlen(seg.line);
		
		/* Se envia el ultimo paquete para indicar que se termino de enviar el archivo */
		if (( numbytes = send(conexionAceptada, data, datalen, 0)) == -1 ){
		        perror("\n\tError al enviar EOF: sendto\n"); 
		        exit(1);
		}
		
		//se cierra el archivo
		fclose(archivo);
	    } 
                                               
        
        }
                             
        
                
 }while(strcmp(buf,"salir"));

        //cerramos el socket       
        close(sockfd);
        exit(1);
        
        
 
 
return 0; 
}




