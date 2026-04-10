#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <algorithm>
#include <sys/wait.h>
#include <sys/prctl.h>
#include <signal.h>
#include <csignal>



std::vector<pid_t> connected_clients;



const int MAXLINE = 1024;
const int LISTENQ = 10;
const int SERV_PORT = 8080;

void str_echo(int sockfd) {
    ssize_t n = 0;
    char buf[MAXLINE];
    while((n = read(sockfd, buf, MAXLINE)) > 0) {
        std::cout<< "Message from client: "; 
        std::cout<<std::string_view{buf, n} <<std::endl; 
        write(sockfd, buf, n);
    }
}



void kill_all(int sig){
    while(connected_clients.size() > 0){
        kill(connected_clients.back(),SIGKILL);
        connected_clients.pop_back();
    }
    std::cout<< "Exit "; 

    exit(0);
}





int main() {

    signal(SIGINT, kill_all);

    int listenfd, connfd; 
    pid_t childpid; 
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr; 
    
    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    
    int optval = 1; 
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
        close(listenfd);
        return 1;
    }

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT); 
    
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) != 0)
    {   
        perror("error: ");
        return -1;

    }
    if(listen(listenfd, LISTENQ) != 0) {
        perror("error: ");
        return -1;
    }
    
    std::cout << "Сервер запущен на порту " << SERV_PORT << std::endl;
    
    while(true) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen); 
        if(connfd  == -1){
            std::cout << "Клиент не подключился"  << std::endl;
            exit(2);
        }
      
        std::cout << "Клиент подключился на сокете" <<std::endl;
        if((childpid = fork()) == 0) {
            //prctl (PR_SET_PDEATHSIG, SIGUSR1); 
            close(listenfd);
            str_echo(connfd);
            std::cout << "Клиент отключился"  << std::endl;
            _exit(0);
        }
        connected_clients.push_back(childpid);
        if(childpid == -1)
            perror("error in fork: ");
        
        close(connfd);


    }
    
    return 0;
}