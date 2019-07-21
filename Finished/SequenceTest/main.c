#include <stdio.h>
#include <stdlib.h>

#include "linear_sequence.h"

LSQ_HandleT Seq1, Seq2;
LSQ_IteratorT It1, It2;
int *Arr;

void LSQ_PrintOut(LSQ_HandleT handle){
    printf("\nSequence is ");
    if(handle == LSQ_HandleInvalid){
        printf("invalid\n");
        return;
    }
    if(LSQ_GetSize(handle) == 0){
        printf("empty\n");
        return;
    }
    LSQ_IteratorT ItT = LSQ_GetFrontElement(handle);
    while(LSQ_IsIteratorDereferencable(ItT)){
        printf("%i ", *LSQ_DereferenceIterator(ItT));
        LSQ_AdvanceOneElement(ItT);
    }
    printf(" size is %i\n", LSQ_GetSize(handle));
    LSQ_DestroyIterator(ItT);
}

int main(){
    Arr = malloc(sizeof(int)*100);
    for(int i = 0; i < 100; i++){
        Arr[i] = i;
    }
    Seq1 = LSQ_CreateSequence();
    Seq2 = LSQ_CreateSequence();
    LSQ_InsertRearElement(Seq1, 1);
    LSQ_InsertRearElement(Seq1, 2);
    LSQ_InsertRearElement(Seq1, 3);
    LSQ_InsertRearElement(Seq1, 4);
    LSQ_InsertFrontElement(Seq1, 0);
    LSQ_PrintOut(Seq1);
    LSQ_DeleteRearElement(Seq1);
    LSQ_DeleteFrontElement(Seq1);
    LSQ_PrintOut(Seq1);
    It1 = LSQ_GetElementByIndex(Seq1, 0);
    printf("%i is element No 0\n", *LSQ_DereferenceIterator(It1));
    LSQ_ShiftPosition(It1, 1);
    printf("%i is element No 0+1\n", *LSQ_DereferenceIterator(It1));
    LSQ_InsertElementBeforeGiven(It1, 50);
    printf("Added 50 to pos 0+1\n");
    LSQ_PrintOut(Seq1);
    LSQ_SetPosition(It1, 3);
    LSQ_RewindOneElement(It1);
    LSQ_DeleteGivenElement(It1);
    LSQ_PrintOut(Seq1);


    printf("start destroying\n");
    LSQ_DestroySequence(Seq1);
    LSQ_DestroySequence(Seq2);
    return 0;
}
