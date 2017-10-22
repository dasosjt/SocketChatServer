#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
   int sockfd, puerto, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   
   char buffer[256];
   
   
   puerto = atoi(argv[1]);
   
   //Creamos Socket
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("Error en socket");
      exit(1);
   }
   

   //Da el hostname en direccion de socket
   server = gethostbyname(argv[0]);    
   
   if (server == NULL) {
      fprintf(stderr,"No existe el host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(puerto);
   

   //Conecta al servidor
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("No se pudo conectar");
      exit(1);
   }
   
 
   //Mensaje y envia al servidor
   printf("Ingrese su mensaje: ");
   bzero(buffer,256);
   fgets(buffer,255,stdin);

   n = write(sockfd, buffer, strlen(buffer));
   
   if (n < 0) {
      perror("No se pudo escrivir en el socket");
      exit(1);
   }

   //Respuesta del servidor
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("No se pudo leer servidor");
      exit(1);
   }
   
   //mensaje
   printf("%s\n",buffer);

   close(sockfd);
   return 0;
}     