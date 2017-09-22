cmake_minimum_required(VERSION 2.6)
project(chat)
add_executable(chat
    server/server_list.c
    server/server.c
    server/chat.c
)
