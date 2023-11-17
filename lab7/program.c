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

#define BUFF_SIZE 4096

void rigths(struct stat fileStats, char* user, char* group, char* other)
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

void generateStats(DIR* directory, int fout, char *dirpath) {
    char foutContent[BUFF_SIZE+1];
    char path_to_entry[BUFF_SIZE+1];
    char user_rights[4] = "", group_rights[4] = "", other_rights[4] = ""; 
    char date[20];
    long int fileSize = 0;
    unsigned int h = 0, w = 0;
    int fin = 0;

    struct dirent* entry = NULL;
    struct stat fileStats;
    struct stat target_fileStats;

    while((entry = readdir(directory)) != NULL) {
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, dirpath) == 0) {
            continue;
        }

        strcpy(foutContent, "");
        snprintf(path_to_entry, sizeof(path_to_entry), "%s/%s", dirpath, entry->d_name);

        if(lstat(path_to_entry, &fileStats) == 0) {
            fileSize = fileStats.st_size;

        } else {
            printf("Error: Can not get file stats!\n");
            exit(-1);
        }

         if(S_ISREG(fileStats.st_mode)) {
            if(strstr(entry->d_name, ".bmp") != NULL) {
                fin = open(path_to_entry, O_RDONLY);
                if (fin == -1) {
                    perror("Error opening input file!\n");
                    exit(-1);
                }

                if(lseek(fin, 18, SEEK_SET) == -1) {
                    perror("Error setting cursor!\n");
                    close(fin);
                    exit(-1);
                }
                // read(fin, &finContent, BUFF_SIZE);

                read(fin, &w, 4);
                read(fin, &h, 4);  

                strftime(date, sizeof(date), "%d.%m.%Y", localtime(&fileStats.st_mtime));
                rigths(fileStats, user_rights, group_rights, other_rights);
            
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
                        entry->d_name, h, w, fileSize, fileStats.st_uid, date, fileStats.st_nlink, user_rights, group_rights, other_rights);
                
                if(close(fin) != 0) {
                    perror("Error! Could not close file %s !\n");
                    exit(-1); 
                }
            } else {
                strftime(date, sizeof(date), "%d.%m.%Y", localtime(&fileStats.st_mtime));
                rigths(fileStats, user_rights, group_rights, other_rights);

                sprintf(foutContent, "nume fisier: %s\n"
                                     "dimensiune: %ld By\n"
                                     "identificatorul utilizatorului: %d\n"
                                     "timpul ultimei modificari: %s\n"
                                     "contorul de legaturi: %ld\n"
                                     "drepturi de acces user: %s\n"
                                     "drepturi de acces grup: %s\n"
                                     "drepturi de acces altii: %s\n\n", 
                        entry->d_name, fileSize, fileStats.st_uid, date, fileStats.st_nlink, user_rights, group_rights, other_rights);
            }
         } else if(S_ISLNK(fileStats.st_mode)) {
            rigths(fileStats, user_rights, group_rights, other_rights);
            if(stat(path_to_entry, &target_fileStats) == -1)
            {
                perror("Error getting target file information for symbolic link!\n");
                exit(-1);
            }
            sprintf(foutContent, "nume legatura: %s\n"
                                 "dimensiune legatura: %ld\n"
                                 "dimensiune fisier: %ld\n"
                                 "drepturi de acces user: %s\n"
                                 "drepturi de acces grup: %s\n"
                                 "drepturi de acces altii: %s\n\n",
                    entry->d_name, fileStats.st_size, target_fileStats.st_size, user_rights, group_rights, other_rights);
         } else if(S_ISDIR(fileStats.st_mode)) {
            rigths(fileStats, user_rights, group_rights, other_rights);
            
            snprintf(foutContent, sizeof(foutContent), "nume director: %s\n"
                                                       "identificatorul utilizatorului: %d\n"
                                                       "drepturi de acces user: %s\n"
                                                       "drepturi de acces grup: %s\n"
                                                       "drepturi de acces altii: %s\n\n",
                    entry->d_name, fileStats.st_uid, user_rights, group_rights, other_rights);
         }

        strcpy(date, "");
        strcpy(user_rights, "");
        strcpy(group_rights, "");
        strcpy(other_rights, "");

        if(strcmp(foutContent, "") != 0) {
            if (write(fout, foutContent, strlen(foutContent)) == -1) {
                perror("Error writing to output file");
                close(fout);
                exit(-1);
            }
        }
    }
}

int main(int argc, char **argv) {
    // input verification

    if(argc != 2) {
        printf("Error! Program should look like: Usage ./program <director_intrare>!\n");
        exit(-1);
    }

    // opening directory

    DIR* directory = NULL;
    if(!(directory = opendir(argv[1])))
    {
        perror("Error! Unable to open directory %s!\n");
        exit(-1);
    }

    // creating & opening output file

    int fout = open("statistica.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fout == -1) 
    {
        perror("Error opening output file!\n");
        exit(-1);
    }

    // solving:

    generateStats(directory, fout, argv[1]);

    // closing files verification
    if(close(fout) != 0) {
        perror("Error! Could not save and close file %s !\n");
        exit(-1); 
    }

    // closing directory
    closedir(directory);

    return 0;
}