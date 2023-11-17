#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>

typedef struct student
{
    char nume[256];
    int grupa;
} student;

void generateStudentList(student studenti[6][16]) {
    char buff_nume[256];
    for(unsigned int i = 0; i < 6; ++i) {
        for(unsigned int j = 0; j < 16; ++j) {
            sprintf(buff_nume, "NumeStudent%d Grupa%d", j, i);
            strcpy(studenti[i][j].nume, buff_nume);
            studenti[i][j].grupa = j;
        }
    }
}

void printStudentListFromGroup(student studenti[6][16], unsigned int group) {
    printf("Lista studentilor din grupa %d este:\n", group);
    for(unsigned int j = 0; j < 16; ++j) {
            printf("%d. %s\n", j, studenti[group][j].nume);
    }
}

int main(int argc, char **argv) {

    // input verification

    if(argc != 2) {
        printf("Error! Program should look like: Usage ./program <director_intrare>!\n");
        exit(-1);
    }

    student studenti[6][16];
    generateStudentList(studenti);
    printStudentListFromGroup(studenti, atoi(argv[1]));
    
    return 0;
}