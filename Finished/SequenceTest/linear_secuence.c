#include "linear_sequence.h"

typedef struct LinearSeqElem TLSElem;
typedef struct LinearSeqCont TLSCont;
typedef struct LinearSeqIter TLSIter;
typedef struct LinearSeqElem{
    struct LinearSeqElem *Next, *Prev;
    LSQ_BaseTypeT Val;
}TLSElem;
typedef struct LinearSeqCont{
    LSQ_IntegerIndexT Size;
    TLSElem *First, *Last;
    TLSIter *RelaIter;
}TLSCont;
typedef struct LinearSeqIter{
    LSQ_IntegerIndexT Pos;
    TLSElem *Cur;
    TLSCont *Seq;
    TLSIter *NextForSeq;
}TLSIter;

LSQ_HandleT LSQ_CreateSequence(void){
    TLSCont *SeqAdr = malloc(sizeof(TLSCont));
    if(SeqAdr){
        SeqAdr->Size = 0;
        SeqAdr->First = NULL;
        SeqAdr->Last = NULL;
        SeqAdr->RelaIter = NULL;
        return SeqAdr;
    }
    return LSQ_HandleInvalid;
}

void LSQ_DestroySequence(LSQ_HandleT handle){//i presume i don't need to free elements themselves e.g. int when seq stores pointers to int
    if(handle == LSQ_HandleInvalid){
        return;
    }
    if(((TLSCont*)handle)->Size){
        TLSElem *Cur = ((TLSCont*)handle)->First;
        while(Cur->Next){
            Cur = Cur->Next;
            free(Cur->Prev);
        }
        free(Cur);
    }
    TLSIter *GoThrough, *Del = ((TLSCont*)handle)->RelaIter;
    while(Del){
        GoThrough = Del->NextForSeq;
        free(Del);
        Del = GoThrough;
    }
    free(handle);//////////////////////add related iterator destructor
}

LSQ_IntegerIndexT LSQ_GetSize(LSQ_HandleT handle){
    if(handle != LSQ_HandleInvalid){
        return ((TLSCont*)handle)->Size;
    }else{
        return -1;
    }
}

int LSQ_IsIteratorDereferencable(LSQ_IteratorT iterator){
    if(iterator && (((TLSIter*)iterator)->Seq != LSQ_HandleInvalid) && ((TLSIter*)iterator)->Cur){
        return 1;
    }
    return 0;
}

int LSQ_IsIteratorPastRear(LSQ_IteratorT iterator){
    if(iterator && (((TLSIter*)iterator)->Seq != LSQ_HandleInvalid) && (!((TLSIter*)iterator)->Cur) && ((TLSIter*)iterator)->Pos >= ((TLSIter*)iterator)->Seq->Size){
        return 1;
    }
    return 0;
}

int LSQ_IsIteratorBeforeFirst(LSQ_IteratorT iterator){
    if(iterator && (((TLSIter*)iterator)->Seq != LSQ_HandleInvalid) && (!((TLSIter*)iterator)->Cur) && ((TLSIter*)iterator)->Pos < 0){
        return 1;
    }
    return 0;
}

LSQ_BaseTypeT* LSQ_DereferenceIterator(LSQ_IteratorT iterator){
    if(iterator && (((TLSIter*)iterator)->Seq != LSQ_HandleInvalid) && ((TLSIter*)iterator)->Cur){
        return &(((TLSIter*)iterator)->Cur->Val);
    }
    return NULL;
}

LSQ_IteratorT LSQ_GetElementByIndex(LSQ_HandleT handle, LSQ_IntegerIndexT index){
    if(handle == LSQ_HandleInvalid){
        return NULL;
    }
    TLSIter *IterAdr = malloc(sizeof(TLSIter));
    if(!(IterAdr)){
        return NULL;
    }
    if((index < 0) || (index >= ((TLSCont*)handle)->Size)){
        IterAdr->Cur = NULL;
        IterAdr->Pos = index;
    }else{
        if(index > ((TLSCont*)handle)->Size / 2){
            IterAdr->Cur = ((TLSCont*)handle)->Last;
            IterAdr->Pos = ((TLSCont*)handle)->Size - 1;
            while(IterAdr->Pos > index){
                IterAdr->Cur = IterAdr->Cur->Prev;
                IterAdr->Pos--;
            }
        }else{
            IterAdr->Cur = ((TLSCont*)handle)->First;
            IterAdr->Pos = 0;
            while(IterAdr->Pos < index){
                IterAdr->Cur = IterAdr->Cur->Next;
                IterAdr->Pos++;
            }
        }
    }
    IterAdr->Seq = handle;
    IterAdr->NextForSeq = NULL;
    TLSIter **GoThrough = &(((TLSCont*)handle)->RelaIter);//place iter to the end of list of iterators for that sequense
    while(*GoThrough){
        GoThrough = &((*GoThrough)->NextForSeq);
    }
    *GoThrough = IterAdr;
    return IterAdr;
}

