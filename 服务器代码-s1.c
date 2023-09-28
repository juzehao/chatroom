#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "mymsg.h"
#include <mysql/mysql.h>
#include <time.h>
#include <signal.h>
#include "socketsearch.h"
#define  NUM 32
sem_t mysem;
typedef struct Socket
{
    int num;
    char username[105];
}info;
MYSQL *conn;
MYSQL_RES *res;
int result;
MYSQL_ROW row;
char user[105];
info information[105];
int Num,i,num;
node *head;
pthread_mutexattr_t attr;
pthread_mutex_t mutex;
pthread_t Thread[1005],hreatbeat;
int Threadsocket[1005];
int Threadcount[1005];
typedef struct thread
{
	int sock;
	int thread_num;
}threadinformation;
void *hreat(void *p)
{
	while(1)
	{
		int i;
		sleep(5);
		printf("\n%d",num);
		for(i=0;i<num;i++)
		{
			if(Threadcount[i]!=-10)
			{
				printf("\n%d",Threadcount[i]);
				if(Threadcount[i]>0)
				{
					pthread_cancel(Thread[i]);
					Threadcount[i]=-10;
					dele(head,Threadsocket[i]);
					close(Threadsocket[i]);				
					printf("close one:%d\n",Threadsocket[i]);
				}
				else
				Threadcount[i]=3;
			}		
		}
	}
}
void sigroutine()
{
	int i;
	for(i=0;i<num;i++)
	pthread_cancel(Thread[i]);
	deleall(head);
	pthread_cancel(hreatbeat);
	mysql_close(conn);
	printf("close all the socket and thread!!!\n");
	return;
}
void * comm_cli_thread(void * p)
{
	msg mymsg;
	responsemeg rmsg;
	int msglen;
	int rmsglen;
	int pairfd;
	int i;
	threadinformation information=*(threadinformation *)p;
	int comm_sock=information.sock;
	int threadnum=information.thread_num;
	msglen=sizeof(msg);
	rmsglen=sizeof(responsemeg);
	
	char operator[64];
	while(1)
	{
		memset(&mymsg,'\0',msglen);
		memset(&rmsg,'\0',rmsglen);
		read(comm_sock,&mymsg,msglen);
		switch(mymsg.command)
		{
			case 1://login
			{
				sprintf(operator,"select * from users where username='%s';", mymsg.from);
				mysql_query(conn,operator);
				res=mysql_use_result(conn);
				int flag,t;
				if(res)
				{
					while((row=mysql_fetch_row(res))!=NULL)
					{	
						for(t=0;t<mysql_num_fields(res);t++)
						{
							printf("%s",row[1]);
							if(strcmp(row[1],mymsg.mess[0])==0)
								flag=1;
						}
					}
				}
				mysql_free_result(res);
				//mysql_close(conn);
				rmsg.command=flag;
				write(comm_sock,&rmsg,rmsglen);
				break;
			}
			case 2://register
			{
				
				sprintf(operator,"insert into users values('%s','%s');", mymsg.mess[0],mymsg.mess[1]);
				result=mysql_query(conn,operator);
				if(result==1)
				rmsg.command=2;
				else
				rmsg.command=1;
				write(comm_sock,&rmsg,rmsglen);
				char folderPath[20];
				if(rmsg.command==1)
				{
					sprintf(folderPath,"mkdir -p ./%s",mymsg.mess[0]); 
			      	system(folderPath);
				}
				break;
			}
			case 3://chat
			{
				time_t timep;
				//printf("123");
			    char t[20];
			    struct tm *p;
			    time(&timep);
			    p = gmtime(&timep);
			     //printf("123");
			    sprintf(t,"%d-%d-%d-%d-%d-%d", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday,(p->tm_hour+6)%24,p->tm_min, p->tm_sec);
			    //printf("%s",t);
				sprintf(operator,"insert into chat values('%s','%s','%s','%s',0);", mymsg.from,mymsg.to,t,mymsg.mess[0]);
				printf("%s",operator);
				mysql_query(conn,operator);
				//mysql_close(conn);
				sprintf(operator,"%s:\nmessage:%s\ntime:%s", mymsg.from,mymsg.mess[0],t);
				rmsg.command=1;
				strcpy(rmsg.from,mymsg.from);
				strcpy(rmsg.to,mymsg.to);
				rmsg.len=1;
				strcpy(rmsg.mess[0],operator);
				int to=find(head,mymsg.from);
				rmsg.command=2;
				printf("%d",to);
				if(to!=-1)
				{
					write(to,&rmsg,rmsglen);	
				}
				to=find(head,mymsg.to);
				if(to!=-1)
				{
					write(to,&rmsg,rmsglen);
				}
				break;
			}
			case 4://groupchat
			{
				
				time_t timep;
			    char t[20];
			    struct tm *p;
			    time(&timep);
			    p = gmtime(&timep);
			    sprintf(t,"%d-%d-%d-%d-%d-%d", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday,(p->tm_hour+6)%24,p->tm_min, p->tm_sec);
			    //printf("%s",t);
				sprintf(operator,"insert into chat values('%s','%s','%s','%s',1);", mymsg.from,mymsg.to,t,mymsg.mess[0]);
				mysql_query(conn,operator);
				//mysql_close(conn);
				sprintf(operator,"%s:\nmessage:%s\ntime:%s", mymsg.from,mymsg.mess[0],t);
				rmsg.len=2;	
				strcpy(rmsg.mess[0],operator);
				strcpy(rmsg.mess[1],mymsg.to);	
				rmsg.command=3;
				node * newhead=(node *)malloc(sizeof(node));
				newhead=head;
				while(newhead!= NULL)
				{
					sprintf(operator,"select * from groupchat where N='%s' and U='%s';", mymsg.to,newhead->s);
					printf("%s",operator);
					result=mysql_query(conn,operator);
					if(result)
					write(newhead->num,&rmsg,rmsglen);	
					newhead=newhead->next;
				}
				break;
			}
			case 5://query
			{
				
				sprintf(operator,"select * from chat where ((F='%s' and T='%s')|(T='%s' and F='%s')) and type=0;", mymsg.from,mymsg.to,mymsg.from,mymsg.to);
				printf("%s",operator);	
				mysql_query(conn,operator);
				res=mysql_use_result(conn);
				int flag,t;
				char message[256];
				int tt=0;
				if(res)
				{
					while((row=mysql_fetch_row(res))!=NULL)
					{	
						message[0]='\0';
						for(t=0;t<mysql_num_fields(res);t++)
						{
							sprintf(message,"%s:\nmessage:%s\ntime:%s", row[0],row[3],row[2]);
							strncpy(rmsg.mess[tt],message,strlen(message));
						}
						tt++;
					}
				}
				rmsg.len=tt;
				mysql_free_result(res);
				//mysql_close(conn);
				rmsg.command=1;
				write(comm_sock,&rmsg,rmsglen);
				break;
			}
			case 6://groupquery
			{
				sprintf(operator,"select * from groupchat where N='%s' and U='%s';",mymsg.mess[0],mymsg.mess[1]);
				MYSQL_RES *res;
				mysql_query(conn,operator);
				res=mysql_use_result(conn);
				int flag,t;
				char message[256];
				int tt=0;
				if(res)
				{
					while((row=mysql_fetch_row(res))!=NULL)
					{	
						tt++;
					}
					rmsg.len=tt;
				}
				if(rmsg.len==0)
				rmsg.len=-1;
				mysql_free_result(res);
				rmsg.command=5;
				write(comm_sock,&rmsg,rmsglen);
				break;
			}
			case 7:
			{
				insert(head,mymsg.from,comm_sock);
				sprintf(operator,"select * from file where T='%s';", mymsg.from);
				printf("%s",operator);
				mysql_query(conn,operator);
				res=mysql_use_result(conn);
				int flag,t;
				int tt=0;
				if(res)
				{
					while((row=mysql_fetch_row(res))!=NULL)
					{
						for(t=0;t<mysql_num_fields(res);t++)
						{
							strcpy(rmsg.mess[tt],row[0]);
						}
						tt++;
					}
				}
				rmsg.len=tt;
				sprintf(operator,"delete from file where T='%s';", mymsg.from);
				mysql_query(conn,operator);
				mysql_free_result(res);
				//mysql_close(conn);
				rmsg.command=4;
				write(comm_sock,&rmsg,rmsglen);
				for(i=0;i<rmsg.len;i++)
				{
					FILE *in;
					sprintf(operator,"./%s/%s", rmsg.mess[i],mymsg.from);
					in=fopen(operator,"rb");
					char buff[1024] = {0};  //缓冲区
			    		int nCount;	
				   while( (nCount = fread(buff, 1, 1024, in)) )
					{
						send(comm_sock, buff, nCount, 0);
			    	}
					fclose(in);
				}
				break;
			}
			case 8:
			{
				int to=find(head,mymsg.to);
				if(to!=-1)
				{
					rmsg.len=1;
					strcpy(rmsg.mess[0],mymsg.from);
					rmsg.command=4;
					write(to,&rmsg,rmsglen);
				}
				else
				{
					sprintf(operator,"insert into file values('%s','%s','');",mymsg.from,mymsg.to);
					mysql_query(conn,operator);
					//mysql_close(conn);
				}
				sprintf(operator,"./%s/%s",mymsg.from,mymsg.to);
				
				int length = 0;  
				char buffer[1024];
						length = recv(comm_sock, buffer, 1024, 0);
					if(to==-1)
			        	{
						FILE *fp = fopen(operator, "w");
						int write_length = fwrite(buffer, sizeof(char), length, fp);  
						fclose(fp);
					}
					else 
					{
						send(to, buffer, length, 0);
					}
					bzero(buffer, 1024);
				break;
			}
			case 9:
			{
				int count=0,nowc=0,i;			
				char nn[128];
				for(i=0;i<strlen(mymsg.mess[1]);i++)
				{
					if(mymsg.mess[1][i]==',')
					{
						nn[nowc]='\0';
						nowc=0;
						sprintf(operator,"insert into groupchat values('%s','%s');", mymsg.mess[0],nn);
						mysql_query(conn,operator);
					}		
					else
					nn[nowc++]=mymsg.mess[1][i];				
				}
				nn[nowc]='\0';
				sprintf(operator,"insert into groupchat values('%s','%s');",nn,mymsg.mess[0]);
				mysql_query(conn,operator);
				break;
			}
			case 10:
			{
			Threadcount[threadnum]--;
				break;
				
			}
			case 11:
			{
				sprintf(operator,"select * from chat where T='%s' and type=1;",mymsg.mess[0]);
				MYSQL_RES *res;
				mysql_query(conn,operator);
				res=mysql_use_result(conn);
				int flag,t;
				char message[256];
				int tt=0;
				if(res)
				{
					while((row=mysql_fetch_row(res))!=NULL)
					{	
						message[0]='\0';
						for(t=0;t<mysql_num_fields(res);t++)
						{
							sprintf(message,"%s:\nmessage:%s\ntime:%s", row[0],row[3],row[2]);
							strncpy(rmsg.mess[tt],message,strlen(message));
						}
						tt++;
					}
					rmsg.len=tt;
				}
				mysql_free_result(res);
				rmsg.command=6;
				write(comm_sock,&rmsg,rmsglen);
				break;
			}
		}
	}
	pthread_exit("ok");
}
int main()
{
	struct sockaddr_in ser_addr,cli_addr;
	int ser_sock,cli_sock;
	char buffer[NUM];
	int len;
	signal(SIGINT,sigroutine);
	sem_init(&mysem,0,1);
	ser_sock=socket(AF_INET,SOCK_STREAM,0);
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	ser_addr.sin_port=9765;
	len=sizeof(ser_addr);
	bind(ser_sock,(struct sockaddr*)&ser_addr,len);
	listen(ser_sock,5);
	head=(node *)malloc(sizeof(node));
	pthread_mutexattr_init(&attr);
	pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(&mutex, &attr);
	pthread_create(&hreatbeat,0,hreat,(void*)NULL);
	memset(Threadcount,0,sizeof(Threadcount));
	num=0;
	listen(ser_sock,10);
	char* server="localhost";//本地连接
	char* user="user";//
	char* password="JZH44554455";//mysql密码
	char* database="test";//数据库名
	conn=mysql_init(NULL);
	mysql_real_connect(conn,server,user,password,database,0,NULL,0);
	while(1)
	{
		len=sizeof(cli_addr);
		cli_sock=accept(ser_sock,(struct sockaddr*)&cli_addr,&len);
		printf("connecting:%d\n",cli_sock);
		threadinformation *msg = (threadinformation*)malloc(sizeof(threadinformation));
		msg->sock=cli_sock;
		msg->thread_num=num;
		Threadcount[num]=0;
		Threadsocket[num]=cli_sock;
		pthread_create(&Thread[num++],0,comm_cli_thread,(void*)msg);
	}
	sem_destroy(&mysem);
	exit(0);
}
