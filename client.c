//Universida del Valle de Guatemala
//Cliente.c
//Diego Sosa
//Juan Carlos Tapia
//Leonel Guillén
//Referencias para conexion de socket -> https://courses.cs.washington.edu/courses/cse476/02wi/labs/lab1/client.c
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <pthread.h>
#include "parser.h"
#define BUFFER_SIZE 256

char buffer[BUFFER_SIZE];
int sockfd = 0;
char s[INET_ADDRSTRLEN];
char *Usuario;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//Functions
//Print Errors
void alert(const char *mensaje)
{
  perror(mensaje);
  exit(0);
}
//Write to socket
void sendRequest(int sockfd, char *buffer)
{
  if (write(sockfd, buffer, BUFFER_SIZE) < 0)
  {
    alert("No se pudo escribir en el socket \n");
  }
  memset(buffer, 0, BUFFER_SIZE);
}
//Read from socket
void response(int sockfd, char *buffer)
{
    if (read(sockfd, buffer, BUFFER_SIZE) < 0) 
    {
        alert("No se pudo leer del socket\n" );
    }
}

//Socket reader
void *sreader(void *arg)    
{
  int *reader = (int *)arg;
  char buffer_reader[BUFFER_SIZE];
  protocol *p = malloc(sizeof(protocol));
  memset(p, 0, sizeof(protocol));

  while(1)
  {
    response(*reader, buffer_reader);
    fprintf(stdout, "\n\n\tMessage Received: %s\n", buffer_reader);
    p = interpret(buffer_reader);
    memset(buffer_reader, 0, BUFFER_SIZE);

    if(strcmp(p->accion, "01") == 0)
    {
      fprintf(stdout, "\n\n\tThe Server already has this username logged in. Try again later.\n");
      exit(0);
    }
    else if(strcmp(p->accion, "05") == 0)
    {
      //05|usuario|direccionIP|puerto|status
      fprintf(stdout, "\n\n\tIP: %s PORT: %s STATUS: %s \n", p->ip, p->puerto, p->status);
    }
    else if(strcmp(p->accion, "07") == 0)
    {
      fprintf(stdout, "07\n");
      /*for(int i = 0; i < 2; i++){
        fprintf(stdout, "USER: %s STATUS: %s \n", ((user)p->listaUsuarios[i]).nombre, ((user)p->listaUsuarios[i]).status);
      }*/
    }
    else
    {
      //fprintf(stdout, "Not admited protocol \n"); 
    }
    memset(buffer_reader, 0, BUFFER_SIZE);
    memset(p, 0, sizeof(protocol));
  }

  free(p);

  return NULL;
}

char* scanInput()
{
  char *message = malloc(BUFFER_SIZE);
  memset(message, 0, BUFFER_SIZE);
  if (message == NULL){
    printf("No memory for input.\n");
    return NULL;
  }
  
  fgets(message, BUFFER_SIZE, stdin);

  if((strlen(message) > 0) && (message[strlen (message) - 1] == '\n'))
  {
    message[strlen (message) - 1] = '\0';
  }

  return message;
}

int main( int argc, char *argv[])
{
  struct addrinfo serverinfo, *result, *p;
  int opcion, status;

  if(argc != 4)
  {
    printf("\n usage: %s <ip> <port> <username>\n", argv[0]);
    return 1;
  }

  Usuario = argv[3];

  memset(&serverinfo, 0, sizeof serverinfo);
  serverinfo.ai_family = AF_INET;
  serverinfo.ai_socktype = SOCK_STREAM;   

  if(status = getaddrinfo(argv[1], argv[2], &serverinfo, &result) != 0)
  { 
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
    return 1;
  }

  for (p = result; p != NULL; p = p->ai_next){
    
    if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
    {
      perror("couldnt create socket \n");
      continue;
    } 

    if( connect(sockfd, p->ai_addr, p->ai_addrlen) == -1)
    {
      close(sockfd);
      printf("connect failed \n");
      continue;
    } 
    break;
  }

  if (p == NULL)
  {
    fprintf(stderr, "client failed to connect\n");
    return 2;
  }

  snprintf(buffer, BUFFER_SIZE, "00|%s|192.168.0.1|1100|0", Usuario);

  sendRequest(sockfd, buffer);

  freeaddrinfo(result);  
  struct sockaddr_in *ipv4 = (struct sockaddr_in *)p;

  inet_ntop(p->ai_family, &(ipv4->sin_addr), s, sizeof s);
  printf("client: connecting to %s\n", s);

  printf("----------------------------------------------------------\n");
  printf("---------------------Proyecto 1---------------------------\n");
  printf("------------------------Chat------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("Diego Sosa------------------------------------------------\n");
  printf("Juan Carlos Tapia-----------------------------------------\n");
  printf("Leonel Guillen--------------------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("---------------------Bienvenido---------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("\n");

  memset(buffer, 0, BUFFER_SIZE);

  pthread_t socket_reader;

  if(pthread_create(&socket_reader, NULL, sreader, (void *)&sockfd) < 0)
  {
      alert("No se creo el thread para escuchar\n");
  }

  while(opcion != 0)
  {
    printf("Opciones: (Ingrese un numero valido)\n");
    printf("1.Chat\n");
    printf("2.Cambiar estado\n");
    printf("3.Estado usuario en especifico\n");
    printf("4.Usuarios y estados\n");
    printf("5.Cerrar \n");
    printf("Ayuda: seleccione el numero de opcion que desea ejecutar. \n");
    opcion = atoi(scanInput());
    switch(opcion)
    {
      case 1: //  Chat
        fprintf(stdout, "Usuario a quien le enviara el mensaje:");
        char *Udestino;
        Udestino = scanInput();
        
        fprintf(stdout, "Ingrese mensaje:");
        char *msg;
        msg = scanInput();

        snprintf(buffer, BUFFER_SIZE, "08|%s|%s|%s", Usuario, Udestino, msg);
        sendRequest(sockfd, buffer);

        break;

      case 2: //  change status
        
        printf("Ingrese el nuevo estado: \n");
        printf("0 -> Activo\n");
        printf("1 -> IDLE\n");
        printf("2 -> Away\n");
        printf("Seleccione una opcion valida:\n");
        char *status;
        status = scanInput();
        
        snprintf(buffer, sizeof(buffer), "03|%s|%s", Usuario, status);
        sendRequest(sockfd, buffer);
        
        break;

      case 3: // get specific user status
        fprintf(stdout, "Usuario de quien desea obtener informacion:");
        char *Ustatus;
        Ustatus = scanInput();

        snprintf(buffer, sizeof(buffer), "04|%s|%s", Ustatus, Usuario);
        sendRequest(sockfd, buffer);

        break;

      case 4: //  get Users status
          
        snprintf(buffer, sizeof(buffer), "06|%s", Usuario);
        sendRequest(sockfd, buffer);
        
        break;

      case 5: // Close connection
          
        snprintf(buffer, sizeof(buffer), "02|%s", Usuario);
        sendRequest(sockfd, buffer);
        close(sockfd);
        
        exit(0);    
    }
  }
}
