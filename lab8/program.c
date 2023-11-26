#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <ctype.h>
#include <dirent.h>

int main(void)
{
	int pfd[2];
	int pid;

	// ...
	if(pipe(pfd)<0) {
	  perror("Eroare la crearea pipe-ului\n");
	  exit(1);
	}
	// ...
	if((pid=fork())<0) {
	  perror("Eroare la fork\n");
	  exit(1);
	}
	if(pid==0) {
          /* procesul fiu */
	  close(pfd[0]); /* inchide capatul de citire; */
		         /* procesul va scrie in pipe  */
	//   ...
	  write(pfd[1],buff,len); /* operatie de scriere in pipe */
	//   ...
	  close(pfd[1]); /* la sfarsit inchide si capatul utilizat */
	  exit(0);
	}
	/* procesul parinte */	
	close(pfd[1]); /* inchide capatul de scriere; */
	      	       /* procesul va citi din pipe  */
	// ...
	read(pfd[0],buff,len); /* operatie de citire din pipe */
	// ...
	close(pfd[0]); /* la sfarsit inchide si capatul utilizat */

	
	return 0;
}