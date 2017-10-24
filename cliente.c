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

//Inicio de argumentos

char *cliente; //Es el nombre de cliente
char *Usuario; //Nombre de Usuario
char *UserPort; //Puerto del Usuario
char *ServerIP; //IP del servidor
char *puertoServidor; //Puerto del servidor
char *Estado; //Estado del usuario, por defecto es "Activo"

//Main del cliente 
int main( int argc, char *argv[]) {
    //Recibe lor agumentos que se les pasa
    cliente = argv[1];
    Usuario = argv[2];
    UserPort = argv[3];
    ServerIP = argv[4];
    ServerPort = argv[5];
    Estado = "Activo"; //Por defecto es activo



    //Conexion
    int sockfd, portno, n;
    struct in_addr ipv4addr;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[256];
    
    //Para el menu

    int opcion = 1;
    // Convierte String en int para el puerto
    portno = atoi(ServerPort);
    //retorna el valor retornado por el socket system call
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        alerta("No se pudo abrir el socket :/");
    //Convierte IPv4(6) adress a forma binaria
    inet_pton(AF_INET, ServerIP, &ipv4addr);

    //Da el hostname
    server = gethostbyaddr(&ipv4addr, sizeof ipv4addr, AF_INET);
    if (server == NULL) {
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
        alerta("Ocurrio un error al conectarse");
    
    
    //Probando validez de usuario y puerto para el servidor
    //00|ususario|direccionIP|puerto|status
    bzero(buffer,256);

    snprintf(buffer, sizeof(buffer), "00|%s|127.0.0.1|%s|%s", Usuario,UserPort,Estado);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) {
         alerta("No se pudo escribir en el socket");
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if (n < 0) {
        alerta("No se pudo leer del socket");
    }
    //Aqui divide la cadena en tokens separado por el caracter "|" ldv
    //strcmp ->  comparacion de strings, 
    //compara la primera parte de la cadena con "01" para ver se el usuario ya existe
    char *token = strtok(buffer, "|");
    if(strcmp(token,"01")==0){
        printf("Usuario o IP ya en uso \n");  
        exit(0);
    }
    printf("%s\n",buffer);
    

    printf("--------------------------\n");
    printf("--------Proyecto 1--------\n");
    printf("-----------Chat-----------\n");
    printf("--------------------------\n");
    printf("--------------------------\n");
    printf("Integrantes:--------------\n");
    printf("Diego Sosa----------------\n");
    printf("Juan Carlos Tapia---------\n");
    printf("Leonel Guillen------------\n");
    printf("--------------------------\n");
    printf("--------Bienvenido--------\n");
    printf("--------------------------\n");
    printf("\n");
    printf("\n");
    printf("\n");
    while(opcion != 0){
        printf("Opciones: (Ingrese un numero valido)\n");
        printf("1.Chat\n");
        printf("2.Estado\n");
        printf("3.Usuarios y estados\n");
        printf("4.Cerrar \n");
        printf("Ayuda: seleccione el numero de opcion que desea ejecutar. \n");
        scanf("%d", &opcion);
        switch(opcion){


            case 1:; //Chatear (enviar mensaje)
                printf("Usuario a quien le enviara el mensaje:");
                char Udestino[1024] = "";
                scanf("%s", Udestino);
                printf("Ingrese mensaje:");
                char msgC[1024] = "";
                scanf("%s", msgC);
                bzero(buffer,256);
                //08|emisor|receptor|mensaje
                snprintf(buffer, sizeof(buffer), "08|%s|%s|%s", Usuario, Udestino, msgC);
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) {
                     alerta("No se pudo escribir en el socket :/");
                }
                printf("Enviado :D");
                break;
                
            case 2:; //Cambiar estado
                printf("Nombre de Usuario:\n");
                char user [1024] = "";
                scanf("%s", user);
                printf("Ingrese el estado: \n");
                printf("0 -> Activo\n");
                printf("1 -> IDLE\n");
                printf("2 -> Away\n");
                printf("Seleccione una opcion valida:\n");
           
                char UStatus[1024] = "";
                scanf("%s", UStatus);
                bzero(buffer,256);
                snprintf(buffer, sizeof(buffer), "03|%s|%s", user, UStatus);
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) {
                     alerta("No se pudo escribir al socket");
                }
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
                if (n < 0) {
                    alerta("No se pudo leer del socket");
                }
                printf("%s\n",buffer);
                break;
                
            case 3:; //Usuarios y estado
                bzero(buffer,256);
                snprintf(buffer, sizeof(buffer), "06|%s", Usuario);
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) {
                     alerta("No se pudo escribir en el socket");
                }
                bzero(buffer,256);
                n = read(sockfd,buffer,255);
                if (n < 0) {
                    alerta("No se pudo leer del socket");
                }
                
                printf("Listado de usuarios:\n");
                printf("Usuario y Estado\n");
                strtok(buffer, "|");
                char *lista = strtok(NULL,"&");
                while(lista != NULL){
                    printf("%s", lista);
                    lista = strtok(NULL,"&");
                }
                break;
            case 4:
                snprintf(buffer, sizeof(buffer), "02|%s", Usuario);
                n = write(sockfd,buffer,strlen(buffer));
                if (n < 0) {
                     alerta("No se pudo escribir en el socket");
                }
                exit(0);
            
        }
        
        
    }
}

//Funcion que imprimira los errores
void alerta(const char *mensaje)
{
    perror(mensaje);
    exit(0);
}
