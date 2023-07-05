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
#define S1 SIGINT        //2
#define S2 SIGUSR1  //10
#define S3 SIGUSR2 //12
#define S4 SIGCONT  //18
int PID[3];
int des[2];
int stop;
struct sembuf buf;
int semid; //semafor
int shmid; //pamiec dzielona
FILE *pid;
key_t key1, key2;			// zmienne na klucze do semaforow
void first_handler(int signal)
{
	printf("\n%d: Konczenie pracy programu. Otrzymalem sygnal %s\n",getpid(),sys_siglist[signal]);//obsluga sygnalu konczenie pracy
	semctl (semid, 0, IPC_RMID);
	semctl (semid, 1, IPC_RMID);
	semctl (semid, 2, IPC_RMID);
	shmctl (shmid, IPC_RMID, NULL);
	kill(PID[0],SIGKILL);
	kill(PID[1],SIGKILL);
	kill(PID[2],SIGKILL);
	kill(getpid(),SIGKILL);
	return;
}
void second_handler(int signal)
{
	printf("\n[%d]: Otrzymalem sygnal %d %s\n",getpid(),signal,sys_siglist[signal]);//obsluga synalu wstrzymanie 10 SIGUSR1
	if(signal==2)
	{
		kill(getppid(),S1);
		kill(getpid(),SIGKILL);
	}
	return;
}
void third_handler(int signal)
{
	//obsluga sygnalu wznowienie
	printf("\n[%d]: WZNOWIENIE! Otrzymalem sygnal %d %s\n",getpid(),signal,sys_siglist[signal]);//obsluga synalu wznowienie 12 SIGUSR2
	//stop=0;
	if(stop==1)
	{
		if(getpid()==PID[0])
{
	kill(PID[1],S3);
	kill(PID[2],S3);
}
	else if(getpid()==PID[1])
	{
	kill(PID[0],S3);
	kill(PID[2],S3);
}
	else if(getpid()==PID[2])
{
	kill(PID[0],S3);
	kill(PID[1],S3);
}
}
	stop=0;
	//stop=0;
	
	return;
}
void fourth_handler(int signal)
{
	printf("\n[%d]: WSTRZYMANIE! Otrzymalem sygnal %d %s\n",getpid(),signal,sys_siglist[signal]);//obsluga synalu wstrzymanie
//obsluga synalu wstrzymanie
	if(getpid()==PID[0])
{
	kill(PID[1],S4);
	kill(PID[2],S4);
}
	else if(getpid()==PID[1])
	{
	kill(PID[0],S4);
	kill(PID[2],S4);
}
	else if(getpid()==PID[2])
{
	kill(PID[0],S4);
	kill(PID[1],S4);
}
	stop=1;
	while(stop==1)
	{
		sleep(1);
		printf("Przerwa\n");
		sleep(6);
	}
return;
}
void fifth_handler(int signal)
{
	printf("\n[%d]: WSTRZYMANIE! Otrzymalem sygnal %d %s\n",getpid(),signal,sys_siglist[signal]);//obsluga synalu wznowienie 12 SIGUSR2
	//stop=0;
	stop=1;
	while(stop==1)
	{
		sleep(1);
		printf("Przerwa\n");
		sleep(6);
	}
}
void podnies(int semid, int semnum)//podniesienie semafora powoduje wznowienie pracy
{ 
	buf.sem_num = semnum; 
	buf.sem_op = 1; 
	buf.sem_flg = 0; 
	int r;
	do {
        r = semop(semid, &buf, 1);

    }while(r < 0 && errno == EINTR);
    if (r < 0 && errno != -1){
        perror("Opuszczanie semafora");
        exit(EXIT_FAILURE);
    }
} 


