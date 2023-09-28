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
#include "mymsg.h"
static GtkWidget *entry1; 
static GtkWidget *entry2,*entry3;
const gchar *username;
const gchar *password,*repeatpassword;
GtkWidget *window;
GtkWidget *home;
GtkWidget *vbox;
GtkWidget *hbox1,*hbox2,*hbox3,*hbox4;
GtkWidget *button;
GtkWidget *label;

void * read_thread(void *p){
int comm_sock=*(int*)p;
//char buffer[NUM];
responsemeg rmsg;
int rmsglen;
rmsglen=sizeof(responsemeg);
while(1){
memset(&rmsg,'\0',rmsglen);
read(comm_sock,&rmsg,rmsglen);
printf("%d",rmsg.command);
if(rmsg.command==1)
{
	int childpid;  
	int i;  
	    char * execv_str[] = {username, "executed by execv",NULL};  
	    execv("./chat",execv_str);	
}
else
{

}
break;
}
}
void Log(int comm_sock){
int msglen;
msg sendmsg;
msglen=sizeof(msg);
memset(&sendmsg,'\0',msglen);
sendmsg.command=2;
	sendmsg.messlen=2;
	strncpy(sendmsg.mess[0],username,strlen(username));
	strncpy(sendmsg.mess[1],password,strlen(password));		
	write(comm_sock,&sendmsg,msglen);
}
void on_log_clicked(GtkButton* button,gpointer data){//登录按钮 
	if(strcmp(password,repeatpassword)!=0)
	{
		GtkWidget *dialog;


  dialog = gtk_message_dialog_new(window,


            GTK_DIALOG_DESTROY_WITH_PARENT,


            GTK_MESSAGE_INFO,


            GTK_BUTTONS_OK,


            "need same password", "title");


  gtk_window_set_title(GTK_WINDOW(dialog), "Information");


  gtk_dialog_run(GTK_DIALOG(dialog));


  gtk_widget_destroy(dialog);
		return ;
	}
	struct sockaddr_in  ser_addr;
	int comm_sock;
	int len;
	char buffer[NUM];
	pthread_t tid;
	void *r;
	int msglen;
	msg sendmsg;
	comm_sock=socket(AF_INET,SOCK_STREAM,0);
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
	ser_addr.sin_port=9765;
	len=sizeof(ser_addr);
	//len=sizeof(ser_addr);
	//printf();
	connect(comm_sock,(struct sockaddr*)&ser_addr,len);
	Log(comm_sock);
	pthread_create(&tid,0,read_thread,(void*)(&comm_sock));
	pthread_join(tid,&r);

}

void on_register_clicked(GtkButton* button,gpointer data){//back按钮
	execv("./log",NULL);
}


void entry_callback1(GtkWidget *widget, gpointer entry)//获取内容
{

        //获取文本内容
        username = gtk_entry_get_text(GTK_ENTRY(entry));

        //printf("uesrname:%s\n", username);

        return;

}

void entry_callback2(GtkWidget *widget, gpointer entry)//获取内容
{

        //获取文本内容
        password = gtk_entry_get_text(GTK_ENTRY(entry));

        //printf("password:%s\n", password);

        return;

}
void entry_callback3(GtkWidget *widget, gpointer entry)//获取内容
{

        //获取文本内容
        repeatpassword = gtk_entry_get_text(GTK_ENTRY(entry));

        //printf("password:%s\n", password);

        return;

}

//登陆界面 
void login(int argc,char *argv[]){
		//初始化 
		gtk_init(&argc,&argv);
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);  //主窗口
        g_signal_connect(G_OBJECT(window),"delete_event",
        G_CALLBACK(gtk_main_quit),NULL);
        gtk_window_set_title(GTK_WINDOW(window),"124聊天室");  //主窗口标签栏
        gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
        gtk_container_set_border_width(GTK_CONTAINER(window),10);

        vbox = gtk_vbox_new(FALSE,0);
        gtk_container_add(GTK_CONTAINER(window),vbox);


        hbox1 = gtk_hbox_new(FALSE,0);                  //定义hbox1容纳 账号 账号输入栏
        gtk_box_pack_start(GTK_BOX(vbox),hbox1,FALSE,FALSE,5);

        label = gtk_label_new("账号");
        gtk_box_pack_start(GTK_BOX(hbox1),label,TRUE,TRUE,5);

        entry1 = gtk_entry_new();
        gtk_widget_set_direction(entry1,GTK_TEXT_DIR_RTL);
        gtk_box_pack_start(GTK_BOX(hbox1),entry1,FALSE,FALSE,5);

        //获取文本内容
        g_signal_connect(entry1, "changed", G_CALLBACK(entry_callback1), entry1);
        
        
        hbox2 = gtk_hbox_new(FALSE,0);          //定义hbox2容纳密码 密码输入栏
        gtk_box_pack_start(GTK_BOX(vbox),hbox2,FALSE,FALSE,5);

        label = gtk_label_new("密码");
        gtk_box_pack_start(GTK_BOX(hbox2),label,TRUE,TRUE,5);

        entry2 = gtk_entry_new();
        gtk_widget_set_direction(entry2,GTK_TEXT_DIR_RTL);
        gtk_box_pack_start(GTK_BOX(hbox2),entry2,FALSE,FALSE,5);
        gtk_entry_set_visibility(GTK_ENTRY(entry2),FALSE);
        g_signal_connect(entry2, "changed", G_CALLBACK(entry_callback2), entry2);
	
	 hbox3 = gtk_hbox_new(FALSE,0);          //定义hbox2容纳密码 密码输入栏
        gtk_box_pack_start(GTK_BOX(vbox),hbox3,FALSE,FALSE,5);

        label = gtk_label_new("密码");
        gtk_box_pack_start(GTK_BOX(hbox3),label,TRUE,TRUE,5);

        entry3 = gtk_entry_new();
        gtk_widget_set_direction(entry3,GTK_TEXT_DIR_RTL);
        gtk_box_pack_start(GTK_BOX(hbox3),entry3,FALSE,FALSE,5);
        gtk_entry_set_visibility(GTK_ENTRY(entry3),FALSE);
        g_signal_connect(entry3, "changed", G_CALLBACK(entry_callback3), entry3);


        hbox4 = gtk_hbox_new(FALSE,0);          //定义hbox4容纳 登录按钮
        gtk_box_pack_start(GTK_BOX(vbox),hbox4,FALSE,FALSE,5);

        button = gtk_button_new_with_label("注册");             //按键"注册"
        g_signal_connect(G_OBJECT(button),"clicked",
        G_CALLBACK(on_log_clicked),NULL);

        gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);

        button = gtk_button_new_with_label("back");             //按键"登录"
        g_signal_connect(G_OBJECT(button),"clicked",
        G_CALLBACK(on_register_clicked),NULL);
        gtk_box_pack_start(GTK_BOX(hbox4),button,TRUE,TRUE,5);


        gtk_widget_show_all(window);
        gtk_main();
}

//主界面 
void homepage(int argc,char *argv[]){
	
}


int main ( int argc , char* argv[])
{
        printf("123");
        login(argc,argv);
		homepage(argc,argv);
        return FALSE;
}

