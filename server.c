#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <unistd.h> // sleep

#include <pthread.h>
#include "thpool.h"
#include "queue.h"
#include "parser.h"
#include "hashmap.h"


#define PORT "3490" // the port in use
#define BACKLOG 10  // how many connections the queue will hold
#define BUFFER 1024 //  the maximum length of the buffer
#define TRUE 1
#define FALSE 0
#define N_THREADS 10
#define KEY_MAX_LENGTH (256)

typedef struct client
{
  char *ip;
  char *port;
  char *status;
  int fd;
} client;

typedef struct h_map_element
{
  char key_string[KEY_MAX_LENGTH];
  void* value;
} h_map_element;

fd_set master;  //  master file descriptor list
fd_set readfds; //  temp file descriptor list for select()
int fdmax;      //  maximum file descriptor number
int listener;   //  listener for new connections
char remoteIP[INET6_ADDRSTRLEN];  //  remote ip
struct sockaddr_storage remoteaddr; //  remote address storage


threadpool thpool;  // global pool of threads
queue protocol_queue; // global queue of protocols
map_t client_map; //  global map of clients

/*
 * This will get the address IPvX
 * */
void *get_in_addr(struct sockaddr *sa)
{
  if (sa->sa_family == AF_INET)
  {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * This will handle a new connection
 * */
void *new_connection_handler(void * args)
{
  socklen_t addr_size;  //  Get address size
  int new_fd; //  new file descriptor for new connections
  char *message;  //  message to send

  inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, sizeof(remoteIP));
  
  if(strlen(remoteIP) > 0)
  {
    fprintf(stdout, "server: got connection from %s\n", remoteIP);
  }

  addr_size = sizeof(remoteaddr);
  new_fd = accept(listener, (struct sockaddr *)&remoteaddr, &addr_size);

  if(new_fd == -1)
  {
    fprintf(stderr, "could not create socket(): %s\n", gai_strerror(new_fd));
  } 
  else
  {
    FD_SET(new_fd, &master);  //  add to master set
    if(new_fd > fdmax)  //  keep track of the max 
    {
      fdmax = new_fd;
    }
    //Send some messages to the client
    message = "Hello from OS SERVER..\n";
    write(new_fd , message , strlen(message));
  }   

  return NULL;
}

/*
 * This will handle a protocol switch aplication depending on the activity
 * */
void *switch_protocol_handler(void* args)
{
  protocol* p = (protocol*)args; //  Get protocol

  //fprintf(stdout, "PROTOCOL: %s\n", p->accion);

  if(strcmp(p->accion, "00") == 0)
  {
    fprintf(stdout, "00 Hello World! \n");
    client * c = malloc(sizeof(client));
    memset(c, 0, sizeof(client));
    h_map_element* element = malloc(sizeof(h_map_element));
    memset(element, 0, sizeof(h_map_element));
    
    snprintf(element->key_string, KEY_MAX_LENGTH, "%s", p->usuario);

    c->ip = p->ip;
    c->port = p->puerto;
    c->status = p->status;
    c->fd = p->fd;
    element->value = (void*)c;

    if(hashmap_put(client_map, element->key_string, element) == 0)
    {
      //Send transacted protocol messages to the client
      char * message = "Transacted protocol 00\n";
      write(((client*)element->value)->fd, message , strlen(message));
    }

  }
  else if(strcmp(p->accion, "02") == 0)
  {
    fprintf(stdout, "02 Hello World! \n"); 

    char *user = malloc(sizeof(char*));
    memset(user, 0, sizeof(char));

    snprintf(user, KEY_MAX_LENGTH, "%s", p->usuario);
  
    if(hashmap_remove(client_map, user) == 0)
    {
      fprintf(stdout, "connection closed\n");
      //Send transacted protocol messages to the client
      char * message = "Transacted protocol 02\n";
      write(p->fd, message, strlen(message));
      close(p->fd);  // Bye :(
      FD_CLR(p->fd, &readfds);
      FD_CLR(p->fd, &master);
      free(user);
    }
  }
  else if(strcmp(p->accion, "03") == 0)
  {
   fprintf(stdout, "03 Hello World! \n"); 
  }
  else if(strcmp(p->accion, "04") == 0)
  {
   fprintf(stdout, "04 Hello World! \n"); 
  }
  else if(strcmp(p->accion, "06") == 0)
  {
   fprintf(stdout, "06 Hello World! \n"); 
  }
  else if(strcmp(p->accion, "07") == 0)
  {
   fprintf(stdout, "07 Hello World! \n"); 
  }
  else if(strcmp(p->accion, "08") == 0)
  {
   fprintf(stdout, "08 Send Message! \n"); 
  }
  else
  {
    fprintf(stdout, "Not admited protocol \n");
  }

  free(p);
  return NULL;
}

/*
 * This will handle a protocol aplication
 * */
void *new_protocol_handler(void* args)
{
  protocol* p = (protocol*)args;  // Get protocol

  fprintf(stdout, "PROTOCOL: %s\n", p->accion);
  fprintf(stdout, "USER: %s\n", p->usuario);

  protocol *arg = malloc(sizeof(protocol));
  memset(arg, 0, sizeof(protocol));

  *arg = *p;

  h_map_element* h_element;

  if(strcmp(arg->accion,"08") == 0) // IF IS in readfds and we have to send the message
  {
    if(hashmap_get(client_map, arg->usuario2, (void**)(&h_element)) == 0) //User Found
    { 
      client* c;
      c = (client *)h_element;
      fprintf(stdout, "Send message from %d\n", c->fd);
      if(FD_ISSET(c->fd, &readfds))
      {
        enqueue(&protocol_queue, (void *)p);
        fprintf(stdout, "queue size: %d\n", queue_num_size(&protocol_queue));
      }
      else
      {
        //  handle switch protocol handler
        if( thpool_add_work(thpool, (void*)switch_protocol_handler, (void *)arg) < 0){
          fprintf(stderr, "could not create thread(): \n");
          return (void *) 2;
        }
      }
    } 
  } else {
    //  handle switch protocol handler
    if( thpool_add_work(thpool, (void*)switch_protocol_handler, (void *)arg) < 0){
      fprintf(stderr, "could not create thread(): \n");
      return (void *) 2;
    }
  }

  free(p);

  return NULL;
}

/*
 * This will handle a new message from client
 * */
 void *new_message_handler(void *socket_desc) 
 {
  int sock = *((int*)socket_desc); //Get the socket descriptor;
  int status_connection;
  char *rmsg_test;
  rmsg_test = malloc(BUFFER); //  allocate memory for the buffer

  inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr *)&remoteaddr), remoteIP, sizeof(remoteIP));
  
  if(strlen(remoteIP) > 0)
  {
    fprintf(stdout, "server: got message from %s\n", remoteIP);
  }
  
  switch(status_connection = recv(sock, rmsg_test, BUFFER, 0))
  {
    case 0:
      fprintf(stdout, "connection closed\n");
      close(sock);  // Bye :(
      FD_CLR(sock, &readfds);
      FD_CLR(sock, &master);
      free(rmsg_test);
      free(socket_desc);
      return (void *)0;
  
    default:
      if(status_connection > 0)
      {
        fprintf(stdout, "message received: %s \n", rmsg_test);
        
        protocol *arg = malloc(sizeof(protocol));
        memset(arg, 0, sizeof(protocol));

        if(arg == NULL)
        {
          fprintf(stderr, "could not allocate memory for thread arg.\n");
          return (void *)4;
        }

        protocol * temp = interpret(rmsg_test);

        //fprintf(stdout, "PROTOCOL: %s\n", temp->accion);

        *arg = *temp;
        arg->fd = sock;

        //  handle new protocol
        if( thpool_add_work(thpool, (void*)new_protocol_handler, (void *)arg) < 0)
        {
          fprintf(stderr, "could not create thread(): \n");
          return (void *) 2;
        } 

        FD_CLR(sock, &readfds);
        free(rmsg_test);
        free(socket_desc);

        return (void *)1;  
      }
      else
      {
        fprintf(stderr, "recv(): %s\n", gai_strerror(status_connection));
        FD_CLR(sock, &readfds);
        free(rmsg_test);
        free(socket_desc);
        return (void *)-1;
      }
  }
}

