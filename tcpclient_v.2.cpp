#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <vector>
#include <algorithm>


const int num_of_client = 10;



class tcpclient
{
private:
    int sockfd;
    sockaddr_in adress;
    static const int MAXLINE = 1024;
    char sendline[MAXLINE], reciveline[MAXLINE];
    std::string message;


public:
    tcpclient();
    void create_socket(int family, int type, int protocol);
    int getsockfd();
    void set_address(int family, in_port_t port, in_addr_t address);
    void connect_socket();
    void str_cli(std::string message);
    void close_socket();
};
void tcpclient::set_address(int family, in_port_t port, in_addr_t address){
    memset(&adress, 0, sizeof(adress));
    adress.sin_family = family;
    adress.sin_port = port;
    adress.sin_addr.s_addr = address; 
}


tcpclient::tcpclient()
{
    
}


void tcpclient::create_socket(int family, int type, int protocols){

    sockfd = socket(family, type, protocols);
    if(sockfd < 0){
        std::cout<< "socket opening error"<<std::endl;
        exit(1);
    }
}
int tcpclient::getsockfd(){
    return sockfd;
}


void tcpclient::connect_socket(){
    if(sockfd >= 0){
        if(connect(sockfd, (struct sockaddr *) &adress, sizeof(adress)) != 0){
            perror("connect func error: ");
            exit(1);
        }
        std::cout<< "client is connected to serv: "<< sockfd<<std::endl;
    }
}


void tcpclient::str_cli(std::string message){
    memset(sendline, 0, sizeof(sendline));
    memset(reciveline, 0, sizeof(reciveline));
    strcpy(sendline, message.c_str());
    std::cout<< "Message from socket: " << sockfd<< " to server: "<<std::string_view{sendline,strlen(sendline)}<< std::endl; 
    write(sockfd, sendline, strlen(sendline));
    if (read(sockfd, reciveline, MAXLINE) == 0){
        perror("server terminated prematurely: ");
        exit(0);
    }
    std::cout<< "Message from server: " <<std::string_view{reciveline,strlen(sendline)}<< std::endl;    
}

void tcpclient::close_socket(){
    std::cout<< "socket is closed: "<< sockfd<<std::endl;
    close(sockfd);
}



int main(){
    std::vector<tcpclient> clients;
    int num_of_clients = 10;
    for (int i = 0; i < num_of_clients; i++)
    {
        tcpclient tcp;
        tcp.create_socket(AF_INET, SOCK_STREAM, 0);
        tcp.set_address(AF_INET, htons(8080), inet_addr("127.0.0.1"));
        tcp.connect_socket();
        clients.push_back(tcp);
    }
    for (int i = 0; i < num_of_clients; i++)
    {
        clients[i].str_cli("hello");

    }
    for (int i = 0; i < num_of_clients; i++)
    {
        clients[i].close_socket();
        clients.erase(clients.begin());
    }
}