LSQ_IteratorT LSQ_GetFrontElement(LSQ_HandleT handle){
    if(handle == LSQ_HandleInvalid){
        return NULL;
    }
    TLSIter *IterAdr = malloc(sizeof(TLSIter));
    if(!(IterAdr)){
        return NULL;
    }
    IterAdr->Cur = ((TLSCont*)handle)->First;
    IterAdr->Pos = 0;
    IterAdr->Seq = handle;
    IterAdr->NextForSeq = NULL;
    TLSIter **GoThrough = &(((TLSCont*)handle)->RelaIter);//place iter to the end of list of iterators for that sequense
    while(*GoThrough){
        GoThrough = &((*GoThrough)->NextForSeq);
    }
    *GoThrough = IterAdr;
    return IterAdr;
}

LSQ_IteratorT LSQ_GetPastRearElement(LSQ_HandleT handle){
    if(handle == LSQ_HandleInvalid){
        return NULL;
    }
    TLSIter *IterAdr = malloc(sizeof(TLSIter));
    if(!(IterAdr)){
        return NULL;
    }
    IterAdr->Cur = NULL;
    IterAdr->Pos = ((TLSCont*)handle)->Size;
    IterAdr->Seq = handle;
    IterAdr->NextForSeq = NULL;
    TLSIter **GoThrough = &(((TLSCont*)handle)->RelaIter);//place iter to the end of list of iterators for that sequense
    while(*GoThrough){
        GoThrough = &((*GoThrough)->NextForSeq);
    }
    *GoThrough = IterAdr;
    return IterAdr;
}

void LSQ_DestroyIterator(LSQ_IteratorT iterator){
    if(((TLSIter*)iterator)->Seq == LSQ_HandleInvalid){
        return;
    }
    TLSIter **GoThrough = &(((TLSIter*)iterator)->Seq->RelaIter);
    while(*GoThrough != iterator){
        GoThrough = &((*GoThrough)->NextForSeq);
    }
    *GoThrough = ((TLSIter*)iterator)->NextForSeq;
    free(iterator);
}

void LSQ_AdvanceOneElement(LSQ_IteratorT iterator){
    if(!(iterator) || ((TLSIter*)iterator)->Seq == LSQ_HandleInvalid){
        return;
    }
    ((TLSIter*)iterator)->Pos++;
    if(((TLSIter*)iterator)->Cur){
        ((TLSIter*)iterator)->Cur = ((TLSIter*)iterator)->Cur->Next;
    }else if(((TLSIter*)iterator)->Pos == -1){
        ((TLSIter*)iterator)->Cur = ((TLSIter*)iterator)->Seq->First;
    }
}

void LSQ_RewindOneElement(LSQ_IteratorT iterator){
    if(!(iterator) || ((TLSIter*)iterator)->Seq == LSQ_HandleInvalid){
        return;
    }
    ((TLSIter*)iterator)->Pos--;
    if(((TLSIter*)iterator)->Cur){
        ((TLSIter*)iterator)->Cur = ((TLSIter*)iterator)->Cur->Prev;
    }else if(((TLSIter*)iterator)->Pos == ((TLSIter*)iterator)->Seq->Size){
        ((TLSIter*)iterator)->Cur = ((TLSIter*)iterator)->Seq->Last;
    }
}

void LSQ_ShiftPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT shift){
    if(!(iterator) || (((TLSIter*)iterator)->Seq == LSQ_HandleInvalid) || (shift == 0)){
        printf("shiftfail\n");
        return;
    }
    TLSIter *IterAdr = iterator;
    LSQ_IntegerIndexT NewPos = IterAdr->Pos + shift;
    if((NewPos < 0) || (NewPos >= IterAdr->Seq->Size)){
        IterAdr->Cur = NULL;
        IterAdr->Pos = NewPos;
    }else{
        if(IterAdr->Cur){
            if(shift < 0){
                while(IterAdr->Pos > NewPos){
                    IterAdr->Cur = IterAdr->Cur->Prev;
                    IterAdr->Pos--;
                }
            }else{
                while(IterAdr->Pos < NewPos){
                    IterAdr->Cur = IterAdr->Cur->Next;
                    IterAdr->Pos++;
                }
            }
        }else if(NewPos > IterAdr->Seq->Size / 2){
            IterAdr->Cur = IterAdr->Seq->Last;
            IterAdr->Pos = IterAdr->Seq->Size - 1;
            while(IterAdr->Pos > NewPos){
                IterAdr->Cur = IterAdr->Cur->Prev;
                IterAdr->Pos--;
            }
        }else{
            IterAdr->Cur = IterAdr->Seq->First;
            IterAdr->Pos = 0;
            while(IterAdr->Pos < NewPos){
                IterAdr->Cur = IterAdr->Cur->Next;
                IterAdr->Pos++;
            }
        }
    }
}