int main(void)
{ 
  struct addrinfo hints, *res;

  int status, i, True = TRUE;

  thpool = thpool_init(N_THREADS); // pool of threads

  protocol_queue = queue_init();  // queue of protocols

  client_map = hashmap_new();  // map of clients

  //  clear the master and temp sets
  FD_ZERO(&master);
  FD_ZERO(&readfds);

  //  load up address structs with getaddrinfo()
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_UNSPEC; // user IPv4 or IPv6, whichever
  hints.ai_socktype = SOCK_STREAM; // TCP
  hints.ai_flags = AI_PASSIVE;  //fill in my IP for me

  if(((status = getaddrinfo(NULL, PORT, &hints, &res)) != 0))
  {
        fprintf(stderr, "getaddrinfo(): %s\n", gai_strerror(status));
        return 2;
  }

  //  make a socket, bind it, and listen on it

  //  make the socket
  listener = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  
  // lose address already in use error message
  setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &True, sizeof(int));
  
  // bind the socket
  if(bind(listener, res->ai_addr, res->ai_addrlen) < 0)
  {
    fprintf(stderr, "bind():\n");
    close(listener);
    return 3;
  }

  if(res == NULL)
  {
    fprintf(stderr, "failed to bind():\n");
    return 4;
  }

  freeaddrinfo(res);  //  no use for now on

  if(listen(listener, BACKLOG) == -1)
  {
    fprintf(stderr, "listen():\n");
    return 5;
  }

  // add listener to master set
  FD_SET(listener, &master);

  //  track biggest file description
  fdmax = listener;

  while(True)
  {
    readfds = master;
    if(select(fdmax+1, &readfds, NULL, NULL, NULL) == -1)
    {
      fprintf(stderr, "select():\n");
      return 6;
    }

    for(i = 0; i <= fdmax; i++)
    {
      if(FD_ISSET(i, &readfds))
      {  // connection response
        if(i == listener)
        {
          //  handle new connections
          if( thpool_add_work(thpool, (void*)new_connection_handler, NULL) < 0)
          {
            fprintf(stderr, "could not create thread(): \n");
            return 7;
          } 
        } 
        else
        {
          //  handle new message
          int *arg = malloc(sizeof(int));

          if(arg == NULL)
          {
            fprintf(stderr, "could not allocate memory for thread arg.\n");
            return 8;
          }

          *arg = i;

          if(thpool_add_work(thpool, (void*)new_message_handler , (void*) arg) < 0)
          {
            fprintf(stderr, "could not create thread(): \n");
            return 9;
          }
        }
      }
    }

    while(queue_num_size(&protocol_queue) > 0)
    {
      qnode* n = peek(&protocol_queue);
      dequeue(&protocol_queue);

      fprintf(stdout, "message received: %s \n", (char*)n->data);

      //  handle new protocol
      if( thpool_add_work(thpool, (void*)new_protocol_handler, (void *)n->data) < 0)
      {
        fprintf(stderr, "could not create thread(): \n");
        return 10;
      } 
    }

    thpool_wait(thpool);
  }
  
  close(listener);
  thpool_destroy(thpool);
  queue_destroy(protocol_queue);

  return 0;
}