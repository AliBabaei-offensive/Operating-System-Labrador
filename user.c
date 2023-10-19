#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


void writeMode(){
     

    char deviceName[20];
    char transactionType[2];
    char sender[2],reciver[2],amount[10];
    char path[100] = "/dev/";

    printf("Please enter your Device name:");
    scanf("%s",deviceName);
    printf("Please enter transaction type(t:Transfer/d:Deposit/w:Withdra):");
    scanf("%s",&transactionType);
    printf("Please enter sender:");
    scanf("%s",sender);
    printf("Please enter reciver:");
    scanf("%s",reciver);
    printf("Please enter amount:");
    scanf("%s",amount);
    //printf("%s",deviceName);
    //printf("device name:%s\nTransaction Type:%s\nSender:%s\nReciver:%s\nAmount:%s",deviceName,transactionType,sender,reciver,amount);

    char * format[20];
    
    strcat(path,deviceName);//making path with device name
    sprintf(format,"%s,%s,%s,%s",transactionType,sender,reciver,amount);//making sending format
    printf("%s\n",format);

    //starting writing
	char Buffer[128]={0};
	int fd=open(path, O_RDWR);
	sprintf(Buffer,"%s",format);
	write(fd,Buffer,strlen(Buffer));
	//close(fd);
    //ending writing
    //start Reading
    char readBuffer[1024]={0};
	//fd=open(path, O_RDONLY);
	read(fd,readBuffer, 128);
	fprintf(stdout,"%s\n",readBuffer);
	close(fd);
    //ending reading 
}

void readMode(){
    char readBuffer[1024]={0};
	int fd=open("/dev/bank_device", O_RDONLY);
	read(fd,readBuffer, 128);
	fprintf(stdout,"%s\n",readBuffer);
	close(fd);
}

int main(int argc, char *argv[])
//int main()
{
    if (strcmp(argv[1],"read") == 0) readMode();
    else if (strcmp(argv[1],"write") == 0) writeMode();
    else if (argv[1] == NULL) printf("Pleas enter a correct mode");
    else printf("Please enter correct mode");
   
    
	return 0;
}


