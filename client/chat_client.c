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
#include <openssl/ssl.h>
// Client Main

struct Client{
    int socket;
    char* username;
    pthread_t thread;
    SSL_CTX* ctx;
    SSL* ssl;
    const char* certificate;
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
        } if(argc > 3){
            client.certificate = argv[3];
        } else{
            client.certificate = "public.pem";
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
            SSL_load_error_strings();
            SSL_library_init();
            client->ctx = SSL_CTX_new(SSLv23_method());
            SSL_CTX_use_certificate_file(client->ctx, client->certificate, SSL_FILETYPE_PEM);
            SSL_CTX_load_verify_locations(client->ctx, client->certificate, NULL);
            SSL_CTX_set_default_verify_paths(client->ctx);
            client->ssl = SSL_new(client->ctx);
            SSL_set_fd(client->ssl, client->socket);
            SSL_set_verify(client->ssl, SSL_VERIFY_PEER, NULL);
            if(SSL_connect(client->ssl) <= 0){
                SSL_free(client->ssl);
                SSL_CTX_free(client->ctx);
                close(client->socket);
                client->socket = -1;
                printf("There was an unknown error...\nMaybe your certificate is not installed or updated.\n");
            } else{
                pthread_attr_t attr;
                pthread_attr_init(&attr);
                pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
                pthread_create(&client->thread, &attr, receive_and_print_message, &client->socket);
                pthread_attr_destroy(&attr);
            }
        }
    }
    return client->socket;
}

static void* receive_and_print_message(void* polymorph){
    struct Client* client = polymorph;
    char* message = malloc(1000);
    while(true){
        fd_set lecture;
        FD_ZERO(&lecture);
        FD_SET(client->socket, &lecture);
        int ret_value = select(client->socket + 1, &lecture, NULL, NULL, NULL);
        int bytes_readed = SSL_read(client->ssl, message, 1000);
        if(ret_value == -1){
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
            SSL_write(client->ssl, message, length);
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
    SSL_shutdown(client->ssl);
    close(client->socket);
    pthread_join(client->thread, NULL);
    SSL_free(client->ssl);
    SSL_CTX_free(client->ctx);
}
