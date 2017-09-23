# Dev-Chat
###### Chat in local network


## Introduction
A local network chat.

For this chat you we'll need CMake 3.0 and OpenSSL 1.0 or higher y pthread.
For this chat is required hosting the server in MacOS and Linux (Windows support will be added in the future).

## Configuring and running the Server
Server requires a public and private certificate in .pem extension.

For running the server you need to use this command
```bash
chat_server {port} {private_certificate_path} {public_certificate_path}
```
It starts the server listening on the specified port. If private and public certificates are not provided, will be using default values:

 - "key.pem" for private certificate
 - "public.pem" for public certificate


 Notice that the certificates must be in the current working directory.


 ## Joining to the server
To join to the server you only need the public certificate of the server and the address (IP or Hostname).

```bash
chat_client {address} {port} {public_certificate_path}
```

If public certificate is not provided will be using a default value:
 - "public.pem" for public certificate

Notice that the certificate must be in the current working directory.