void LSQ_SetPosition(LSQ_IteratorT iterator, LSQ_IntegerIndexT pos){
    if(!(iterator) || (((TLSIter*)iterator)->Seq == LSQ_HandleInvalid)){
        return;
    }
    TLSIter *IterAdr = iterator;
    if(pos > IterAdr->Seq->Size / 2){
        IterAdr->Cur = IterAdr->Seq->Last;
        IterAdr->Pos = IterAdr->Seq->Size - 1;
        while(IterAdr->Pos > pos){
            IterAdr->Cur = IterAdr->Cur->Prev;
            IterAdr->Pos--;
        }
    }else{
        IterAdr->Cur = IterAdr->Seq->First;
        IterAdr->Pos = 0;
        while(IterAdr->Pos < pos){
            IterAdr->Cur = IterAdr->Cur->Next;
            IterAdr->Pos++;
        }
    }
}

void LSQ_InsertFrontElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
    if(handle == LSQ_HandleInvalid){
        return;
    }
    TLSElem *NewElem = malloc(sizeof(TLSElem));
    if(!(NewElem)){
        return;
    }
    NewElem->Val = element;
    NewElem->Prev = NULL;
    NewElem->Next = ((TLSCont*)handle)->First;
    if(((TLSCont*)handle)->Size > 0){
        ((TLSCont*)handle)->First->Prev = NewElem;
    }else{
        ((TLSCont*)handle)->Last = NewElem;
    }
    ((TLSCont*)handle)->First = NewElem;
    TLSIter *GoThrough = ((TLSCont*)handle)->RelaIter;
    while(GoThrough){
        if(GoThrough->Pos >= 0){
           GoThrough->Pos++;
        }
        GoThrough = GoThrough->NextForSeq;
    }
    ((TLSCont*)handle)->Size++;
}

void LSQ_InsertRearElement(LSQ_HandleT handle, LSQ_BaseTypeT element){
    if(handle == LSQ_HandleInvalid){
        return;
    }
    TLSElem *NewElem = malloc(sizeof(TLSElem));
    if(!(NewElem)){
        return;
    }
    NewElem->Val = element;
    NewElem->Next = NULL;
    NewElem->Prev = ((TLSCont*)handle)->Last;
    if(((TLSCont*)handle)->Size > 0){
        ((TLSCont*)handle)->Last->Next = NewElem;
    }else{
        ((TLSCont*)handle)->First = NewElem;
    }
    ((TLSCont*)handle)->Last = NewElem;
    TLSIter *GoThrough = ((TLSCont*)handle)->RelaIter;
    while(GoThrough){
        if(GoThrough->Pos >= ((TLSCont*)handle)->Size){
           GoThrough->Pos++;
        }
        GoThrough = GoThrough->NextForSeq;
    }
    ((TLSCont*)handle)->Size++;
}

void LSQ_InsertElementBeforeGiven(LSQ_IteratorT iterator, LSQ_BaseTypeT newElement){//this iterator is on added element afterwards
    if(!(iterator) || (((TLSIter*)iterator)->Seq == LSQ_HandleInvalid)){
        return;
    }
    TLSElem *NewElem = malloc(sizeof(TLSElem));
    if(!(NewElem)){
        return;
    }
    NewElem->Val = newElement;
    if(((TLSIter*)iterator)->Pos <= 0){//if iterator is outside of boundaries it will be shifted to add element in the rear or front appropriately
        ((TLSIter*)iterator)->Pos = 0;
        NewElem->Prev = NULL;
        NewElem->Next = ((TLSIter*)iterator)->Seq->First;
        if(((TLSIter*)iterator)->Seq->Size > 0){
            ((TLSIter*)iterator)->Seq->First->Prev = NewElem;
        }else{
            ((TLSIter*)iterator)->Seq->Last = NewElem;
        }
        ((TLSIter*)iterator)->Seq->First = NewElem;
    }else if(((TLSIter*)iterator)->Pos >= ((TLSIter*)iterator)->Seq->Size){
        ((TLSIter*)iterator)->Pos = ((TLSIter*)iterator)->Seq->Size;
        NewElem->Next = NULL;
        NewElem->Prev = ((TLSIter*)iterator)->Seq->Last;
        if(((TLSIter*)iterator)->Seq->Size > 0){
            ((TLSIter*)iterator)->Seq->Last->Next = NewElem;
        }else{
            ((TLSIter*)iterator)->Seq->First = NewElem;
        }
        ((TLSIter*)iterator)->Seq->Last = NewElem;
    }else{//has elements and within boundaries
        NewElem->Prev = ((TLSIter*)iterator)->Cur->Prev;
        NewElem->Prev->Next = NewElem;
        NewElem->Next = ((TLSIter*)iterator)->Cur;
        ((TLSIter*)iterator)->Cur->Prev = NewElem;
    }
    ((TLSIter*)iterator)->Cur = NewElem;
    TLSIter *GoThrough = ((TLSIter*)iterator)->Seq->RelaIter;
    while(GoThrough){
        if((GoThrough->Pos >= ((TLSIter*)iterator)->Pos) && (GoThrough != iterator)){
           GoThrough->Pos++;
        }
        GoThrough = GoThrough->NextForSeq;
    }
    ((TLSIter*)iterator)->Seq->Size++;
}

