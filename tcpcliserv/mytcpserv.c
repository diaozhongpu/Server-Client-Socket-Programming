#include	"unp.h"
#include	<time.h>
#include	<string.h>

int cliArray[100]={0};
int cliNum = 0;

void itoaSpace(int val, char* buf){
	
	//static char buf[32] = {0};
	char* bufend=&buf[strlen(buf)];
	
	int base = 10;
	int i = 3;
	

	for(i = 3; i >= 0 ; --i)
	{
		bufend[i]="0123456789"[val % base];
		val /= base;
	}

	bufend[4]=':';
	bufend[5]='\0';
}
void itoaNewline(int val, char* buf){
	
	//static char buf[32] = {0};
	char* bufend=&buf[strlen(buf)];
	
	int base = 10;
	int i = 3;
	

	for(i = 3; i >= 0 ; --i)
	{
		bufend[i]="0123456789"[val % base];
		val /= base;
	}

	bufend[4]=';';
	bufend[5]='\0';
}

void
str_process(int sockfd)
{
	ssize_t		n;
	char		buf[MAXLINE];
	int 		quit = 0;



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

	//if((n = read(sockfd, buf, MAXLINE)) > 0)
	if((n = Readline(sockfd, buf, MAXLINE)) > 0)
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
				printf("input: %s \n", buf);
				printf("now cliNum: %d\n", cliNum);
				buf[0]='S';
				buf[1]='e';
				buf[2]='r';
				buf[3]='i';
				buf[4]=' ';
				buf[5]='c';
				buf[6]='o';
				buf[7]='n';
				buf[8]='n';
				buf[9]=' ';
				buf[10]='\0';
				for(i=0; i<cliNum; i++)
				{
					//itoa(i, strtmp1, 5);
					itoaSpace(i, buf);
					printf("i: %d, %s\n", i, buf);
					//itoa(cliArray[i], strtmp2, 100);
					itoaNewline(cliArray[i], buf);
					printf("cliArray: %d, %s\n", cliArray[i], buf);
				}
				buf[strlen(buf)]=0x0a;
				Writen(sockfd, buf, strlen(buf)+1);
				break;
			case 's': //send message
				sendSerial=atoi(&buf[1]);
				printf("sendSerial: %d\n", sendSerial);
				if(sendSerial>=0 && sendSerial<cliNum)
				{	
					sockfd=cliArray[sendSerial]; //sockfd=sendconn
					//sbuf=buf;
					buf[strlen(buf)]=0x0a;
					Writen(sockfd, buf, strlen(buf)+1);//buf!! sbuf for all others
				}
				else
				{
					strcpy(buf, "Invalid Destination");
					buf[strlen("Invalid Destination")]=0x0a;
					Writen(sockfd, buf, strlen(buf)+1);
				}
				break;
			case 'q': //quit
				quit = 1;
				break;
		}
	}

// to del
// again:
	// while ( (n = read(sockfd, buf, MAXLINE)) > 0)
	// {
	// 	Writen(sockfd, buf, n);
	// }

	if (quit)
	{
		printf("quit\n");
		return;
	}
	else
	{
		buf[0]='\n';
		printf("cont\n");
		goto again;
	}

	// if (n < 0 && errno == EINTR)
	// 	goto again;
	// else if (n < 0)
	// 	err_sys("str_pro: read error");

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
		cliNum=cliNum+1;
		printf("New client: Serial=%d, connfd=%d, cliNum=%d\n", cliSrial, connfd, cliNum);

		if ( (childpid = Fork()) == 0) {	/* child process */
			Close(listenfd);	/* close listening socket */
			str_process(connfd);	/* process the request */
			//exit(0);

			if(cliSrial>=0)
			{
				cliNum--;
				if(cliSrial!=cliNum)
					cliArray[cliSrial]=cliArray[cliNum];
			}
			Close(connfd);			/* parent closes connected socket */
			printf("client closed: now cliNum=%d\n", cliNum);
		}


	}
}
