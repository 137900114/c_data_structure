#include <stdio.h>
#include <math.h>


#include "MArray.h"
#include "MMemoryPool.h"
#include "MLinkedList.h"

typedef struct _A{
    float grade;
    int   id;
    const char* name;
}A;

void printA(A* a){
    printf("student %d's name is %s.His/Her grade is %f\n",a->id,a->name,a->grade);
    return;
}


int main(){
    if(!mempool_initialize()){
        printf("fail to initialize memory pool");
    }else{
        printf("memory pool initialized successfully");
    }

    MLinkedList list = mlist_make(sizeof(A));
    
    A students[] = {
        {100.,12,"fuck"},{90.1,13,"shit"},{88.,1,"bitch"},{33.,12,"ass"},{313,23,"hole"}
    };

    int test[] = {3,1,66,7};

    for(int i = 0;i != _countof(students);i++){
        mlist_insert_end(&list,students + i);
    }

    for(int j = 0;j != _countof(test);j++){
        A a;
        if(!mlist_visit(&list,test[j],&a)){
            printf("index %d is invalid for link list\n",test[j]);
        }else{
            printA(&a);
            mlist_remove(&list,test[j]);
        }
    }

    size_t size = mlist_size(&list);
    for(int m = 0;m != size;m++){
        A a;
        mlist_visit(&list,m,&a);
        printA(&a);
    }

    mempool_finalize();
}