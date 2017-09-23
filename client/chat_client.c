#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
// Client Main

struct Client{
    int socket;
    char* username;
    pthread_t thread;
};

static void* receive_and_print_message(void* polymorph);
static void read_and_send_message(struct Client* client);
static int set_ip_and_port(struct in_addr* ip_adress, int port, struct Client* client);


int main(int argc, const char* argv[]){
    struct Client client;
    struct in_addr ip_address;
    int port = 3128;
    if(argc > 1){
        if(inet_aton(argv[1], &ip_address) == 0){
            fprintf(stderr, "The argument %s is not a valid IPv4 address\n",argv[1]);
            exit(1);
        }
        if(argc > 2){
            int local_var = atoi(argv[2]);
            if(local_var >= 0 && local_var <= 65535){
                port = local_var;
            } else{
                fprintf(stderr, "Argument is requiered: Expected port.\n");
                exit(2);
            }
        }
    } else{
        fprintf(stderr, "Argument is requiered: Expected IP address\n");
        exit(3);
    }
    int socket = set_ip_and_port(&ip_address, port, &client);
    if(socket != -1){
        read_and_send_message(&client);
    } else{
        perror("");
        exit(4);
    }

    return 0;
}

static int set_ip_and_port(struct in_addr* ip_address, int port, struct Client* client){
    /*Creates the socket and stablish the connection with the server given. Returns -1 if there's any issue*/
    client->socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client->socket != -1){
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr = *ip_address;
        server_address.sin_port = htons(port);
        if(connect(client->socket, (struct sockaddr*) &server_address, sizeof(struct sockaddr_in)) == -1){
            close(client->socket);
            client->socket = -1;
        } else{
            pthread_create(&client->thread, NULL, receive_and_print_message, &client->socket);
        }
    }
    return client->socket;
}

static void* receive_and_print_message(void* polymorph){
    struct Client* client = polymorph;
    char* message = malloc(1000);
    while(true){
        int bytes_readed = recv(client->socket, message, 1000, 0);
        if(client->socket == -1){
            free(message);
            pthread_exit(NULL);
        } else if(bytes_readed > 0){
            printf("%.*s", bytes_readed, message);
            if(strstr(message, "This username already exists") != NULL){
                free(client->username);
                client->username = NULL;
            }
        } else{
            printf("Server has closed...\nPress intro to close this process...\n");
            free(message);
            fclose(stdin);
            pthread_exit(NULL);
        }
    }
}

static void read_and_send_message(struct Client* client){
    size_t length;
    client->username = NULL;
    char* message = NULL;
    while(!feof(stdin) && !ferror(stdin)){
        if(getline(&message, &length, stdin) != -1){
            length = strlen(message);
            send(client->socket, message, length, 0);
            if(client->username == NULL){
                client->username = strdup(message);
                client->username[length - 1] = '\0';
            }
            printf("(%s): ", client->username);
            free(message);
            message = NULL;
        }
    }
    free(client->username);
    close(client->socket);
}
