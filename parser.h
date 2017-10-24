#ifndef _PARSER_
#define _PARSER_

typedef struct user {
    char  nombre[50];
    char  status[50];  // 0,1,2
} user;

typedef struct protocol {
    int fd;
    char accion[3];  //00,01,02,03,04,05,06,07,08
    char usuario[50]; // En 04 es el del info, en 06 es el que pide.
    char ip[50];
    char status[50]; // 0,1,2
    char puerto[10];
    char usuario2[50];  // En 04 es el que pide
    struct user listaUsuarios[200];   // Se usa para el 07
} protocol;

protocol* interpret(char cadena[]);

#endif
