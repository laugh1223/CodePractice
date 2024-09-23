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
	id = msgget(key,IPC_CREAT | 0666);
	printf("id : %d\n",id);
	if(id == -1)
	{
		printf("create msg error \n");
		return 0;
	}
	while(1)
	{
		char msg[512];
		memset(msg,0,sizeof(msg));
		ckxmsg.mtype = 1;
		printf("input message:");
		fgets(msg,sizeof(msg),stdin);
		strcpy(ckxmsg.mtext,msg);

		if(msgsnd(id,(void *)&ckxmsg,512,0) < 0)
		{
			printf("send msg error \n");
			return 0;
		}

		if(strncmp(msg,"-1",2) == 0)
			break;
	}
	if(msgctl(id,IPC_RMID,NULL) < 0)
	{
		printf("del msg error \n");
		return 0;
	}
	return 0;
}