void LSQ_DeleteFrontElement(LSQ_HandleT handle){//iterators on deleted element won't have their pos shifted
    if((handle == LSQ_HandleInvalid) || (((TLSCont*)handle)->Size <= 0)){
        return;
    }
    if(((TLSCont*)handle)->First->Next){
        ((TLSCont*)handle)->First = ((TLSCont*)handle)->First->Next;
        free(((TLSCont*)handle)->First->Prev);
        ((TLSCont*)handle)->First->Prev = NULL;
    }else{
        free(((TLSCont*)handle)->First);
        ((TLSCont*)handle)->First = NULL;
        ((TLSCont*)handle)->Last = NULL;
    }
    ((TLSCont*)handle)->Size--;
    TLSIter *GoThrough = ((TLSCont*)handle)->RelaIter;
    while(GoThrough){
        if(GoThrough->Pos > 0){
           GoThrough->Pos--;
        }
        GoThrough = GoThrough->NextForSeq;
    }
}

void LSQ_DeleteRearElement(LSQ_HandleT handle){
    if((handle == LSQ_HandleInvalid) || (((TLSCont*)handle)->Size <= 0)){
        return;
    }
    if(((TLSCont*)handle)->Last->Prev){
        ((TLSCont*)handle)->Last = ((TLSCont*)handle)->Last->Prev;
        free(((TLSCont*)handle)->Last->Next);
        ((TLSCont*)handle)->Last->Next = NULL;
    }else{
        free(((TLSCont*)handle)->Last);
        ((TLSCont*)handle)->Last = NULL;
        ((TLSCont*)handle)->First = NULL;
    }
    ((TLSCont*)handle)->Size--;
    TLSIter *GoThrough = ((TLSCont*)handle)->RelaIter;
    while(GoThrough){
        if(GoThrough->Pos > ((TLSCont*)handle)->Size){
           GoThrough->Pos--;
        }
        GoThrough = GoThrough->NextForSeq;
    }
}

void LSQ_DeleteGivenElement(LSQ_IteratorT iterator){
    if(!(iterator) || (((TLSIter*)iterator)->Seq == LSQ_HandleInvalid)){
        return;
    }
    if((((TLSIter*)iterator)->Pos < 0) || (((TLSIter*)iterator)->Pos >= ((TLSIter*)iterator)->Seq->Size)){//also checks if size == 0
        return;
    }
    TLSElem *NewCur = ((TLSIter*)iterator)->Cur->Next;
    if(NewCur){
        NewCur->Prev = ((TLSIter*)iterator)->Cur->Prev;
    }else{
        ((TLSIter*)iterator)->Seq->Last = ((TLSIter*)iterator)->Cur->Prev;
    }
    if(((TLSIter*)iterator)->Cur->Prev){
        ((TLSIter*)iterator)->Cur->Prev->Next = NewCur;
    }else{
        ((TLSIter*)iterator)->Seq->First = NewCur;
    }
    free(((TLSIter*)iterator)->Cur);
    ((TLSIter*)iterator)->Cur = NewCur;
    ((TLSIter*)iterator)->Seq->Size--;
    TLSIter *GoThrough = ((TLSIter*)iterator)->Seq->RelaIter;
    while(GoThrough){
        if((GoThrough->Pos > ((TLSIter*)iterator)->Pos) && (GoThrough != iterator)){
           GoThrough->Pos--;
        }
        GoThrough = GoThrough->NextForSeq;
    }

}
