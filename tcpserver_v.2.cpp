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





class tcpserv
{
private:
    const int MAXLINE = 1024;
    const int LISTENQ = 10;
    const int SERV_PORT = 8080;
    int listenfd, connfd; 
    pid_t childpid; 
    socklen_t clilen; 
    struct sockaddr_in cliaddr, servaddr;
    std::vector<pid_t> processees;
    int optval = 1; 

public:
    void str_echo(int sockfd) {
        ssize_t n = 0;
        char buf[MAXLINE];
        while((n = read(sockfd, buf, MAXLINE)) > 0) {
            std::cout<< "Message from client: "; 
            std::cout<<std::string_view{buf, n} <<std::endl; 
            write(sockfd, buf, n);
        }
    }

    void kill_all(std::vector<pid_t>& processees){

        while(processees.size() > 0){
            kill(processees.back(),SIGKILL);
            processees.pop_back();
        }
    }

    int set_up_server(){
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
        
            std::cout << "Клиент подключился"  << std::endl;
            if((childpid = fork()) == 0) {
                prctl (PR_SET_PDEATHSIG, SIGUSR1); 
                close(listenfd);
                str_echo(connfd);
                std::cout << "Клиент отключился"  << std::endl;
                _exit(0);
            }
            processees.push_back(childpid);
            if(childpid == -1)
                perror("error in fork: ");
            
            close(connfd);
        }

        return 0;
    }
    
    tcpserv();
    ~tcpserv();


};

tcpserv::tcpserv()
{
    listenfd = socket(AF_INET, SOCK_STREAM, 0); 
    set_up_server();

}

tcpserv::~tcpserv()
{
    kill_all(processees);
    std::cout << "Сработал деструктор"  << std::endl;

}



int main() {
    tcpserv t;
    return 0;
}