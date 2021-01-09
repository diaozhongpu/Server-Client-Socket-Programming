#include	"unp.h"
#include	<time.h>
#include	<string.h>

int cliArray[100]={0};
int cliNum = 0;

void itoa(int val, char* buf){
	
	//static char buf[32] = {0};
	char* bufend=&buf[strlen(buf)];
	
	int base = 10;
	int i = 4;
	
	buf[base]='\0';

	for(i = 4; i >= 0 ; --i)
	{
		bufend[i]="0123456789"[val % base];
		val /= base;
	}
	

}

void
str_process(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];



	//char		sbuf[10000];
	//sbuf=(char*)malloc(sizeof(char)*10000);


	time_t rawtime;
 	struct tm * timeinfo;
 	time ( &rawtime );
	timeinfo = localtime ( &rawtime );

	int i;
	char		*strtmp1;// *strtmp2;
	strtmp1=(char*)malloc(sizeof(char)*10);
	strtmp1=(char*)malloc(sizeof(char)*100);

	int sendSerial;
	//int sendconn;
	//char *tbuf;
again:

	if((n = read(sockfd, buf, MAXLINE)) > 0)
	{
		switch(buf[0])
		{
			case 't': //return time
				strcpy(buf, asctime (timeinfo));
				Writen(sockfd, buf, strlen(asctime(timeinfo)));
				break;
			case 'n': //return name
				strcpy(buf, "This is Diao's Computer");
				buf[strlen("This is Diao's Computer")]=0x0a;
				Writen(sockfd, buf, strlen(buf)+1);
				break;
			case 'l': //return list
				buf[0]=0x0a;
				for(i=0; i<cliNum; i++)
				{
					//itoa(i, strtmp1, 5);
					itoa(i, buf);
					//itoa(cliArray[i], strtmp2, 100);
					itoa(cliArray[i], buf);
				}
				buf[strlen(buf)]=0x0a;
				Writen(sockfd, buf, strlen(buf)+1);
				break;
			case 's': //send message
				sendSerial=atoi(&buf[1]);
				if(sendSerial>=0 && sendSerial<cliNum)
				{	
					sockfd=cliArray[sendSerial]; //sockfd=sendconn
					//sbuf=buf;
					Writen(sockfd, buf, n);//buf!! sbuf for all others
				}
				else
				{
					strcpy(buf, "Invalid Destination");
					buf[strlen("Invalid Destination")]=0x0a;
					Writen(sockfd, buf, strlen(buf)+1);
				}
				break;
		}
	}

// to del
// again:
	while ( (n = read(sockfd, buf, MAXLINE)) > 0)
	{
		Writen(sockfd, buf, n);
	}

	if (n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		err_sys("str_pro: read error");

}

int
main(int argc, char **argv)
{
	int					listenfd, connfd;
	int 				cliSrial = -1;
	pid_t				childpid;
	socklen_t			clilen;
	struct sockaddr_in	cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		cliSrial=cliNum;
		cliArray[cliNum]=connfd;
		cliNum++;

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			str_process(connfd);	/* process the request */
			exit(0);
		}

		if(cliSrial==cliNum-1)
		{
			cliNum--;
		}
		else if(cliSrial==-1)
		{}
		else
		{
			cliNum--;
			cliArray[cliSrial]=cliArray[cliNum];
		}
		Close(connfd);			/* parent closes connected socket */
	}
}
