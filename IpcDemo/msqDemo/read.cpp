#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/msg.h>
#include<sys/ipc.h>

struct mymesg{
	long int mtype;
	char mtext[512];
};
int main()
{
	int id = 0;
	struct mymesg ckxmsg;
	key_t key = ftok("/tmp",66);
	id = msgget(key,0666|IPC_CREAT);
	printf("id : %d\n",id);
	if(id == -1)
	{
		printf("open msg error \n");
		return 0;
	}
	while(1)
	{
		if(msgrcv(id,(void *)&ckxmsg,512,1,0) < 0)
		{
			printf("receive msg error \n");
			return 0;
		}
		printf("data:%s",ckxmsg.mtext);
		if(strncmp(ckxmsg.mtext,"-1",2) ==0)
			break;
		bzero(ckxmsg.mtext, 512);
	}
	return 0;
}