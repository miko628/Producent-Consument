#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <signal.h>
#include <sys/shm.h>
#include <string.h>
#include <errno.h>
#include <err.h>
int main(void)
{
	int PID[3];
	int wybor,w;
	FILE* pid;
	pid=fopen("pidy1","r");
fscanf(pid,"%d",&PID[0]);
fclose(pid);
pid=fopen("pidy2","r");
fscanf(pid,"%d",&PID[1]);
fclose(pid);
pid=fopen("pidy3","r");
fscanf(pid,"%d",&PID[2]);
fclose(pid);
while(1)
{
	printf("1: Wyslanie sygnalu do P1\n2: Wyslanie sygnalu do P2\n3: Wyslanie sygnalu do P3\n");
	scanf("%d",&wybor);
	switch(wybor)
	{
		case 1:
		printf("1: Sygnal konca P1\n2: Sygnal wstrzymania P1\n3: Sygnal wznowienia P1\n");
		scanf("%d",&w);
		switch(w)
	{
		case 1:
		kill(PID[0],SIGINT);
		break;
		case 2:
		kill(PID[0],SIGUSR1);	
		break;
		case 3:
		kill(PID[0],SIGUSR2);
		break;
	}
		break;
		case 2:
		printf("1: Sygnal konca P2\n2: Sygnal wstrzymania P2\n3: Sygnal wznowienia P2\n");
		scanf("%d",&w);
		switch(w)
	{
		case 1:
		kill(PID[1],SIGINT);
		break;
		case 2:
		kill(PID[1],SIGUSR1);	
		break;
		case 3:
		kill(PID[1],SIGUSR2);
		break;
	}
		break;
		case 3:
		printf("1: Sygnal konca P3\n2: Sygnal wstrzymania P3\n3: Sygnal wznowienia P3\n");
		scanf("%d",&w);
		switch(w)
	{
		case 1:
		kill(PID[2],SIGINT);
		break;
		case 2:
		kill(PID[2],SIGUSR1);	
		break;
		case 3:
		kill(PID[2],SIGUSR2);
		break;
	}
		break;
	}
	switch(w)
	{
		case 1:
		kill(PID[0],SIGINT);
		break;
		case 2:

		break;
		case 3:

		break;
	}
	
}
return 0;
}
