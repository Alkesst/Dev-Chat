#include "chat.h"
#include "server.h"

#include <stdlib.h>
//#include <>

static struct Server server;
void stopping_server(int number);


int main(int argc, const char* argv[]){
    int returnable = 0;
    int port = 3128;
    if(argc > 1){
        int local_var = atoi(argv[1]);
        if(local_var >= 0 && local_var <= 65535){
            port = local_var;
        } else{
            fprintf(stderr, "%s\n", "That's not a fucking port, you fucking piece of shit");
            /*Ends the program*/
            exit(2);
        }
    }
    if(open_server(&server, port)){
        printf("Server listening in the port %d\n", + port);
        start_polling(&server);
    } else{
        perror("An error has occurred");
        returnable = 1;
    }

    return returnable;
}

void stopping_server(int number){
    close_server(&server);
}
