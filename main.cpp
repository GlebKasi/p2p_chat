#include "inet.h"

/*class one object for bind detwen client and server*/
/*another for painting */
/*we should use fork*/
/*do some comands*/
/*help describe some rules*/
/*mess: <message> send message*/
/*ipshow show your IP*/
/**/

std::string content( const char* mes, int bytes_read)
{
    std::string result(bytes_read + 1, '\n');
    for(int i = 0; i < bytes_read; i++)
    {
        result[i] = mes[i];
    }
    return result;
}

void err_sys( const char* mes )
{
    perror( mes );
    exit(1);
}

class CONNECT
{
protected:
	int sockfd;
	int newsockfd;
	struct sockaddr_in serv_addr;
	char buf[MAXLINE];
public:
    virtual void Connect() = 0;
    virtual void ServerListen()
    {}
    virtual void CloseConnectClient()
    {}
    virtual void CliendSend( const std::string mess)
    {}
};

class SERVER: public CONNECT
{
    void Connect() override
	{
        bzero( (char*)&serv_addr, sizeof( serv_addr ) );
		serv_addr.sin_family		= AF_INET;
		serv_addr.sin_addr.s_addr	= htonl( INADDR_ANY );/**/
		serv_addr.sin_port	        = htons( SERV_TCP_PORT );
		std::cout << serv_addr.sin_port << std::endl;
		/*Open a TCP socket*/
		if(( sockfd = socket( AF_INET, SOCK_STREAM, 0)) < 0 )
            err_sys( "server: can't open stream socket" );
        if( bind( sockfd, (struct sockaddr*)&serv_addr, sizeof( serv_addr )) < 0)
            err_sys( "server: can't bind local address");
        std::cout << "server connect" << std::endl;
	}

	void ServerListen() override
	{
        listen( sockfd, 5);
        socklen_t clilen = sizeof( serv_addr );
        newsockfd = accept( sockfd, (struct sockaddr*)&serv_addr, &clilen);
        if( newsockfd < 0 ) err_sys( "server: accept error" );
        for( ; ; )
        {
        int bytes_read = recv(newsockfd, buf, MAXLINE, 0);
        if(bytes_read == 0)
        {
            close(newsockfd);
            return;
        }
        std::cout << "server read" << std::endl;
        std::cout << content(buf, bytes_read) << std::endl;
        }
	}
};

class CLIENT: public CONNECT
{
    void Connect() override
	{
        bzero( (char*)&serv_addr, sizeof( serv_addr ) );
		serv_addr.sin_family		= AF_INET;
		serv_addr.sin_addr.s_addr	= inet_addr( SERV_TCP_IP );/**/
		serv_addr.sin_port	        = htons( SERV_TCP_PORT );
		/*Open a TCP socket*/
		if(( sockfd = socket( AF_INET, SOCK_STREAM, 0)) < 0 )
            err_sys( "client: can't open stream socket" );
        if( connect( sockfd, (struct sockaddr*)&serv_addr, sizeof( serv_addr )) < 0)
            err_sys( "client: can't connect to server");
        std::cout << "client connect" << std::endl;
	}
	void CloseConnectClient() override
	{
        close( sockfd );
	}
	void CliendSend( const std::string mess) override
	{
        for(int i = 0; i < mess.size(); i++)
        {
            buf[i] = mess[i];
        }
        send(sockfd, buf, mess.size(), 0);
        std::cout << "client send" << std::endl;
	}
};

int main(int arg, char* argv[])
{
    pid_t chpid = fork();
    std::string mess;
    CONNECT* sender;
    switch(chpid)
    {
    case -1:
        err_sys("error fork");
        break;
    case 0:
        /*child process client*/
        sender = new CLIENT;
        break;
    default:
        /*parent process server*/
        sender = new SERVER;
        break;
    }
    sender->Connect();
        switch(chpid)
        {
        case -1:
            err_sys("error fork");
            break;
        case 0:
            /*child process client*/
            for( ; ; )
            {
                getline(std::cin, mess);
                if(mess == "exit")
                {
                    sender->CliendSend( mess);
                    break;
                }
                sender->CliendSend( mess);
            }
            sender->CloseConnectClient();
            break;
        default:
            /*parent process server*/
            sender->ServerListen();
            break;
        }
    return 0;
}
