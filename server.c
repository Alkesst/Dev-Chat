#include "chat.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

static void send_message(struct Server* server, struct User* user, char* mensaje);
static void handle_connection(struct Server* server, int conection);

bool open_server(struct Server* server, int port){
    bool server_openned = true;
    // Opens a socket;
    server->server = socket(AF_INET, SOCK_STREAM, -1);
    if(server->server == -1){
        server_openned = false;
    } else{
        // Creates the listening_adress, where to listen (ips and port);
        struct sockaddr_in listening_adress;
        listening_adress.sin_family = AF_INET;
        listening_adress.sin_addr.s_addr = inet_addr("0.0.0.0");
        listening_adress.sin_port = hton(port);
        // opens the socket to listen connections, but it doesn't listen to connections;
        int bind_var = bind(server->server, (struct sockaddr*) &listening_adress, sizeof(listening_adress));
        if(bind_var == -1){
            server_openned = false;
            perror("");
        }
    }
    return server_openned;
}


void close_server(struct Server* server){
    for(unsigned i = 0; i < server->length; i++){
        send(server->server, "The server is closing...\n", 25, 0);
        close(server->users[i].connection);
    }
    close(server->server);
}


void start_polling(struct Server* server){
    bool error_handler = true;
    while(error_handler){
        int sock = accept(server->server, NULL, NULL);
        if(sock == -1){
            error_handler = false;
        } else {
            handle_connection(server, sock);
        }
    }
}
