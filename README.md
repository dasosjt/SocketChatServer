# SocketChatServer

# Compilar Server con

gcc server.c thpool.c queue.c parser.c hashmap.c -D THPOOL_DEBUG -lpthread -o server

# Compilar Cliente con

gcc cliente.c parser.c -o client -lpthread

# Registro de Usuario

[Registro {cliente a servidor}]

    00|ususario|direccionIP|puerto|status¬

[Error de registro {servidor a cliente}]

    01|usuario|direccionIP¬

# Liberacion de Usuario

[Cliente cierra chat {cliente a servidor}]

    02|usuario¬

# Cambio de status

[Cliente cambio estado {cliente a servidor}]

    03|usuario|status¬
        
        status
        0   Activo  
        1   Idle
        2   Away
        
# Obtencion de informacion de usuario

[Peticion de informacion {cliente a servidor}]

    04|usuarioDeInfo|usuarioQuePide¬

[Retorno de informacion {servidor a cliente}]

    05|usuario|direccionIP|puerto|status¬
    
# Listado de usuarios conectados

[Peticion de listado {cliente a servidor}]

    06|usuarioQuePide¬

[Retorno de listado {servidor a cliente}]

    07|usuarioQuePide|usuario1+estado1&usuario2+estado2...usuarioN+estadoN¬

#  Envio de Mensajes
[Peticion de envio de mensaje {cliente a servidor}]

    08|source|destination|message
