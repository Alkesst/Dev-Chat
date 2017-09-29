# Dev-Chat
###### Chat in local network


## Introduction
This local network chat.

For this chat you we'll need CMake 3.0 and OpenSSL 1.0 or higher y pthread.
Hosting the server in MacOS and Linux is also required (Windows support will be added in the future).

## Configuring and running the Server
The server requires a public and private certificate in .pem extension.

To run the server you need to use this command
```bash
chat_server {port} {private_certificate_path} {public_certificate_path}
```
It starts the server listening on the specified port. If private and public certificates are not provided, default values will be used:

 - "key.pem" for private certificate
 - "public.pem" for public certificate


 Please notice that the certificates must be in the current working directory.


 ## Joining to the server
To join to the server you only need the server's adress (IP or Horstname) and public certificate.

```bash
chat_client {address} {port} {public_certificate_path}
```

If no public certificate is provided a default value will be used:
 - "public.pem" for public certificate

Please notice that this certificate must too be in the current working directory.
