#include <gtk/gtk.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#define NUM 32
#define BUF_SIZE 10005
#include "mymsg.h"
int clientfd,b_file;
gchar *name,*changename,*nowname,*text,*gname,*guser;
GtkWidget *window;
char filename[105];
//发送目标用户窗口。
GtkWidget *entryname;
// 文本框缓冲区。
GtkTextBuffer *bufferuser;
GtkTextBuffer *bufferuser_group;
char username[105];
GtkWidget *entryname,*groupname,*groupuser;
pthread_mutex_t mute;
GtkWidget *view,*view_group;
static GtkWidget *entry; 
int comm_sock;
int i;
int flag;
int start;
char now[105];
char userall[1024],groupall[1024];
typedef struct Student
{
    char from[25];
    char to[25];
    int command;
	int length;
	char mess[25];
}info;
void * write_thread(void *arg)
{
	msg sendmsg;
	int msglen=sizeof(sendmsg);
	info *information=(info *)arg;
	switch(information->command)
	{
		case 0:
		{
			sendmsg.command=6;
			strcpy(sendmsg.mess[1],username);
			strcpy(sendmsg.mess[0],gname);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 1:
		{	
			sendmsg.command=5;
			strcpy(sendmsg.from,username);
			strcpy(sendmsg.to,nowname);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 2:
		{
			sendmsg.command=7;
			strcpy(sendmsg.from,username);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 3:
		{
			sendmsg.command=3;
			strcpy(sendmsg.from,username);
			strcpy(sendmsg.to,name);
			strcpy(sendmsg.mess[0],text);
			//printf("write1:%s",nowname);
			//printf("1231");
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 4:
		{
			sendmsg.command=4;
			strcpy(sendmsg.from,username);
			strcpy(sendmsg.mess[0],text);
			strcpy(sendmsg.to,gname);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 5:
		{
			//printf("123");
			sendmsg.command=8;
			strcpy(sendmsg.from,username);
			strcpy(sendmsg.to,name);
			write(comm_sock,&sendmsg,msglen);
			break;
		}	
		case 6:
		{
			sendmsg.command=10;
			strcpy(sendmsg.from,username);
			write(comm_sock,&sendmsg,msglen);
			break;
		}	
		case 7:
		{
			sendmsg.command=9;	
			strcpy(sendmsg.mess[0],gname);
			strcpy(sendmsg.mess[1],guser);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
		case 8:
		{
			sendmsg.command=11;	
			strcpy(sendmsg.mess[0],gname);
			write(comm_sock,&sendmsg,msglen);
			break;
		}
	}
	//printf("write2:%s",nowname);
}

void * read_thread(void *arg)
{
	while(1)
	{
		
		responsemeg rmsg;
		int rmsglen;
		rmsglen=sizeof(responsemeg);	
		memset(&rmsg,'\0',rmsglen);	
		read(comm_sock,&rmsg,rmsglen);
		printf("\n%d\n",rmsg.command);
		switch(rmsg.command)
		{
			
			case 1:
			{
				memset(userall,'\0',sizeof(userall));		
				for(i=0;i<rmsg.len;i++)		
				sprintf(userall,"%s\n%s",userall,rmsg.mess[i]);
				//printf("%s");info *msg = (info*)malloc(sizeof(info));
				gtk_text_buffer_set_text(bufferuser,userall,strlen(userall));	
				gtk_text_view_set_buffer(GTK_TEXT_VIEW(view),bufferuser);	
				flag=0;
				break;		
			}
			case 2:
			{
				if((strcmp(rmsg.to,now)==0&&strcmp(rmsg.from,username)==0)||(strcmp(rmsg.to,username)==0&&strcmp(rmsg.from,now)==0))
				{
					sprintf(userall,"%s\n%s",userall,rmsg.mess[0]);
					gtk_text_buffer_set_text(bufferuser,userall,strlen(userall));	
					gtk_text_view_set_buffer(GTK_TEXT_VIEW(view),bufferuser);	
				}			
				break;
			}
			case 3:
			{
				if(gname==NULL)
				break;
				if(strcmp(rmsg.mess[1],gname)==0)
				{
					sprintf(groupall,"%s\n%s",groupall,rmsg.mess[0]);
					gtk_text_buffer_set_text(bufferuser_group,groupall,strlen(groupall));	
					gtk_text_view_set_buffer(GTK_TEXT_VIEW(view_group),bufferuser_group);
				}
				break;
			}
			case 4:
			{	
				if(rmsg.len>0)
				{
					//printf("start:%d",rmsg.len);
				//sleep(0.5);
					char file[105];
					sprintf(file,"from%s",rmsg.mess[0]);
					FILE *fp = fopen(file, "w");
					char buffer[1024];
					int length;
					//printf("start");	
				      while(length = recv(comm_sock, buffer, 1024, 0))  
				    	{  
						if(length==0)
						break;
						//printf("\n%d\n",length);
				       	if (length < 0)  
						{  
					    	printf("Recieve Data From Server Failed!\n");  
					    	break;  
						}
						int write_length = fwrite(buffer, sizeof(char), length, fp);  
						if (write_length < length)  
						{  
					    	printf("File:Write Failed!\n");  
					    	break;  
						}  
						if(length<1024)
						break;
						bzero(buffer, 1024);  		
					}
					//printf("end");	
					fclose(fp);
				GtkWidget *dialog;


  dialog = gtk_message_dialog_new(window,


            GTK_DIALOG_DESTROY_WITH_PARENT,


            GTK_MESSAGE_INFO,


            GTK_BUTTONS_OK,


            "Download success", "title");


  gtk_window_set_title(GTK_WINDOW(dialog), "Information");


  gtk_dialog_run(GTK_DIALOG(dialog));


  gtk_widget_destroy(dialog);}
	break;
			}
		case 5:
			{	
				if(rmsg.len==-1)
				{
					GtkWidget *dialog;


  dialog = gtk_message_dialog_new(window,


            GTK_DIALOG_DESTROY_WITH_PARENT,


            GTK_MESSAGE_INFO,


            GTK_BUTTONS_OK,


            "You are not in the group", "title");


  gtk_window_set_title(GTK_WINDOW(dialog), "Information");


  gtk_dialog_run(GTK_DIALOG(dialog));


  gtk_widget_destroy(dialog);
				}
				else
				{
					info *msg = (info*)malloc(sizeof(info));	
					msg->command=8;
					pthread_t tid;
					pthread_create(&tid,0,write_thread,(void *)msg);	
					//printf("\\%d\\",rmsg.len);
				}
				break;
			}
			case 6:
			{
					groupall[0]='\0';
					for(i=0;i<rmsg.len;i++)		
					sprintf(groupall,"%s\n%s",groupall,rmsg.mess[i]);	
					//printf("%s",groupall);
					gtk_text_buffer_set_text(bufferuser_group,groupall,strlen(groupall));	
					gtk_text_view_set_buffer(GTK_TEXT_VIEW(view_group),bufferuser_group);
			}
		}
		//printf("%d",rmsg.command);
	}
}
void * hreatbeat(void *arg)
{
	info *msg = (info*)malloc(sizeof(info));
	while(1)
	{
		sleep(1);
		pthread_t tid;
		msg->command=6;			
		pthread_create(&tid,0,write_thread,(void *)msg);
	}
}
void sendtouser(GtkButton *button, gpointer entry);
void sendtogroup(GtkButton *button, gpointer entry);
void selectfile(GtkWidget *widget,gpointer data);
void deliverfile(GtkWidget *widget,gpointer data);

void inline init();
void changeone(GtkButton *button, gpointer entry)
{
	nowname = gtk_entry_get_text(GTK_ENTRY(entryname));
	gname=gtk_entry_get_text(GTK_ENTRY(groupname));
	if(strcmp(nowname,""))
	{
	strcpy(now,nowname);
	gtk_window_set_title(GTK_WINDOW(window), username);
	info *msg = (info*)malloc(sizeof(info));	
	msg->command=1;
	pthread_t tid;
	pthread_create(&tid,0,write_thread,(void *)msg);	
	}
	else
	{
	info *msg = (info*)malloc(sizeof(info));	
	msg->command=0;
	pthread_t tid;
	pthread_create(&tid,0,write_thread,(void *)msg);	
	}
}
//
int main ( int argc , char* argv[])
{
	if(argc>1)
	strncpy(username,argv[0],strlen(argv[0]));
	else
	strncpy(username,"1230",4);	
	struct sockaddr_in  ser_addr;
	int len;
	pthread_mutex_init(&mute,NULL); 
	flag=1;
	start=1;
	strcpy(now,"-1");
	
	comm_sock=socket(AF_INET,SOCK_STREAM,0);
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=9765;
	len=sizeof(ser_addr);
	info *msg = (info*)malloc(sizeof(info));
	connect(comm_sock,(struct sockaddr*)&ser_addr,len);
	pthread_t tid,listen_tid,user_tid,hreat;
	msg->command=2;	
	strcpy(msg->from,username);
	pthread_create(&user_tid,0,write_thread,(void *)msg);
	pthread_join(user_tid,NULL);	
	pthread_create(&listen_tid,0,read_thread,NULL);	
	pthread_create(&hreat,0,hreatbeat,NULL);	
	gtk_init(&argc, &argv);	
	init();	
}
void makegg(GtkButton *button, gpointer entry)
{
	info *msg = (info*)malloc(sizeof(info));
	pthread_t tid;
	gname = gtk_entry_get_text(GTK_ENTRY(groupname));	//获得行编辑
	guser = gtk_entry_get_text(GTK_ENTRY(groupuser));	//获得行编辑
	printf("\n%s\n",gname);
	msg->command=7;
	pthread_create(&tid,0,write_thread,(void *)msg);
	pthread_join(tid,NULL);
}
void sendtouser(GtkButton *button, gpointer entry)
{
	text = gtk_entry_get_text(GTK_ENTRY(entry));//获得行编辑entry的内容并静态建立text指针进行指定。		
	name = gtk_entry_get_text(GTK_ENTRY(entryname));	//获得行编辑	
	info *msg = (info*)malloc(sizeof(info));	
	msg->command=3;	
	pthread_t tid;
	pthread_create(&tid,0,write_thread,(void *)msg);	
}
void sendtogroup(GtkButton *button, gpointer entry){
	text = gtk_entry_get_text(GTK_ENTRY(entry));//获得行编辑entry的内容并静态建立text指针进行指定。
	gname = gtk_entry_get_text(GTK_ENTRY(groupname));		
	info *msg = (info*)malloc(sizeof(info));
	msg->command=4;	
	pthread_t tid;
	pthread_create(&tid,0,write_thread,(void *)msg);
}
void print_filename(GtkWidget *file)
{
   	gchar *newfile;
    newfile=gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file));
	strcpy(filename,newfile);
    gtk_widget_destroy(file);

} 
void selectfile(GtkWidget *widget,gpointer data){
 	   GtkWidget *file;
    int file_return;
    file=gtk_file_chooser_dialog_new("SelectFile",NULL,GTK_FILE_CHOOSER_ACTION_OPEN,GTK_STOCK_CANCEL,GTK_RESPONSE_CANCEL,GTK_STOCK_OK,GTK_RESPONSE_ACCEPT,NULL);
    if(gtk_dialog_run(GTK_DIALOG(file))==GTK_RESPONSE_ACCEPT)
        print_filename(file);
    else
        gtk_widget_destroy(file); 
}

void deliverfile(GtkWidget *widget,gpointer data)
{
	info *msg = (info*)malloc(sizeof(info));
	pthread_t tid;
	name = gtk_entry_get_text(GTK_ENTRY(entryname));	//获得行编辑	
	msg->command=5;
	pthread_create(&tid,0,write_thread,(void *)msg);
	pthread_join(tid,NULL);
	FILE *in;
	in=fopen(filename,"rb");
	char buff[BUF_SIZE] = {0};  //缓冲区
    	int nCount,sum=0;	
	   while( (nCount = fread(buff, 1, BUF_SIZE, in)) > 0 ){
        send(comm_sock, buff, nCount,0);
    		}
	fclose(in);
	
	
	GtkWidget *dialog;


  dialog = gtk_message_dialog_new(window,


            GTK_DIALOG_DESTROY_WITH_PARENT,


            GTK_MESSAGE_INFO,


            GTK_BUTTONS_OK,


            "upload success", "title");


  gtk_window_set_title(GTK_WINDOW(dialog), "Information");


  gtk_dialog_run(GTK_DIALOG(dialog));


  gtk_widget_destroy(dialog);
	return;
}
void inline init()
{
	GtkWidget *scrolled;
	GtkWidget *scrolled_group;//群发
	// 初始化 			
	// 创建顶层窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	// 设置窗口的标题
	gtk_window_set_title(GTK_WINDOW(window), username);
	// 设置窗口在显示器中的位置为居中
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	// 设置窗口的最小大小
	gtk_widget_set_size_request(window, 820, 470);

	
	// 固定窗口的大小
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE); 
	// "destroy" 和 gtk_main_quit 连接
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	//创建一个固定容器
 	GtkWidget *fixed = gtk_fixed_new(); 	
	gtk_container_add(GTK_CONTAINER(window), fixed);
	
	//单聊滚动条
	scrolled = gtk_scrolled_window_new(NULL,NULL); /*创建滚动窗口构件*/
	gtk_container_add(GTK_CONTAINER(window),scrolled);/*将滚动窗口构件加入窗体*/
	gtk_widget_show(scrolled);/*显示滚动窗口构件*/
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);/*滚动条属性*/
	//群聊滚动条
	scrolled_group = gtk_scrolled_window_new(NULL,NULL); /*创建滚动窗口构件*/
	gtk_container_add(GTK_CONTAINER(window),scrolled_group);/*将滚动窗口构件加入窗体*/
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_group),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);/*滚动条属性*/
	gtk_widget_show(scrolled_group);/*显示滚动窗口构件*/
 	
	GtkWidget *label_two;
	GtkWidget *label_one;
	GtkWidget *label_three;
	GtkWidget *label_four;
	// 创建标签
	label_one = gtk_label_new("单聊");
	label_three = gtk_label_new("群聊");
	// 将按钮放在布局容器里
	gtk_fixed_put(GTK_FIXED(fixed), label_one,20,10);
	gtk_fixed_put(GTK_FIXED(fixed), label_three,320,10);	
 
	// 创建标签
	label_two = gtk_label_new("系统通知：");
	//gtk_fixed_put(GTK_FIXED(fixed), label_two,320,10);
 
	label_two = gtk_label_new("选择用户：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,630,10);
	label_four = gtk_label_new("群聊名：");
	gtk_fixed_put(GTK_FIXED(fixed), label_four,630,90);
	label_four = gtk_label_new("群聊用户：");
	gtk_fixed_put(GTK_FIXED(fixed), label_four,630,150);
	label_two = gtk_label_new("发送内容：");
	gtk_fixed_put(GTK_FIXED(fixed), label_two,16,295);
	

	// 行编辑的创建_entry
	entryname = gtk_entry_new();
	//最大长度 
	gtk_entry_set_max_length(GTK_ENTRY(entryname),500);
	gtk_editable_set_editable(GTK_EDITABLE(entryname), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entryname,625,30); 

 	groupname = gtk_entry_new();
	//最大长度 
	gtk_entry_set_max_length(GTK_ENTRY(groupname),500);
	gtk_editable_set_editable(GTK_EDITABLE(groupname), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), groupname,625,110); 

	groupuser = gtk_entry_new();
	//最大长度 
	gtk_entry_set_max_length(GTK_ENTRY(groupuser),500);
	gtk_editable_set_editable(GTK_EDITABLE(groupuser), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), groupuser,625,170); 
	
	GtkWidget *entry = gtk_entry_new();	
	gtk_entry_set_max_length(GTK_ENTRY(entry),500);
	gtk_widget_set_usize(entry, 600, 50);
	gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
	gtk_fixed_put(GTK_FIXED(fixed), entry,20,330);	
		
	//行编辑的创建_text
	//entryname = gtk_text_view_new();
	
	// 创建按钮
	GtkWidget *bsend = gtk_button_new_with_label("发送");
	gtk_button_set_relief(GTK_BUTTON(bsend), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), bsend,520,390);
 
	GtkWidget *send_all = gtk_button_new_with_label("群发");
	gtk_button_set_relief(GTK_BUTTON(send_all), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), send_all,570,390);	

	GtkWidget *change = gtk_button_new_with_label("--转 换--");
	gtk_button_set_relief(GTK_BUTTON(change), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), change,675,400);

	GtkWidget *choose = gtk_button_new_with_label("选择文件");
	gtk_button_set_relief(GTK_BUTTON(choose), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), choose,675,300);

	GtkWidget *makegroup = gtk_button_new_with_label("创建");
	gtk_button_set_relief(GTK_BUTTON(makegroup), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), makegroup,675,250);

	GtkWidget *deliver = gtk_button_new_with_label("发送文件");
	gtk_button_set_relief(GTK_BUTTON(deliver), GTK_RELIEF_NONE);
	gtk_fixed_put(GTK_FIXED(fixed), deliver,675,350);

	
 
	// 绑定回调函数
	g_signal_connect(bsend, "pressed", G_CALLBACK(sendtouser), entry);
	g_signal_connect(send_all, "pressed", G_CALLBACK(sendtogroup), entry);
	g_signal_connect(change, "pressed", G_CALLBACK(changeone), entry);
	g_signal_connect(choose, "pressed", G_CALLBACK(selectfile), entry);
	g_signal_connect(deliver, "pressed", G_CALLBACK(deliverfile), entry);
	g_signal_connect(makegroup, "pressed", G_CALLBACK(makegg), entry);
	//单聊窗口
	view = gtk_text_view_new(); 
	gtk_widget_set_size_request (view, 300, 250);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), scrolled, 15, 30);
	gtk_container_add(GTK_CONTAINER(scrolled), view);

	//群聊窗口
	view_group = gtk_text_view_new();
	gtk_widget_set_size_request (view_group, 300, 250);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(view_group), FALSE);
	gtk_fixed_put(GTK_FIXED(fixed), scrolled_group, 320, 30);
	gtk_container_add(GTK_CONTAINER(scrolled_group), view_group); 
			
	// 获取文本缓冲区
	bufferuser=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view));
	bufferuser_group=gtk_text_view_get_buffer(GTK_TEXT_VIEW(view_group));        	
	gtk_widget_show_all(window);// 显示窗口全部控件	
	gtk_main();
}
