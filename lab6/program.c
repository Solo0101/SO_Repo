#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFF_SIZE 256

void generateStats(int *fin, int *fout, char givenCharacter) {
    char finContent[BUFF_SIZE+1];

    unsigned int countSmallLetters = 0;
    unsigned int countCapsLetters = 0;
    unsigned int countDigits = 0;
    unsigned int countCh = 0;
    unsigned int fileSize = 0;

    struct stat fileStats;
    if(fstat(*fin, &fileStats) == 0) {
        fileSize = fileStats.st_size;
    } else {
        printf("Error in getting file stats!\n");
    }

    read(*fin, &finContent, BUFF_SIZE);  

    for(unsigned int i = 0; i < BUFF_SIZE; ++i) {
        if(finContent[i] >= 'A' && finContent[i] <= 'Z') {
            countCapsLetters++;
        } else if (finContent[i] >= 'a' && finContent[i] <= 'z') {
            countSmallLetters++;
        } else if(finContent[i] >= '0' && finContent[i] <= '9') {
            countDigits++;
        }
        if(finContent[i] == givenCharacter) {
            countCh++;
        }

    }
    char foutContent[BUFF_SIZE+1];
    sprintf(foutContent, "numar litere mici: %d\nnumar litere mari: %d\nnumar cifre: %d\nnumar aparitii caracter: %d\ndimensiune fisier: %d%c", 
                                countSmallLetters, countCapsLetters, countDigits, countCh, fileSize, '\0');
    write(*fout, &foutContent, BUFF_SIZE+1); 
}

int main(int argc, char **argv) {
    // input verification
    if(argc != 4) {
        printf("Error! Program should look like: './program <path/to/fisier_intrare> <path/to/fisier_statistica> <ch>'!\n");
        exit(-1);
    }
    int fin, fout;
    char givenCharacter;
    if(strlen(argv[3]) != 1){
        printf("Error! Invalid character input!\n");
    } else {
        givenCharacter = argv[3][0];
    }
    // opening files verification
    if((fin = open(argv[1], O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH)) == -1) {
        printf("Error! Could not open file %s !\n", argv[1]);
        printf("Error code: %d\n", fin);
        exit(-1);
    }
    if((fout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH)) == -1) {
        printf("Error! Could not open file %s !\n", argv[2]);
        printf("Error code: %d\n", fout);
        exit(-1);
    }

    // solving:

    generateStats(&fin, &fout, givenCharacter);

    // closing files verification
    if(close(fin) != 0) {
        printf("Error! Could not save and close file %s !\n", argv[1]);
        exit(-1); 
    }
    if(close(fout) != 0) {
        printf("Error! Could not save and close file %s !\n", argv[2]);
        exit(-1); 
    }

    return 0;
}

//pwd: /mnt/d/ProiecteTeme/UPT_CURSURI/Anul_3/SO/SO_Repo/lab6/
//./program '/mnt/d/ProiecteTeme/UPT_CURSURI/Anul_3/SO/SO_Repo/lab6/input.txt' '/mnt/d/ProiecteTeme/UPT_CURSURI/Anul_3/SO/SO_Repo/lab6/output.txt' ';'