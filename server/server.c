/*
 * Dev-Chat A chat made by developers, for developers.
 * Copyright (C) 2017  Alejandro Garau Madrigal & Melchor Garau Madrigal
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "chat_server.h"
#include "server_list.h"

#include <sys/ioctl.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <errno.h>
#include <netinet/tcp.h>


static void send_message(struct Server* server, struct User* user, char* mensaje);
static void handle_connection(struct Server* server, int connection);
static int associate_with_id(struct User* user);
static bool is_prime(int number);
static void* transmission(void* polymorph);
static int next_prime(int number);
static void message_received(struct Server* server, struct User* user, size_t bytes_available);
static void user_closed(struct Server* server, struct User* user);
static void set_certificate(struct Server* server);


bool open_server(struct Server* server, int port){
    sigset_t signal_set;
    /*Blocks the signal SIGALRM we will use to stop the select() from thread*/
    sigaddset(&signal_set, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &signal_set, NULL);
    SSL_load_error_strings();
    SSL_library_init();
    server->ssl_context = SSL_CTX_new(SSLv23_method());
    set_certificate(server);
    bool server_openned = true;
    int opt = 1;
    // Opens a socket;
    server->server = socket(AF_INET, SOCK_STREAM, 0);
    /*Allows to return the server without wait to the error "Adress already in use..."*/
    setsockopt(server->server, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt));
    if(server->server == -1){
        /*if the socket returns -1 means that there was an error */
        server_openned = false;
    } else{
        // Creates the listening_adress, where to listen (ips and port);
        struct sockaddr_in listening_adress;
        listening_adress.sin_family = AF_INET;
        listening_adress.sin_addr.s_addr = inet_addr("0.0.0.0");
        listening_adress.sin_port = htons(port);
        // opens the socket and binds it to the port, and listens the network;
        int bind_var = bind(server->server, (struct sockaddr*) &listening_adress, sizeof(listening_adress));
        if(bind_var == -1){
            server_openned = false;
            close(server->server);
            perror("");
        } else{
            /*Listens the */
            listen(server->server, 256);
            new_list(server);
            pthread_mutexattr_t local_sync;
            pthread_mutexattr_init(&local_sync);
            pthread_mutexattr_settype(&local_sync, PTHREAD_MUTEX_RECURSIVE);
            pthread_mutex_init(&server->sync, &local_sync);
            pthread_create(&server->thread, NULL, transmission, server);
            pthread_mutexattr_destroy(&local_sync);
        }
    }
    return server_openned;
}

void close_server(struct Server* server){
    for(unsigned i = 0; i < server->length; i++){
        /*Shutdowns the server and sends a message to all connected users*/
        SSL_write(server->users[i].ssl, "The server is closing...\n", 25);
        /*Shutdowns the connection from the server to the user*/
        SSL_shutdown(server->users[i].ssl);
    }
    SSL_CTX_free(server->ssl_context);
    close(server->server);
    /*frees the memory of the server*/
    free_list(server);
    pthread_mutex_destroy(&server->sync);
}

void start_polling(struct Server* server){
    /*Accepts connections to the server*/
    bool error_handler = true;
    while(error_handler){
        int sock = accept(server->server, NULL, NULL);
        if(sock == -1){
            error_handler = false;
            perror("An error has occurred in polling tasks");
        } else {
            handle_connection(server, sock);
        }
    }
}

static void handle_connection(struct Server* server, int connection){
    /*If it's the first time that an user logs into the server, it adds to a list*/
    int read_chars = 0;
    char user_response[25];
    int var = 1;
    struct User user;
    setsockopt(connection, IPPROTO_TCP, TCP_NODELAY, &var, sizeof(var));

    user.ssl = SSL_new(server->ssl_context);
    SSL_set_fd(user.ssl, connection);
    if (SSL_accept(user.ssl) <= 0) {
        SSL_free(user.ssl);
        close(connection);
        printf("Somebody tried to connect but happened an unknown error on SSL Layer...\n");
        return;
    }

    char* username = NULL;
    do{
        if(username != NULL){
            free(username);
            username = NULL;
        }
        do{
            SSL_write(user.ssl, "Send me your username\n", 23);
            read_chars = SSL_read(user.ssl, user_response, 25);
            if(read_chars > 25){
                SSL_write(user.ssl, "Your username must have less tan 25 chars\n", 43);
            }
        }while(read_chars > 25);
        username = malloc(read_chars);
        memcpy(username, user_response, read_chars);
        username[read_chars - 1] = '\0';
        pthread_mutex_lock(&server->sync);
        if(search_user_from_usrnm(server, username) != NULL){
            SSL_write(user.ssl, "This username already exists\n", 29);
        }
        pthread_mutex_unlock(&server->sync);
    }while(search_user_from_usrnm(server, username) != NULL);

    user.username = username;
    /*Generates an ID from a HASH of the string*/
    user.id = associate_with_id(&user);
    user.connection = connection;
    user.length = strlen(username);
    pthread_mutex_lock(&server->sync);
    insert_new_user(server, &user);
    pthread_mutex_unlock(&server->sync);
    SSL_write(user.ssl, "Your username has been added successfully\n", 42);
    char* message = malloc(user.length + 26);
    strcpy(message, "The user ");
    strcat(message, user.username);
    strcat(message, " has connected.\n");
    send_message(server, NULL, message);
    free(message);
    /*Sends a signal to the thread, SIGALRM -> stops the select() from transmission*/
    pthread_kill(server->thread, SIGALRM);
}

