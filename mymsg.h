typedef struct Msg{
int command;
char from[12];
char  to[12];
char date[20];
unsigned messlen;
char mess[10][128];
}msg;
typedef struct Responsemsg{
	char from[12];
	char to[12];
	int command;
	int len;
	char mess[20][128];
}responsemeg;

