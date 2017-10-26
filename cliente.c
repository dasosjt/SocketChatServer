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
#define BUFFER_SIZE 1024

//Paramters
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
void alerta(const char *mensaje)
{
    perror(mensaje);
    exit(0);
}
//Write to socket
void sendRequest(int sockfd, char *buffer)
{
    if (write(sockfd, buffer, BUFFER_SIZE) < 0)
    {
         alerta("No se pudo escribir en el socket \n");
    }
    bzero(buffer, BUFFER_SIZE);
}
//Read from socket
void respuesta(int sockfd, char *buffer)
{
    if (read(sockfd, buffer, BUFFER_SIZE) < 0) 
    {
        alerta("No se pudo leer del socket\n" );
    }
}

//Socket reader
void *sreader(void *arg)    
{
    int *reader = (int *)arg;
    char buffer_reader[BUFFER_SIZE];
    protocol *p = malloc(sizeof(protocol));

    while(1)
    {
        respuesta(*reader, buffer_reader);
        fprintf(stdout, "READED: %s\n", buffer_reader);
        p = interpret(buffer_reader);
        memset(buffer_reader, 0, BUFFER_SIZE);

        if(p->accion == "01")
        {
            fprintf(stdout, "01\n");
        }
        else if(p->accion == "05")
        {
            fprintf(stdout, "05\n");
        }
        else if(p->accion == "07")
        {
            fprintf(stdout, "07\n");
        }
    }

    free(p);

    return NULL;
}

int main( int argc, char *argv[])
{
    //Arguments
    cliente = argv[1]; //Client name
    Usuario = argv[2]; //User name
    UserPort = argv[3]; //User port
    ServerIP = argv[4]; //Server IP
    ServerPort = argv[5]; //Server Port
    Estado = "Activo"; //By default

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
        alerta("No se pudo abrir el socket :/ \n");
    //Converts IPv4 adress to binary form
    inet_pton(AF_INET, ServerIP, &ipv4addr);

    //Gives hostname (Change to gethostbyname when conected)
    server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if (server == NULL)
    {
        fprintf(stderr,"No existe el host :/\n");
        exit(0);
    }


    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        alerta("Ocurrio un error al conectarse \n");
    
    bzero(buffer,BUFFER_SIZE);
    //Regist Protocol
    snprintf(buffer, BUFFER_SIZE, "00|%s|127.0.0.1|%s|%s", Usuario, UserPort, Estado); 

    sendRequest(sockfd, buffer);
    bzero(buffer,BUFFER_SIZE);
    
    n = read(sockfd,buffer,BUFFER_SIZE);
    if (n < 0)
    {
        alerta("No se pudo leer del socket\n" );
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
        alerta("No se creo el thread para escuchar\n");
    }

    while(opcion != 0)
    {
        printf("Opciones: (Ingrese un numero valido)\n");
        printf("1.Chat\n");
        printf("2.Estado\n");
        printf("3.Usuarios y estados\n");
        printf("4.Cerrar \n");
        printf("Ayuda: seleccione el numero de opcion que desea ejecutar. \n");
        scanf("%d", &opcion);
        switch(opcion)
        {
            case 1: //  Chat
                printf("Usuario a quien le enviara el mensaje:");
                char Udestino[50];
                scanf("[^\n]", Udestino);
                printf("Ingrese mensaje:");
                char msgC[255];
                scanf("[^\n]", msgC);
                    
                snprintf(buffer, BUFFER_SIZE, "08|%s|%s|%s", Usuario, Udestino, msgC);
                sendRequest(sockfd, buffer);

                break;

            case 2: //  change status
                printf("Ingrese el estado: \n");
                printf("0 -> Activo\n");
                printf("1 -> IDLE\n");
                printf("2 -> Away\n");
                printf("Seleccione una opcion valida:\n");
                char UStatus[BUFFER_SIZE];
                scanf("%s", UStatus);
                
                snprintf(buffer, sizeof(buffer), "03|%s|%s", Usuario, UStatus);
                sendRequest(sockfd, buffer);
                
                break;

            case 3: //  User get status
                
                snprintf(buffer, sizeof(buffer), "06|%s", Usuario);
                sendRequest(sockfd, buffer);
                
                break;

            case 4: // Close connection
                
                snprintf(buffer, sizeof(buffer), "02|%s", Usuario);
                sendRequest(sockfd, buffer);
                close(sockfd);
                
                exit(0);    
        }
    }
}
