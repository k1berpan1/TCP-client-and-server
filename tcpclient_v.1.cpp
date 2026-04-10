#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
const int MAXLINE = 1024;



void str_cli(FILE *fp, int sockfd){
    char sendline[MAXLINE], reciveline[MAXLINE];
    memset(sendline, 0, sizeof(sendline));
    memset(reciveline, 0, sizeof(reciveline));
    std::cout<< "Message to server: "; 

    while (fgets(sendline, MAXLINE, fp) != NULL){
    
        write(sockfd, sendline, strlen(sendline));
        if (read(sockfd, reciveline, MAXLINE) == 0){
            perror("server terminated prematurely: ");
            exit(0);
        }
        std::cout<< "Message from server: " <<reciveline<< std::endl; 
        //fputs(reciveline, stdout);
        memset(sendline, 0, sizeof(sendline));
        memset(reciveline, 0, sizeof(reciveline));
        std::cout<< "Message to server: "; 
    }
}


int main(){
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){
        std::cout<< "socket opening error"<<std::endl;
        exit(1);
    }
    std::cout<< "socket is opened"<<std::endl;

    sockaddr_in adress;
    memset(&adress, 0, sizeof(adress));
    adress.sin_family = AF_INET;
    adress.sin_port = htons(8080);
    adress.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    if(connect(sockfd, (struct sockaddr *) &adress, sizeof(adress)) != 0){
        perror("connect func error: ");
        exit(1);
    }
    std::cout<< "client is connected to serv"<<std::endl;
    str_cli(stdin, sockfd);
    exit(0);
}



