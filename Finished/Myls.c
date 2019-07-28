#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

typedef char FStatA[10][20];//maxfiles/separate parameters for file/maxlen for parameters

//[- 0)C 1)F 2)R 3)a 4)c 5)d 6)i 7)l 8)q 9)r 10)t 11)u 12)1] [--]
int SortM = 0, SortR = 0, AddPost = 0, AddSerial = 0, Recur = 0, DispInv = 0, ListAttr = 0, ReplQuest = 1, SingleCol = 0, NoCat = 0;//argument modifications
int TerWidth, StrStart[16], StrInCol;//columns management

int GetFileStats(char *FullFP, FStatA *FileStats){
    struct stat CurStat;
    struct group *grp;
    struct passwd *pwd;

    if(stat(FullFP, &CurStat) == -1){
        printf("E: couldn't get info on %s\n", FullFP);
        return 1;
    }else{
        //0 is all the rrwrwerwrw cr
        sprintf((*FileStats)[1], "%ld", CurStat.st_nlink);
        pwd = getpwuid(CurStat.st_uid);
        if(!(pwd)){
            printf("E: can't get info about user %d\n", CurStat.st_uid);
            return 1;
        }
        sprintf((*FileStats)[2], "%s", pwd->pw_name);//
        grp = getgrgid(CurStat.st_gid);
        if(!(grp)){
            printf("E: can't get info about group %d\n", CurStat.st_gid);
            return 1;
        }
        sprintf((*FileStats)[3], "%s", grp->gr_name);//
        sprintf((*FileStats)[4], "%ld", (long)CurStat.st_size);
        sprintf((*FileStats)[5], "%s", 4 + ctime(&(CurStat.st_mtime)));
        sprintf((*FileStats)[6], "%ld", CurStat.st_ino);


        strcpy((*FileStats)[8], "\0");
        if(S_ISDIR(CurStat.st_mode)){
            strcpy((*FileStats)[8], "/\0");
        }else if(CurStat.st_mode & S_IXUSR){
            strcpy((*FileStats)[8], "*\0");
        }else if(S_ISFIFO(CurStat.st_mode)){
            strcpy((*FileStats)[8], "|\0");
        }
    }

    return 0;
}

int DoIt(char *SPath){
    FStatA *FileStats;
    int FilesInDir = 0;

    char *FullFP;
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

        FullFP = malloc(sizeof(char)*(strlen(SPath)+strlen(CurFile->d_name)+2));
        strcpy(FullFP, SPath);
        strcpy(FullFP + strlen(SPath),"/\0");
        strcpy(FullFP + strlen(FullFP), CurFile->d_name);
        if(GetFileStats(FullFP, FileStats + FilesInDir)){
		return 1;
	}
        sprintf(FileStats[FilesInDir][7], "%s", CurFile->d_name);
        free(FullFP);
        FilesInDir++;
	}
    closedir(Path);
    //sort the stat table

    //////////////////////
    int i;
    if(SortR){
        i = FilesInDir - 1;
    }else{
        i = 0;
    }
    for(int Flag1 = 1; Flag1; ){//print the table
        if((DispInv) || (FileStats[i][7][0] != '.')){
            if(ListAttr){
                for(int j = 1; j < 6; j++){
                    printf("%9.9s ", FileStats[i][j]);
                }
            }
            if(AddSerial){
                printf("%6.6s ", FileStats[i][6]);
            }
            printf("%9.9s", FileStats[i][7]);
            if(AddPost){
                printf("%s", FileStats[i][8]);
            }
            printf("\n");
        }
        if(SortR){
            if(--i < 0){Flag1 = 0;}
        }else{
            if(++i >= FilesInDir){Flag1 = 0;}
        }
    }
    if(Recur){//if recursive call is on do it for all cats
        for(i = 0; i < FilesInDir; i++){
            if((strcmp(FileStats[i][8], "/") == 0)&&(FileStats[i][7][0] != '.')){
                printf("\nList of files inside %s/:\n", FileStats[i][7]);
                FullFP = malloc(sizeof(char)*(strlen(SPath)+strlen(FileStats[i][7])+2));
                strcpy(FullFP, SPath);
                strcpy(FullFP + strlen(SPath),"/\0");
                strcpy(FullFP + strlen(FullFP), FileStats[i][7]);
                DoIt(FullFP);
                free(FullFP);
                printf("\n");
            }
        }
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
    for(i = 2; i < argc; i++){
        if((argv[i][0] == '-')&&(argv[i][0] != '\0')&&(argv[i][2] == '\0')){
            switch(argv[i][1]){
                case 'C':  break;
                case 'F': AddPost = 1; break;//kk!
                case 'R': Recur = 1; break;
                case 'a': DispInv = 1; break;//kk!
                case 'c': SortM = 1; break;//edit time
                case 'd': NoCat = 1; break;
                case 'i': AddSerial = 1; break;//kk!
                case 'l': SingleCol = 1; ListAttr = 1; break;//kk!
                case 'q': ReplQuest = 1; break;
                case 'r': SortR = 1; break;//kk!
                case 't': SortM = 5; break;//time stamp?/stat no 5
                case 'u': SortM = 3; break;//last access time
                case '1': SingleCol = 1; break;
                case '-': i = 10000; break;
                default:
                    printf("Unknown argument %s\n", argv[i]);
                    return 1;
            }
        }else if(ArgNamed == 0){
            //printf("~~argfilerecorded~~\n");
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
    }

    FStatA *FileStats;
    FileStats = malloc(sizeof(FStatA));
    if(GetFileStats(PathString, FileStats)){
		return 1;
    }
    sprintf(FileStats[0][7], "%s", strrchr(PathString, '/') + 1);
    if((NoCat) || (FileStats[0][8][0] != '/')){
        if(ListAttr){
            for(int j = 1; j < 6; j++){
                printf("%9.9s ", FileStats[0][j]);
            }
        }
        if(AddSerial){
            printf("%6.6s ", FileStats[0][6]);
        }
        printf("%9.9s", FileStats[0][7]);
        if(AddPost){
            printf("%s", FileStats[0][8]);
        }
        printf("\n");
    }else{
        if(DoIt(PathString)){
            printf("E: ?\n");
            free(FileStats);
            return 1;
        }
    }
    free(FileStats);

	//for(i = 2; i < argc; i++){
	//	printf("arg %i: %s\n", i, argv[i]);
	//}
	return 0;
}
