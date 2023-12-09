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

#define BUFF_SIZE 4096
#define OUT_FILE_SUFFIX "statistica.txt"
typedef struct rights {
    char user_rights[4];
    char group_rights[4];
    char other_rights[4]; 
} rights;

void getRigths(struct stat fileStats, char* user, char* group, char* other)
{
    strcat(user, (fileStats.st_mode & S_IRUSR) ? "R" : "-");
    strcat(user, (fileStats.st_mode & S_IWUSR) ? "W" : "-");
    strcat(user, (fileStats.st_mode & S_IXUSR) ? "X" : "-");

    strcat(group, (fileStats.st_mode & S_IRGRP) ? "R" : "-");
    strcat(group, (fileStats.st_mode & S_IWGRP) ? "W" : "-");
    strcat(group, (fileStats.st_mode & S_IXGRP) ? "X" : "-");

    strcat(other, (fileStats.st_mode & S_IROTH) ? "R" : "-");
    strcat(other, (fileStats.st_mode & S_IWOTH) ? "W" : "-");
    strcat(other, (fileStats.st_mode & S_IXOTH) ? "X" : "-");
}

void convertRGBtoGrayscaleBMP(int fin) {
    if(lseek(fin, 54, SEEK_SET) == -1) {
        perror("Error setting cursor!\n");
        close(fin);
        exit(EXIT_FAILURE);
    }
    unsigned char pixel[3];
    while(read(fin, pixel, 3) == 3) {
        unsigned char grayscale_pixel = (unsigned char)(0.299 * pixel[2] + 0.587 * pixel[1] + 0.114 * pixel[0]);
        lseek(fin, -3, SEEK_CUR);
        write(fin, &grayscale_pixel, 1);
        write(fin, &grayscale_pixel, 1);
        write(fin, &grayscale_pixel, 1);
        lseek(fin, 1, SEEK_CUR);
    }    
}

void processBMP(int fin, struct dirent* entry, char *date, struct stat fileStats, char *foutContent, rights file_rights) {
    unsigned int h = 0, w = 0;   
    if(lseek(fin, 18, SEEK_SET) == -1) {
        perror("Error setting cursor!\n");
        close(fin);
        exit(EXIT_FAILURE);
    }
    read(fin, &w, sizeof(int));
    read(fin, &h, sizeof(int));
    
    fflush(NULL);

    sprintf(foutContent, "nume fisier: %s\n"
                         "inaltime: %d\n"
                         "lungime: %d\n"
                         "dimensiune: %ld By\n"
                         "identificatorul utilizatorului: %d\n"
                         "timpul ultimei modificari: %s\n"
                         "contorul de legaturi: %ld\n"
                         "drepturi de acces user: %s\n"
                         "drepturi de acces grup: %s\n"
                         "drepturi de acces altii: %s\n\n", 
            entry->d_name, h, w, 
            fileStats.st_size, fileStats.st_uid, date, fileStats.st_nlink, 
            file_rights.user_rights, file_rights.group_rights, file_rights.other_rights);
}

void processFile(struct dirent* entry, char *date, struct stat fileStats, char *foutContent, rights file_rights) {
    sprintf(foutContent, "nume fisier: %s\n"
                         "dimensiune: %ld By\n"
                         "identificatorul utilizatorului: %d\n"
                         "timpul ultimei modificari: %s\n"
                         "contorul de legaturi: %ld\n"
                         "drepturi de acces user: %s\n"
                         "drepturi de acces grup: %s\n"
                         "drepturi de acces altii: %s\n\n", 
            entry->d_name, fileStats.st_size, fileStats.st_uid, date, 
            fileStats.st_nlink, 
            file_rights.user_rights, file_rights.group_rights, file_rights.other_rights);
}

