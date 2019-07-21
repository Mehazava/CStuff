#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

FILE *FInp, *FOut;
unsigned short BPP;
long RowSizeX, RowSizeb;
long i, j;
char Buff;
unsigned long long BitV[64];
unsigned long Offset, Width, OffsetFromEnd, ShiftFirstRead;
long Height;
unsigned long AllOnes;

struct Decoder{
    int Shift, Filled;
    union DT{
        unsigned long long B;
        unsigned int A[2];
    }D;
} InpB, OutB;

void PrintOut(unsigned long long Test, int Nbit){
    for(int i = Nbit - 1; i >= 0; i--){
        printf("%i", ((Test & BitV[i])>0));
        if(i == 32){printf(" ");}
    }
    printf("\n");
}

unsigned long GetL(){//<-yay
    unsigned long Temp;

    while(InpB.Filled < BPP){
        InpB.D.B <<= InpB.Filled;
        fread(InpB.D.A, 4, 1, FInp);
printf("\nRead: ");
PrintOut(InpB.D.A[0], 32);
        InpB.Shift = InpB.Filled;
        InpB.Filled += 32;
        if(ShiftFirstRead > 0){
            InpB.D.A[0] <<= ShiftFirstRead;
            InpB.Filled -= ShiftFirstRead;
            ShiftFirstRead = 0;
            if(InpB.Filled < BPP){
                fseek(FInp, -8, SEEK_CUR);
                InpB.D.B <<= InpB.Filled;
                fread(InpB.D.A, 4, 1, FInp);
printf("\nRead: ");
PrintOut(InpB.D.A[0], 32);
                InpB.Shift = InpB.Filled;
                InpB.Filled += 32;
            }
        }
        fseek(FInp, -8, SEEK_CUR);
    }

printf("\nPre shift: ");
PrintOut(InpB.D.B, 64);
    InpB.D.B <<= (BPP - InpB.Shift);
    InpB.Filled -= BPP;
printf("\nPos shift: ");
PrintOut(InpB.D.B, 64);
printf("\n A[0]: ");
PrintOut(InpB.D.A[0], 32);
printf("\n A[1]: ");
PrintOut(InpB.D.A[1], 32);

    Temp = InpB.D.A[1];// << (32 - BPP);
    InpB.D.A[1] = 0;
    InpB.Shift = 0;
	printf("\nTemp is %lu ", Temp);
	PrintOut(Temp, 32);
	getchar();
    return Temp;
}

void PutR(unsigned long Temp){//->
    if(OutB.Filled >= 32){
        fwrite(&AllOnes, 4, 1, FOut);//repla witha OutB.D.A
        OutB.D.A[0] = 0;
        OutB.Filled -= 32;
        OutB.D.B >>= OutB.Filled;

    }
    OutB.D.A[1] = Temp << (32 - BPP);
    OutB.D.A[1] >>= (32 - BPP);//to set zeroes /not needed if temp < (32 - BPP)
    OutB.D.B >>= ((32 - OutB.Filled < BPP) ? (32 - OutB.Filled) : BPP);
    OutB.Filled += BPP;
}

void FlushDec(){
    if(OutB.Filled >= 32){
        fwrite(&AllOnes, 4, 1, FOut);//repla witha OutB.D.A
        OutB.D.A[0] = 0;
        OutB.Filled -= 32;
        OutB.D.B >>= OutB.Filled;
    }
    if(OutB.Filled){
        OutB.D.A[0] >>= (32 - OutB.Filled);
        OutB.Filled = 0;
        OutB.Shift = 0;
        fwrite(&AllOnes, 4, 1, FOut);//repla witha OutB.D.A
    }
}

int main(int argc, char **argv){  //    /media/sf_ProgramsCode/Semester 2/C/students-master
    //DIR *Path = opendir("/media/sf_ProgramsCode/Semester 2/C/students-master");
    //closedir(Path);
    FInp = fopen("input.bmp", "rb");
    FOut = fopen("output.bmp", "wb");
    //FInp = fopen("/media/sf_ProgramsCode/Semester 2/C/students-master/input.bmp", "rb");
    //FInp = fopen("/media/sf_ProgramsCode/Semester 2/C/students-master/output.bmp", "wb");
    fseek(FInp, 0L, SEEK_END);
    unsigned long InputSize = ftell(FInp);
    AllOnes = 4294967295;
    BitV[0] = 1;
    for(i = 1; i < 64; i++){
        BitV[i] = BitV[i-1] << 1;
    }

    fseek(FInp, 0L, SEEK_SET);
    char *InputStuff[100];
    fread(InputStuff, 1, 20, FInp);

    fseek(FInp, 10L, SEEK_SET);
    fread(&Offset, 4, 1, FInp);
    fseek(FInp, 18L, SEEK_SET);
    fread(&Width, 4, 1, FInp);
    fseek(FInp, 22L, SEEK_SET);
    fread(&Height, 4, 1, FInp);
    if(Height < 0){Height = 0 - Height;}

    printf("Flipping a %li by %lu px picture... ", Height, Width);

    fseek(FInp, 28L, SEEK_SET);
    fread(&BPP, 2, 1, FInp);

    RowSizeX = BPP * Width;
    RowSizeX = (RowSizeX + 31) / 32;
    RowSizeb = 4 * RowSizeX;
    OffsetFromEnd = RowSizeb * 8 - Width * BPP;//cuz rows are prolonged to be X4B
printf("offset is %lu\n", OffsetFromEnd);
printf("Inp adrs are: \nB:%p\nA[0]:%p\nA[1]%p\n", &(InpB.D.B), &(InpB.D.A[0]), &(InpB.D.A[1]));

    fseek(FInp, 0L, SEEK_SET);
    for(i = 0; i < Offset; i++){
        fread(&Buff, 1, 1, FInp);
        fwrite(&Buff, 1, 1, FOut);
    }
    for(j = Height - 1; j >= 0; j--){
        ShiftFirstRead = OffsetFromEnd;
        fseek(FInp, (Offset + RowSizeb * (j+1) - 4), SEEK_SET);
        InpB.D.B = 0;
        OutB.D.B = 0;
        InpB.Shift = 0;
        InpB.Filled = 0;
        OutB.Shift = 0;
        OutB.Filled = 0;
        for(i = 0; i < Width; i++){
            PutR(GetL());
        }
        FlushDec();
    }
    while(!(feof(FInp))){
        fread(&Buff, 1, 1, FInp);
        fwrite(&Buff, 1, 1, FOut);
    }
    fclose(FInp);
    fclose(FOut);
    printf("Done!\n");
    return 0;
}
