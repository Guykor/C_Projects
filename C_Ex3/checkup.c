//
// Created by guy_korn on 11/12/2019.
//
#include <stdlib.h>
#include "tests_c_ex3/RBTree.h"

int compareInt(const void *a, const void *b)
{
    int* ai = (int*)a;
    int* bi = (int*)b;
    return *ai-*bi;
}

void freeInt()
{

}

int main()
{

    RBTree *tree = newRBTree(compareInt, freeInt);
    int arr[] = {-3, 2, -4, 1, 3,-5,-1,4,0};
    for (int i = 0; i < 9; i++)
    {
        void* data = (void*)&arr[i];
        addToRBTree(tree,  data);
    }
    freeRBTree(tree);
}