void processSymbolicLink(struct dirent* entry, char *path_to_entry, struct stat fileStats, char *foutContent, rights file_rights) {
    struct stat target_fileStats;
    if(stat(path_to_entry, &target_fileStats) == -1) {
        perror("Error getting target file information for symbolic link!\n");
        exit(EXIT_FAILURE);
    }
    sprintf(foutContent, "nume legatura: %s\n"
                         "dimensiune legatura: %ld\n"
                         "dimensiune fisier: %ld\n"
                         "drepturi de acces user: %s\n"
                         "drepturi de acces grup: %s\n"
                         "drepturi de acces altii: %s\n\n",
            entry->d_name, fileStats.st_size, target_fileStats.st_size, file_rights.user_rights, file_rights.group_rights, file_rights.other_rights);
}

void processDirectory(struct dirent* entry, struct stat fileStats, char *foutContent, rights file_rights) {
    sprintf(foutContent, "nume director: %s\n"
                          "identificatorul utilizatorului: %d\n"
                          "drepturi de acces user: %s\n"
                          "drepturi de acces grup: %s\n"
                          "drepturi de acces altii: %s\n\n",
            entry->d_name, fileStats.st_uid, file_rights.user_rights, file_rights.group_rights, file_rights.other_rights);
}

void closeFileEndProcess(char *foutContent, char *diroutpath, pid_t pid, struct dirent *entry, int lines_written) {
    if(pid == 0) {
        if(strcmp(foutContent, "") != 0) {
            // creating & opening output file
            char out_file_path[BUFF_SIZE];
            sprintf(out_file_path, "%s/%s_%s", diroutpath, entry->d_name, OUT_FILE_SUFFIX);
            int fout = open(out_file_path, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fout == -1) {
                perror("Error opening output file!\n");
                exit(EXIT_FAILURE);
            }
            if (write(fout, foutContent, strlen(foutContent)) == -1) {
                perror("Error writing to output file!");
                // closing files verification
                if(close(fout) != 0) {
                    perror("Error! Could not save and close file!\n");
                    exit(EXIT_FAILURE); 
                }
                exit(EXIT_FAILURE);
            }
        }
        // exit(EXIT_SUCCESS);
        exit(lines_written);
    } else{
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)) {
            printf("S-a încheiat procesul cu pid-ul %d și codul %d (pentru fisierul %s)\n", pid, WEXITSTATUS(status), entry->d_name);
        } else {
            printf("Procesul cu pid-ul %d nu s-a încheiat normal, cu codul %d! (pentru fisierul %s)\n", pid, WEXITSTATUS(status), entry->d_name);
        }
    }

}

void stopProcess2(pid_t pid2, struct dirent *entry, int *total_regex_lines) {
    if(pid2 != 0) {
        int status2;
        waitpid(pid2, &status2, 0);
        if(WIFEXITED(status2)) {
            *total_regex_lines += WEXITSTATUS(status2);
            printf("S-a încheiat procesul cu pid2-ul %d și codul %d (pentru fisierul %s)\n", pid2, WEXITSTATUS(status2), entry->d_name);
        } else {
            printf("Procesul cu pid-ul %d nu s-a încheiat normal!\n", pid2);
        }
    }
}

