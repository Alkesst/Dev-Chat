# Dev-Chat
###### A chat made by developers, for developers.



## Introduction

For this chat you we'll need CMake 3.0 and OpenSSL 1.0, or higher, & pthread.
Hosting & joining the server requires MacOS and Linux (Windows support will be added in the future as soon as possible).

### Configuring and running the Server
#### Creating the certificates
The server requires a public and private certificate in .pem extension.
Execute the following command to create them:
```bash
openssl req -x509 -newkey rsa:4096 -sha256 -nodes -keyout key.pem -out public.pem -days 365
```
Is possible to change the name of both certificates.
#### Running the server
To run the server you need to use this command
```bash
chat_server {port} {private_certificate_path} {public_certificate_path}
```
It starts the server listening on the specified port. If private and public certificates are not provided, default values will be used:

 - "key.pem" for private certificate
 - "public.pem" for public certificate


 Please notice that the certificates must be in the current working directory.


## Joining to the server
To join to the server you only need the server's address (IP or Horstname) and public certificate. The public certificate must be provided by the server owner.

```bash
chat_client {address} {port} {public_certificate_path}
```

If no public certificate is provided a default value will be used:
 - "public.pem" for public certificate

Please notice that this certificate must be in the current working directory.


### TODO
Create an interface for Linux (KDE & gnome) and macOS (Swift)
