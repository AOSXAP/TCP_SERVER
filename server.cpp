#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include "color_output.h"

void __SETHINTS(struct addrinfo &hints){
    hints.ai_family = AF_INET; //ipv4
        hints.ai_socktype = SOCK_STREAM; // TCP
    
    hints.ai_flags = AI_PASSIVE; //bind to the wildcard address
}

void __SENDMESSAGE(std::string message , std::string color){
    Color::CPRINT(color , message);
}

#define THROW() std::cout<<errno<<"\n"; return -1;
#define INFINITE 1

int main()
{   
    //prepare struct addrinfo *hints
    struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
    
    __SETHINTS(hints); 
        __SENDMESSAGE("Server started!", "green");

    //fill res with needed info , 0 = 127.0.0.1, loopback adress
    getaddrinfo("192.168.1.6" , "8080" , &hints , &res);

    int server_socket = socket(res->ai_family , 
                res->ai_socktype , res->ai_protocol);

    //do the binding 
    if(bind(server_socket , res -> ai_addr , res -> ai_addrlen) == -1){
        THROW();
    }

    freeaddrinfo(res);
        __SENDMESSAGE("Binding successful!",  "green");

    //listen
    listen(server_socket , 10);
        __SENDMESSAGE("Waiting for connections!", "green");

    //accept incoming conections
    while(INFINITE)
    {
        struct sockaddr_storage client_address;  //info of the connecting client
            socklen_t client_len = sizeof(client_address);   

        int client_socket = accept(server_socket, (struct sockaddr*) 
                    &client_address , &client_len);

        if(!(client_socket >= 0)){
            __SENDMESSAGE("Accept failed!", "red"); THROW();
        }
        //ssize_t recv(int sockfd, void *buf, size_t len, int flags);


        char address_buffer[100] , request[1024];    
    
        getnameinfo((struct sockaddr*)&client_address, client_len, address_buffer, 
            sizeof(address_buffer), 0, 0, NI_NUMERICHOST); // 1 = NI_NUMERICHOST (see hostname as IP)

        //create child process
        int pid = fork();
        
        if(pid == 0){
            close(server_socket);

            recv(client_socket , request , 1024, 0);
            
             __SENDMESSAGE(request, "blue");

            send(client_socket , address_buffer , strlen(address_buffer) , 0);
                close(client_socket); exit(0);
        }

        close(client_socket);
    }   
}