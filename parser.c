#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parser.h"

protocol* interpret(char cadena[])
{
    protocol* p;
    p = malloc(sizeof(protocol));
    memset(p, 0, sizeof(protocol));
    char *accion;
    char *usuario;   
    char *ip;
    char *puerto;
    char *status;
    char *usuario2; 
    char *source, *message, *destination;
    char *saveptr1, *saveptr2,  *saveptr3;
    
    char divisores[3] = "|¬";
    accion = strtok_r(cadena,divisores , &saveptr1);
    
    switch(accion[1]) {

        case '0'  :
            
            usuario = strtok_r(NULL,divisores, &saveptr1);
            ip = strtok_r(NULL,divisores, &saveptr1);
            puerto = strtok_r(NULL,divisores, &saveptr1);
            status = strtok_r(NULL,divisores, &saveptr1);
            
            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            strcpy( p->ip, ip); 
            strcpy( p->puerto, puerto); 
            strcpy( p->status, status); 
            
            break; 
        
        case '1'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);
            ip = strtok_r(NULL,divisores, &saveptr1);
            
            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            strcpy( p->ip, ip); 
            break; 
            
        case '2'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);
            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            break; 
            
        case '3'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);
            status = strtok_r(NULL,divisores, &saveptr1);

            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            strcpy( p->status, status); 
            break; 
            
        case '4'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);
            usuario2 = strtok_r(NULL,divisores, &saveptr1);

            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            strcpy( p->usuario2, usuario2);
            break; 
            
        case '5'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);
            ip = strtok_r(NULL,divisores, &saveptr1);
            puerto = strtok_r(NULL,divisores, &saveptr1);
            status = strtok_r(NULL,divisores, &saveptr1);
            
            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            strcpy( p->ip, ip); 
            strcpy( p->puerto, puerto); 
            strcpy( p->status, status); 
            
            break; 
            
        case '6'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);

            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            break; 
        
        case '8'  :
              source = strtok_r(NULL,divisores, &saveptr1);
              destination = strtok_r(NULL,divisores, &saveptr1);
              message = strtok_r(NULL,divisores, &saveptr1);

              strcpy( p -> accion, accion); 
              strcpy( p -> source, source);
              strcpy( p -> destination, destination);
              strcpy( p -> message, message);
          break; 
        
        case '7'  :
            usuario = strtok_r(NULL,divisores, &saveptr1);

            strcpy( p->accion, accion); 
            strcpy( p->usuario, usuario); 
            

            char *cadenaListado;
            char cadenaListadoFija[100];
            cadenaListado = strtok_r(NULL,divisores , &saveptr1);
            strcpy( cadenaListadoFija, cadenaListado); 
            
            char *cadenaUsuario;
            char *nombreUsuario;
            char *estadoUsuario;

            cadenaUsuario = strtok_r (cadenaListado, "&¬" , &saveptr2);
            
            int contador = 0;
            while (cadenaUsuario != NULL) {
                
                
                user u;
                nombreUsuario = strtok_r (cadenaUsuario, "+" , &saveptr3);
                estadoUsuario = strtok_r (NULL, "+" , &saveptr3);
                
                strcpy( u.nombre, nombreUsuario); 
                strcpy( u.status, estadoUsuario); 
                
                p->listaUsuarios[contador] = u;
                contador++;

                cadena = strtok_r (NULL, "&¬", &saveptr2);
               
                int i;
                
                strcpy( cadenaListado, cadenaListadoFija); 
                
                cadenaUsuario = strtok_r (cadenaListado, "&¬" , &saveptr2);

                for( i = 0; i < contador; i = i + 1 ){
                    cadenaUsuario = strtok_r (NULL, "&¬" , &saveptr2);
                }
            }
            
            break; 
    }
    return p;
}