void generateStats(DIR* directory, DIR* directory_out, char *dirpath, char *diroutpath, char character) {
    //data declaration
    char foutContent[BUFF_SIZE+1];
    char path_to_entry[BUFF_SIZE+1];
    char date[20];
    int fin = 0;
    int pfd[2];
    int lines_written = 0;
    int total_regex_lines = 0;
    rights file_rights;
    struct dirent* entry = NULL;
    struct stat fileStats;
    pid_t pid;
    pid_t pid2;
    
    // cycling through the input directory
    while((entry = readdir(directory)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, dirpath) == 0) {
            continue;
        }
        strcpy(foutContent, "");
        snprintf(path_to_entry, sizeof(path_to_entry), "%s/%s", dirpath, entry->d_name);

        // trying to get file stats
        if(lstat(path_to_entry, &fileStats) != 0) {
            printf("Error: Can not get file stats!\n");
            exit(EXIT_FAILURE);
        }

        // creating pipe
        if(pipe(pfd)) {
            printf("Error: Can not instantiate pipe!\n");
            exit(EXIT_FAILURE);
        }

        //get rights and last modify time for file
        strftime(date, sizeof(date), "%d.%m.%Y", localtime(&fileStats.st_mtime));
        getRigths(fileStats, file_rights.user_rights, file_rights.group_rights, file_rights.other_rights);
        lines_written = 0;

        if((pid = fork() ) < 0) {
            perror("Error! Could not instantiate child process!\n");
            exit(EXIT_FAILURE);
        }
        //check for file
         if(S_ISREG(fileStats.st_mode)) {
            //oppening entry file
            if ((fin = open(path_to_entry, O_RDWR)) == -1) {
                perror("Error opening input file!\n");
                exit(EXIT_FAILURE);
            }
            if((pid2 = fork() ) < 0) {
                    perror("Error! Could not instantiate child process!\n");
                    exit(EXIT_FAILURE);
            }          
            if(pid == 0) {
                // check for .bmp file
                if(strstr(entry->d_name, ".bmp") != NULL) {
                    processBMP(fin, entry, date, fileStats, foutContent, file_rights);
                    lines_written = 10;
                // check for ordinary file
                } else {
                    processFile(entry, date, fileStats, foutContent, file_rights);
                    lines_written = 9;

                    close(pfd[0]);
                    dup2(pfd[1], 1);
                    execlp("cat", "cat", path_to_entry, NULL);
                    close(pfd[1]);
                }
            } else if(pid2 == 0){
                // check for .bmp file
                if(strstr(entry->d_name, ".bmp") != NULL) {
                    convertRGBtoGrayscaleBMP(fin);
                // check for ordinary file
                } else {
                    //proces citire propozitii
                    close(pfd[1]); // closing writing end of pipe
                    dup2(pfd[0], 0);

                    execlp("bash", "bash", "script.sh", character, NULL);
                }
                exit(EXIT_SUCCESS);
            } else {
                stopProcess2(pid2, entry, &total_regex_lines);
            }
            // closing entry file
            if(close(fin) != 0) {
                perror("Error! Could not close file!\n");
                exit(EXIT_FAILURE); 
            }
        // check for symbolic link
        } else if(S_ISLNK(fileStats.st_mode)) {
            if(pid == 0){
                processSymbolicLink(entry, path_to_entry, fileStats, foutContent, file_rights);
                lines_written = 6;
            }
        // check for directory
        } else if(S_ISDIR(fileStats.st_mode) && pid == 0) {
            if(pid == 0) {
                processDirectory(entry, fileStats, foutContent, file_rights);
                lines_written = 5;
            }
        } 

        // reseting rights and last modify time for file
        strcpy(date, "");
        strcpy(file_rights.user_rights, "");
        strcpy(file_rights.group_rights, "");
        strcpy(file_rights.other_rights, "");

        // writing to <director_iesire>/<nume_intrare>_statistica.txt

        close(pfd[0]);
        close(pfd[1]);
        
        closeFileEndProcess(foutContent, diroutpath, pid, entry, lines_written); 
    }

    printf("Au fost identificate in total %d propozitii corecte care contin caracterul '%c'\n", total_regex_lines, character);
}

int main(int argc, char **argv) {
    // input verification
    if(argc != 4) {
        printf("Error! Program should look like: Usage ./program <director_intrare> <director_iesire> <c>!\n");
        exit(EXIT_FAILURE);
    }

    DIR* directory = NULL;
    DIR* directory_out = NULL;
    
    // opening directory
    if((directory = opendir(argv[1])) == 0 || (directory_out = opendir(argv[2])) == 0) {
        perror("Error! Unable to open directory!\n");
        exit(EXIT_FAILURE);
    }

    // solving:
    generateStats(directory, directory_out, argv[1], argv[2], argv[3][0]);

    // closing directory
    closedir(directory);

    return 0;
}