#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

typedef char FStatA[10][20];//maxfiles/separate parameters for file/maxlen for parameters

//[- 0)C 1)F 2)R 3)a 4)c 5)d 6)i 7)l 8)q 9)r 10)t 11)u 12)1] [--]
int SortM = 0, SortR = 0, AddPost = 0, AddSerial = 0, Recur = 0, DispInv = 0, ListAttr = 0, ReplQuest = 1, SingleCol = 0, NoCat = 0;//argument modifications
int TerWidth, StrStart[16], StrInCol;//columns management

int GetFileStats(char *SPath, char *FName, FStatA *FileStats){
    char *FullFP;
    struct stat CurStat;
    FullFP = malloc(sizeof(char)*(strlen(SPath)+strlen(FName)+2));
    strcpy(FullFP, SPath);
    strcpy(FullFP + strlen(SPath),"/\0");
    strcpy(FullFP + strlen(FullFP), FName);
    if(stat(FullFP, &CurStat) == -1){
        printf("E: couldn't get info on %s\n", FullFP);
        free(FullFP);
        return 1;
    }else{
        //0 is all the rrwrwerwrw crap
        sprintf((*FileStats)[1], "%d", CurStat.st_nlink);
        sprintf((*FileStats)[2], "%s", CurStat.st_uid);
        sprintf((*FileStats)[3], "%s", CurStat.st_gid);
        sprintf((*FileStats)[4], "%ld", (long)CurStat.st_size);
        sprintf((*FileStats)[5], "%s", 4 + ctime(&(CurStat.st_mtime)));
        sprintf((*FileStats)[6], "%d", CurStat.st_ino);
        sprintf((*FileStats)[7], "%s", FName);

        strcpy((*FileStats)[8], "\0");
        if(S_ISDIR(CurStat.st_mode)){
            strcpy((*FileStats)[8], "/\0");
        }else if(CurStat.st_mode & S_IXUSR){
            strcpy((*FileStats)[8], "*\0");
        }else if(S_ISFIFO(CurStat.st_mode)){
            strcpy((*FileStats)[8], "|\0");
        }
    }
    free(FullFP);
    return 0;
}

int DoIt(char *SPath){
    FStatA *FileStats;
    int FilesInDir = 0;

    DIR *Path = opendir(SPath);
    char *FileName;
    struct dirent *CurFile;
    if(!(Path)){
		return 1;
	}

    while(CurFile = readdir(Path)){//count files in dir
        FilesInDir++;
    }
    closedir(Path);
    FileStats = malloc(sizeof(FStatA)*FilesInDir);

    FilesInDir = 0;
    Path = opendir(SPath);
    while(CurFile = readdir(Path)){//fill the stat table
        GetFileStats(SPath, CurFile->d_name, FileStats + FilesInDir);
        FilesInDir++;
	}
    closedir(Path);
    //sort the stat table
    //////////////////////
    for(int i = 0; i < FilesInDir; i++){//print the table
        if(ListAttr){
            for(int j = 1; j < 6; j++){
                printf("%s ", FileStats[i][j]);
            }
        }
        if(AddSerial){
            printf("%s ", FileStats[i][6]);
        }
        printf("%s", FileStats[i][7]);
        if(AddPost){
            printf("%s", FileStats[i][8]);
        }
        printf("\n");
    }
    if(Recur){//if recursive call is on do it

    }
    free(FileStats);
    return 0;
}

int main(int argc, char **argv){
    int i, ArgNamed = 0;
    char PathString[69], ArgFile[42];
    if(argc < 2){
        printf("E: correct use is Comm Path *[name -?]");
        return 1;
    }
    strcpy(PathString, argv[1]);
    //TerWidth = atoi(argv[2]);
    for(i = 3; i < argc; i++){
        if((argv[i][0] == '-')&&(argv[i][0] != '\0')&&(argv[i][2] == '\0')){
            switch(argv[i][1]){
                case 'C':  break;
                case 'F': AddPost = 1; break;
                case 'R': Recur = 1; break;
                case 'a': DispInv = 1; break;
                case 'c': SortM = 1; break;
                case 'd': NoCat = 1; break;
                case 'i': AddSerial = 1; break;
                case 'l': SingleCol = 1; ListAttr = 1; break;
                case 'q': ReplQuest = 1; break;
                case 'r': SortR = 1; break;
                case 't': SortM = 2; break;
                case 'u': SortM = 3; break;
                case '1': SingleCol = 1; break;
                case '-': i = 10000; break;
                default:
                    printf("Unknown argument %s\n", argv[i]);
                    return 1;
            }
        }else if(ArgNamed == 0){
            printf("~~argfilerecorded~~\n");
            strcpy(ArgFile, argv[i]);
            ArgNamed = 1;
        }else{
            printf("Unknown argument %s\n", argv[i]);
            return 1;
        }
    }
    if(ArgNamed){
        strcat(PathString, "/");
        strcat(PathString, ArgFile);
        DIR *Path = opendir(argv[1]);
        struct dirent *CurFile;
        struct stat *CurStat;
        if(!(Path)){
            closedir(Path);
            return 1;
        }
        while(CurFile = readdir(Path)){
            if(strcmp(CurFile->d_name, ArgFile) == 0){
                if(stat(PathString, CurStat) == -1){
                    printf("Can't get data about file %s/n", ArgFile);
                    return 1;
                }
                if(CurStat->st_mode == S_IFDIR){
                    if(NoCat){
                        if(ListAttr){
                            ////////////////////////////////////////
                        }
                        if(AddSerial){
                            printf(" %li", CurFile->d_ino);
                        }
                        printf(" %s", ArgFile);
                        if(AddPost){
                            printf("/\n");
                        }
                    }else{
                        if(DoIt(PathString)){
                            closedir(Path);
                            return 1;
                        }
                    }
                }else{
                    if(ListAttr){
                        ////////////////////////////////////////
                    }
                    if(AddSerial){
                        printf(" %li", CurFile->d_ino);
                    }
                    printf(" %s", ArgFile);
                    if(AddPost){
                        if(CurStat->st_mode == S_IFIFO){
                            printf("|\n");
                        }else if(CurStat->st_mode & S_IXUSR){
                            printf("*\n");
                        }
                    }
                }
                closedir(Path);
                return 0;
            }
            CurFile = readdir(Path);
        }
    }else{
        if(!(NoCat)){
            if(!(DoIt(PathString))){
                printf("E: ?\n");
            }
        }
    }


	//for(i = 2; i < argc; i++){
	//	printf("arg %i: %s\n", i, argv[i]);
	//}
	return 0;
}
