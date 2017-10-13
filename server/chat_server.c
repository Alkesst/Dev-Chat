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
#include "server.h"

#include <stdlib.h>
// Server Main.

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
            fprintf(stderr, "Argument is requiered: Expected port\n");
            /*Ends the program*/
            exit(2);
        }
        if(argc > 3){
            server.private_path = argv[2];
            server.public_path = argv[3];
        } else {
            server.private_path = "key.pem";
            server.public_path = "public.pem";
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
