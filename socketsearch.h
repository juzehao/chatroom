typedef struct node
{
	char s[50];
	int num;
	struct node *next;
}node;
void insert(node *head,char *ss,int num)
	{
		struct node **h=&(head);
		struct node *p = (struct node*)malloc(sizeof(struct node));
		struct node *stu=NULL;
		strcpy(p->s,ss);
		p->num=num;
		p->next=NULL;
		if(*h == NULL)
	    {
	        *h = p;
	    }
	    else
	    {
	        stu = *h;
	        while(stu->next != NULL)
	        {
	            stu = stu->next;
	        }
	        stu->next = p;
	    }
	}
void deleall(node *head)
{
	struct node **h=&head;
	struct node *p1 = NULL;//指向要删除的前一个结点
	struct node *p2 = *h;//指向要删除的结点
	while(p2!=NULL)
	{
		p1=p2;
		p2=p2->next;
		close(p1->num);
		free(p1);
	}
}
void dele(node *head,int n)
	{
		struct node **h=&head;
		struct node *p1 = NULL;//指向要删除的前一个结点
		struct node *p2 = *h;//指向要删除的结点
		while(p2 != NULL)
		{
			if(p1==NULL&&p2->num==n)
			{
				*h = p2->next;
				free(p2);
				break ;
			}
			else if(p2->num==n)
			{
				p1->next = p2->next;
				free(p2);
				break ; 
			}
			p1=p2;
			p2=p2->next;
		}
	}	
	int find(node *head,char *ss)
	{
		struct node *h=head;
		while(h!=NULL)
		{
			if(strcmp(ss,h->s)==0)
			{
				return h->num;
			}
		h=h->next;
		}
		return -1;
	}

