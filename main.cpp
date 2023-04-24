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

struct CONNECT
{
private:
	int sockfd;
	int newsockfd;
	struct sockaddr_in serv_addr;
	char buf[MAXLINE];
public:
	void ConnectClient()
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
	void ConnectServer()
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
	/*remake*/
	void ServerListen()
	{
        listen( sockfd, 5);
        socklen_t clilen = sizeof( serv_addr );
        newsockfd = accept( sockfd, (struct sockaddr*)&serv_addr, &clilen);
        if( newsockfd < 0 ) err_sys( "server: accept error" );
        for( ; ; )
        {
        int bytes_read = recv(newsockfd, buf, MAXLINE, 0);
        if(bytes_read == 0)/* recv return 0, when connect disapeared */
        {
            close( sockfd );
            return;
        }
        std::cout << "server read" << std::endl;
        std::cout << content(buf, bytes_read) << std::endl;
        }
	}
	void CloseConnectClient()
	{
        close( sockfd );
	}
	void CliendSend( const std::string mess)
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
    CONNECT conect;
    std::string mess;
    switch(chpid)
    {
    case -1:
        err_sys("error fork");
        break;
    case 0:
        /*child process client*/
        conect.ConnectClient();
        break;
    default:
        /*parent process server*/
        conect.ConnectServer();
        break;
    }
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
                    conect.CliendSend( mess);
                    break;
                }
                conect.CliendSend( mess);
            }
            conect.CloseConnectClient();
            break;
        default:
            /*parent process server*/
            conect.ServerListen();
            break;
        }
    return 0;
}
