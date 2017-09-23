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


static void* receive_and_print_message(void* polymorph);
static void read_and_send_message(int socket);
static int set_ip_and_port(struct in_addr* ip_adress, int port, pthread_t* client_thread);


int main(int argc, const char* argv[]){
    pthread_t client_thread;
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
    int socket = set_ip_and_port(&ip_address, port, &client_thread);
    if(socket != -1){
        read_and_send_message(socket);
    } else{
        perror("");
        exit(4);
    }

    return 0;
}

static int set_ip_and_port(struct in_addr* ip_address, int port, pthread_t* client_thread){
    /*Creates the socket and stablish the connection with the server given. Returns -1 if there's any issue*/
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(client_socket != -1){
        struct sockaddr_in server_address;
        server_address.sin_family = AF_INET;
        server_address.sin_addr = *ip_address;
        server_address.sin_port = htons(port);
        if(connect(client_socket, (struct sockaddr*) &server_address, sizeof(struct sockaddr_in)) == -1){
            close(client_socket);
            client_socket = -1;
        } else{
            pthread_create(client_thread, NULL, receive_and_print_message, (void*) (long) client_socket);
        }
    }
    return client_socket;
}

static void* receive_and_print_message(void* polymorph){
    int client_socket = (int) (long) polymorph;
    char* message = malloc(1000);
    while(true){
        int bytes_readed = recv(client_socket, message, 1000, 0);
        if(client_socket == -1){
            free(message);
            pthread_exit(NULL);
        } else if(bytes_readed > 0){
            printf("%.*s", bytes_readed, message);
        } else{
            printf("Server has closed...\nPress intro to close this process...\n");
            free(message);
            fclose(stdin);
            pthread_exit(NULL);
        }
    }
}

static void read_and_send_message(int socket){
    size_t length;
    char* message = NULL;
    while(!feof(stdin) && !ferror(stdin)){
        if(getline(&message, &length, stdin) != -1){
            length = strlen(message);
            send(socket, message, length, 0);
            free(message);
            message = NULL;
        }
    }
    close(socket);
}
