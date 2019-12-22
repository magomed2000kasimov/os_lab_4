#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <string.h>
#define BUFFER_SIZE 10
//создание временного файла.
int create_tmp() {
	char tmp_name[] = "/tmp/tmpf.XXXXXX";
	int tmp_fd = mkstemp(tmp_name);
	if ( tmp_fd == -1) {
		printf("error\n");
		exit(1);
	}
	int size = BUFFER_SIZE + 1;
	char array[size];
	for ( int i = 0; i < size; ++i ) {
		array[i] = '\0';
	}
	write(tmp_fd, array, size);
	return tmp_fd;
}

//рекурсивное вычисление факториала.
//создаём семафор и заставляем родительский процесс ждать пока дочерний не завершит свои вычисления.
unsigned long long fact(int n, int* map){
    if (n == 0){
        return 1;
    }
    else { 
	const char* out_sem_name = "/o_s";	
	sem_unlink(out_sem_name);
	sem_t* out = sem_open(out_sem_name, O_CREAT, 777, 0);	
        pid_t proc = fork();
        if (proc < 0){
            printf("Error: fork\n");
            exit(1);
        }
         if (proc == 0){ //дочерний процесс
	    unsigned long long res;
            res = fact(n - 1, map);
	    map[0] = res;
            sem_post(out);
	    exit(0);
	    
        }
        if (proc > 0) { //родительский процесс     	
	    sem_wait(out);    
            unsigned long long res;
            res = map[0];
            return n * (res); 
        }    
    }
}



int main(){
	char a[132] = "Instruction.\n Enter only one nonnegative integer number less than 14. In case of incorrect input, the program will simply terminate:";
    for ( int i = 0 ; i < 132 ; ++i ) {
            write(STDOUT_FILENO,&a[i],sizeof(char));
    } //вывод короткой инструкции.
        
        
    int flag = 0,flagPlus = 0,flagTabs = 0,flagNumber = 0;
	int n = 0;
	char c;
        while(true) {
	read(STDIN_FILENO,&c,1);
	if (c <= '9' && c >= '0') {
	flagPlus++;
        flagNumber++;
        n *= 10;
	n += c - '0';
        continue;
	}
	if (c=='\n')
	break;
        if (c == '+' && flagPlus == 0) {
            flagPlus++;
            continue;
       }
       	if ((c == ' ' || c == '\t') && (flagTabs == 0)) {
     		continue;
      	}   
      	else 	
		++flag;
	} 
      	if (flag != 0 || flagNumber == 0) {
       		 return 0;
       	}
	if (n > 13)
			return 0; // парсер.
	unsigned long long k;
	int fd = create_tmp();// дескриптор временного файла.
	//mapping файла.
	int* map = (int*) mmap(NULL,10,PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	if (map == NULL) {
		printf("error mapping\n");
		exit(1);
	}
	k=fact(n,map);
	printf("result %lld\n",k);
	return 0;
}