void opusc(int semid, int semnum)//opuszczenie semafora powoduje wstrzymanie pracy
{ 
	buf.sem_num = semnum; 
	buf.sem_op = -1; 
	buf.sem_flg = 0; 
	int r;
	do {
        r = semop(semid, &buf, 1);

    }while(r < 0 && errno == EINTR);
    if (r < 0 && errno != -1){
        perror("Opuszczanie semafora");
        exit(EXIT_FAILURE);
    }
}
int main(void)
{
sigset_t maska;	
pipe(des);	
char *buf;
//if(fork())
//{	
if ((key1 = ftok(".", 'F')) == -1) 
       errx(1, "Blad tworzenia klucza!");//klucz dla semaforow
       if((semid = semget(key1, 3, IPC_CREAT|0600))==-1)//tablica 3 semaforow
	errx(1,"Blad tworzenia semaforow!");
	//nadawanie wartosci semaforom
	
	if (semctl(semid, 0, SETVAL, (int)1) == -1){//semafor p1
 		perror("Nadanie wartosci semaforowi 0");
		exit(1);
	}
 	if (semctl(semid, 1, SETVAL, (int)0) == -1){
		perror("Nadanie wartosci semaforowi 1");//semafor p2
		exit(1);
 	}
 	if (semctl(semid, 2, SETVAL, (int)0) == -1){
		perror("Nadanie wartosci semaforowi 1");//semafor p2
		exit(1);
 	}
if ((key2 = ftok(".", 'G')) == -1) 
        errx(1, "Blad tworzenia klucza!");//klucz dla pamieci wspoldzielonej
        
    shmid = shmget(key2, sizeof(char), IPC_CREAT|0600);//deklaracja pamieci wspoldzielonej
    if (shmid == -1){
		perror("Utworzenie segmentu pamieci wspoldzielonej");
		exit(1);
	}
	//Pobieramy wskaznik na adres pamieci wspoldzielonej
	buf = shmat(shmid, NULL, 0);

	if (buf == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
	}
if(fork()==0)
{
	//P1
	PID[0]=getpid();
	pid=fopen("pidy1","w+");
	fprintf(pid,"%d\n",getpid());
	fclose(pid);
	printf("[%d]: jestem proces P1 \n",getpid());
	sigfillset(&maska);//wypelnij maske
	sigdelset(&maska,S1);//usun sygnal SIGINT z maski
	sigdelset(&maska,S2);
	sigdelset(&maska,S3);
	sigdelset(&maska,S4);
	sigprocmask(SIG_SETMASK, &maska, NULL);//ustaw maske
	signal(S1,second_handler);
	signal(S2,fourth_handler);
	signal(S3,third_handler);
	signal(S4,fifth_handler);
	char buff[48];
	char hex[97];
	sleep(1);
	pid=fopen("pidy1","r");
	fscanf(pid,"%d",&PID[0]);
	fclose(pid);
	pid=fopen("pidy2","r");
	fscanf(pid,"%d",&PID[1]);
	fclose(pid);
	pid=fopen("pidy3","r");
	fscanf(pid,"%d",&PID[2]);
	fclose(pid);
	FILE* plik1;
	int i;
	int wybor;
	int ile=0;//zmienna przechowujaca ilosc znakow w slowie
while (1)
{
	opusc(semid,0);
	//close(des[0]);
	sleep(1);
		wybor=0;
	while(wybor!=1&&wybor!=2&&wybor!=3)
	{

	sleep(1);
	printf(" 1: Wprowadzenie danych z klawiatury\n 2: Wprowadzenie danych z pliku\n 3: Wprowadzenie danych z /dev/random\n");
	printf("Podaj wybor: ");
	scanf("%d",&wybor);
	if(wybor!=1&&wybor!=2&&wybor!=3)
	printf("Niepoprawny wybor!\n");
	getchar();
	}
	strcpy(buff,"");
	strcpy(hex,"");
	switch(wybor)
	{
case 1:
printf("[%d] Podaj wiadomosc: \n",getpid());
sleep(1);
fgets(buff,sizeof(buff),stdin);

break;
case 2:
plik1=fopen("dane.txt","r");
fgets(buff,sizeof(buff),plik1);
fclose(plik1);
break;
case 3:
plik1=fopen("/dev/urandom","r");
fgets(buff,sizeof(buff),plik1);
fclose(plik1);
break;
default: printf("Niepoprawny wybor!\n");
sleep(2);	
break;
	}
	printf("[%d] odebralem: %s\n",getpid(), buff);
	fflush(stdout);
ile=strlen(buff);
buff[ile]='\0';
if(buff[ile-1]=='\n')
        buff[--ile] = '\0';

    for(i = 0; i<ile; i++){
        sprintf(hex+i*2, "%02X", buff[i]);
    }
    printf("[%d] wysylam k1: %s\n",getpid(), hex);
    ile=strlen(hex);
    //fflush(stdout);
    close(des[0]);
	write(des[1],&ile,sizeof(int));//przesylamy ile znakow bedzie w stringu
	write(des[1],hex,ile*sizeof(char));//przesylamy string

	//nieskonczona petla
	sleep(1);
	podnies(semid,1);
}}
 if(fork()==0)
{char *buf;
	
	pid=fopen("pidy2","w+");
	fprintf(pid,"%d\n",getpid());
	fclose(pid);
	sigfillset(&maska);//wypelnij maske
	sigdelset(&maska,S1);//usun sygnal SIGINT z maski
	sigdelset(&maska,S2);
	sigdelset(&maska,S3);
	sigdelset(&maska,S4);
	sigprocmask(SIG_SETMASK, &maska, NULL);//ustaw maske
	signal(S2,fourth_handler);
	signal(S3,third_handler);
	signal(S1,second_handler);
	signal(S4,fifth_handler);
	int liczba;//P2 Pipe
	char w[97];
	sleep(1);
	pid=fopen("pidy1","r");
	fscanf(pid,"%d",&PID[0]);
	fclose(pid);
	pid=fopen("pidy2","r");
	fscanf(pid,"%d",&PID[1]);
	fclose(pid);
	pid=fopen("pidy3","r");
	fscanf(pid,"%d",&PID[2]);
	fclose(pid);
	//char *buf;
	printf("[%d]: jestem proces P2 \n",getpid());//Generuje dane
	buf = shmat(shmid, NULL, 0);

	if (buf == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
	}
	//int l=0;
	while (1)
{
	sleep(1);
	opusc(semid,1);
	close(des[1]);
	liczba=0;
	strcpy(w, "");
	read(des[0],&liczba,sizeof(int));
	read(des[0],&w,liczba);
	if(liczba!=0)
	{
	w[liczba]='\0';
	printf("[%d] odebrana wiadomosc: %s\n",getpid(),w);
	sleep(1);
	strcpy(buf,w);
	//l=strlen(buf);
	sleep(1);
	printf("[%d] wypelniam k2: %s\n",getpid(),buf);
	//printf("bufor: %d\n",l);
	sleep(1);
	
	podnies(semid,2);
	//close(des[0]);
}
}//nieskonczona petla
	return 0;
}
 if(fork()==0)
{
	
	PID[2]=getpid();
	pid=fopen("pidy3","w+");
	fprintf(pid,"%d\n",getpid());
	fclose(pid);
	char *buf;
	//char *buf2;
	char wynik[97];
//	int i=0;
	int length;
	sigfillset(&maska);//wypelnij maske
	sigdelset(&maska,S1);//usun sygnal SIGINT z maski
	sigdelset(&maska,S2);
	sigdelset(&maska,S3);
	sigdelset(&maska,S4);
	sigprocmask(SIG_SETMASK, &maska, NULL);//ustaw maske
	signal(S1,second_handler);
	signal(S2,fourth_handler);
	signal(S3,third_handler);
	signal(S4,fifth_handler);
	//P3
	sleep(1);
	pid=fopen("pidy1","r");
	fscanf(pid,"%d",&PID[0]);
	fclose(pid);
	pid=fopen("pidy2","r");
	fscanf(pid,"%d",&PID[1]);
	fclose(pid);
	pid=fopen("pidy3","r");
	fscanf(pid,"%d",&PID[2]);
	fclose(pid);
	printf("[%d]: jestem proces P3\n",getpid());//K2 SHMEM
	//Pobieramy wskaznik na adres pamieci wspoldzielonej
	buf = shmat(shmid, NULL, 0);

	if (buf == NULL){
		perror("Przylaczenie segmentu pamieci wspoldzielonej");
		exit(1);
	}
	//fflush(stdout);
	int y=0;
while (1)
{
	
	opusc(semid,2);
	printf("[%d] kosnumuje k2: \n",getpid());
	strcpy(wynik, "");
	strcpy(wynik,buf);
	length=strlen(wynik);
	wynik[length]='\0';
	sleep(1);
	sleep(1);
	for(y=0;y<length;y++)
	{
		printf("%c ",wynik[y]);
		sleep(0.6);
		if((y%15)==14)
		{
			printf("\n");
		}
		
	}
	printf("\n");
	sleep(1);
	podnies(semid,0);
	//nieskonczona petla
}
	return 0;
}
sigfillset(&maska);//wypelnij maske
sigdelset(&maska,SIGINT);//usun sygnal SIGINT z maski
sigprocmask(SIG_SETMASK, &maska, NULL);//ustaw maske
signal(S1,first_handler);
sleep(2);
pid=fopen("pidy1","r");
fscanf(pid,"%d",&PID[0]);
fclose(pid);
pid=fopen("pidy2","r");
fscanf(pid,"%d",&PID[1]);
fclose(pid);
pid=fopen("pidy3","r");
fscanf(pid,"%d",&PID[2]);
fclose(pid);

while(1){

	};

return 0;
}