static void send_message(struct Server* server, struct User* user, char* mensaje){
    /*Sends a message to every user in the chat, except yourself*/
    size_t message_len = strlen(mensaje);
    pthread_mutex_lock(&server->sync);
    for(unsigned i = 0; i < server->length; i++){
        if(user == NULL || server->users[i].id != user->id){
            SSL_write(server->users[i].ssl, mensaje, message_len);
        }
    }
    pthread_mutex_unlock(&server->sync);
}

static int associate_with_id(struct User* user){
    int res = 0, primo = 2;
    for(unsigned i = 0; i < strlen(user->username); i++){
        primo = next_prime(primo);
        res += user->username[i]*primo;
    }
    return res;
}

static int next_prime(int number){
    while(is_prime(number)){
        number++;
    }
    return number;
}

static bool is_prime(int number){
    int divisors = 2;
    while((number % divisors) != 0 && divisors < (number / 2)){
        divisors++;
    }
    return (divisors >= (number / 2));
}

static void signal_ignore(int generic_var_name){}

static void* transmission(void* polymorph){
    /*Receives a polymorph pointer*/
    struct Server* server = polymorph;
    /**/
    sigset_t signal_set;
    sigaddset(&signal_set, SIGALRM);
    pthread_sigmask(SIG_UNBLOCK, &signal_set, NULL);
    signal(SIGALRM, signal_ignore);
    while(true){
        fd_set lecture;
        fd_set errors;
        /*Lists with sockets to notify with events*/
        FD_ZERO(&lecture);
        FD_ZERO(&errors);
        int max_sock = 0;
        pthread_mutex_lock(&server->sync);
        for(size_t i = 0; i < server->length; i++){
            if(server->users[i].connection > max_sock){
                max_sock = server->users[i].connection;
            }
            FD_SET(server->users[i].connection, &lecture);
            FD_SET(server->users[i].connection, &errors);
        }
        pthread_mutex_unlock(&server->sync);
        int ret_value = select(max_sock + 1, &lecture, NULL, &errors, NULL);
        if(ret_value == -1 && errno == EINTR){
            /*A new user was added*/
        } else if(ret_value == -1){
            /*An error has occurred*/
            perror("Couldn't read message...\n");
            /*Stops everything*/
            pthread_exit(NULL);
        } else{
            /*Something arrived or somebody ended the conection*/
            int i = 0;
            pthread_mutex_lock(&server->sync);
            while(i < server->length){
                if(FD_ISSET(server->users[i].connection, &lecture)){
                    int bytes_available;
                    ioctl(server->users[i].connection, FIONREAD, &bytes_available);
                    if(bytes_available == 0){
                        user_closed(server, &server->users[i]);
                        if(i > 0) i--;
                    } else{
                        message_received(server, &server->users[i],(size_t) bytes_available);
                    }
                }
                if(FD_ISSET(server->users[i].connection, &errors)){
                    user_closed(server, &server->users[i]);
                    i--;
                }
                i++;
            }
            pthread_mutex_unlock(&server->sync);
        }
    }
}

static void message_received(struct Server* server, struct User* user, size_t bytes_available){
    char* message;
    size_t ice_t = user->length;
    message = malloc(bytes_available + 5 + ice_t);
    bytes_available = SSL_read(user->ssl, message + ice_t + 4, bytes_available);
    memcpy(message + 1, user->username, ice_t);
    message[0] = '[';
    message[ice_t+1] = ']';
    message[ice_t + 2] = ':';
    message[ice_t + 3] = ' ';
    message[bytes_available + ice_t + 4] = '\0';
    message[bytes_available + ice_t + 3] = '\n';
    send_message(server, user, message);
}

static void user_closed(struct Server* server, struct User* user){
    close(user->connection);
    char *message = malloc(28 + user->length);
    strcpy(message, "The user ");
    strcat(message, user->username);
    strcat(message, " was disconnected\n");
    SSL_free(user->ssl);
    delete_user(server, user);
    send_message(server, NULL, message);
}


static void set_certificate(struct Server* server){
    SSL_CTX_set_ecdh_auto(server->ssl_context, 1);
    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(server->ssl_context, server->public_path, SSL_FILETYPE_PEM) <= 0) {
        printf("No se ha podido cargar el certificado publico\n");
        exit(EXIT_FAILURE);
    }
    if (SSL_CTX_use_PrivateKey_file(server->ssl_context, server->private_path, SSL_FILETYPE_PEM) <= 0 ) {
        printf("No se ha podido cargar el certificado privado\n");
        exit(EXIT_FAILURE);
    }
}
