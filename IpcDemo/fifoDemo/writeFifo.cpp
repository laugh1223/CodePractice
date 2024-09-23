#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <thread>
#include <stdio.h>

using namespace std;

void mythread()
{
	if(access("myfifo01", F_OK))
		mkfifo("myfifo01", 0666);
    int mfd = open("myfifo01", O_WRONLY);
	char buff[1024] = {0};
	while(fgets(buff, 1024, stdin) != NULL){
		buff[strlen(buff)-1] = '\0';
		write(mfd, buff, strlen(buff));
		if(!strcmp(buff, "-1")){
			cout << "exit" << endl;
			break;
		}
		memset(buff, 0, 1024);
	}
	cout << "close fd" << endl;
	close(mfd);
}


int main(int argc, char **argv)
{
	thread tid01(mythread);

	tid01.join();

	return 0;
}
