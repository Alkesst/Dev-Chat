cmake_minimum_required(VERSION 2.6)
project(chat)
add_executable(chat_server
    server/server_list.c
    server/server.c
    server/chat_server.c
)
add_executable(chat_client
    client/chat_client.c
)
