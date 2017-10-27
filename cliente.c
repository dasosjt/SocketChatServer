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

//Parameters
char *cliente; //Client Name
char *Usuario; //User Name
char *UserPort; //User port
char *ServerIP; //Server IP
char *ServerPort; //Server Port
char *Estado; //User state set "Active" by default 
char buffer[BUFFER_SIZE];

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
  //Arguments
  cliente = argv[1]; //Client name
  Usuario = argv[2]; //User name
  UserPort = argv[3]; //User port
  ServerIP = argv[4]; //Server IP
  ServerPort = argv[5]; //Server Port
  Estado = "0"; //By default

  int opcion = 1; //Menu

  //Connection
  int sockfd, portno, n;
  struct in_addr ipv4addr;
  struct sockaddr_in serv_addr;
  struct hostent *server;
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //Reference code used since here
  //Converts String to int for port
  portno = atoi(ServerPort);
  //Return value from socket system call
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
      alert("No se pudo abrir el socket :/ \n");
  //Converts IPv4 adress to binary form
  inet_pton(AF_INET, ServerIP, &ipv4addr);

  //Gives hostname (Change to gethostbyname when conected)
  server = gethostbyname(ServerIP);
  if (server == NULL)
  {
      fprintf(stderr,"No existe el host :/\n");
      exit(0);
  }


  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr_list, 
       (char *)&serv_addr.sin_addr.s_addr,
       server->h_length);
  serv_addr.sin_port = htons(portno);
  if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
      alert("Ocurrio un error al conectarse \n");
  
  bzero(buffer,BUFFER_SIZE);
  //Regist Protocol
  snprintf(buffer, BUFFER_SIZE, "00|%s|127.0.0.1|%s|%s", Usuario, UserPort, Estado); 

  sendRequest(sockfd, buffer);
  bzero(buffer,BUFFER_SIZE);
  
  n = read(sockfd,buffer,BUFFER_SIZE);
  if (n < 0)
  {
      alert("No se pudo leer del socket\n" );
  }
  //Ends Reference code
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  
  
  //Begins menu
  printf("----------------------------------------------------------\n");
  printf("---------------------Proyecto 1---------------------------\n");
  printf("------------------------Chat------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("Diego Sosa------------------------------------------------\n");
  printf("Juan Carlos Tapia-----------------------------------------\n");
  printf("Leonel Guillen--------------------------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("---------------------Bienvenido---------------------------\n");
  printf("----------------------------------------------------------\n");
  printf("\n");

  bzero(buffer,BUFFER_SIZE);

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
