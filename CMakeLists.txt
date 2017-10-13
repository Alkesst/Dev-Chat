cmake_minimum_required(VERSION 3.1.0)
project(chat)
find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})
add_executable(chat_server
    server/server_list.c
    server/server.c
    server/chat_server.c
)
add_executable(chat_client
    client/chat_client.c
)
target_link_libraries(chat_server ${OPENSSL_SSL_LIBRARY} Threads::Threads)
target_link_libraries(chat_client ${OPENSSL_SSL_LIBRARY} Threads::Threads)
