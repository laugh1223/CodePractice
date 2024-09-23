#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

using namespace std;

int main()
{
	int fd1[2], fd2[2];
	pipe(fd1);
	pipe(fd2);
	pid_t cpid;
	if((cpid = fork()) == 0){
		close(fd1[0]);
		close(fd2[1]);
		char rbuff[10] = {0};
		char sbuff[10] = "hello";
		write(fd1[1], sbuff, sizeof(sbuff));
		read(fd2[0], rbuff, sizeof(rbuff));
		cout << "C : " << rbuff << endl;

		close(fd1[1]);
		close(fd2[0]);
		exit(0);
	}
	close(fd1[1]);
	close(fd2[0]);
	char rbuff[10] = {0};
	char sbuff[6] = "world";
	write(fd2[1], sbuff,sizeof(sbuff));
	read(fd1[0], rbuff, sizeof(rbuff));
	cout << "F : " << rbuff << endl;

	close(fd1[0]);
	close(fd2[1]);
	wait(NULL);
	return 0;
}
