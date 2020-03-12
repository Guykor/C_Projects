#include "Structs.h"
#include <stdlib.h>
#include "string.h"

/**
 * missing comments for functions are in the header file.
 */
int vectorCompare1By1(const void *a, const void *b)
{
    Vector *v1 = (Vector *) a;
    Vector *v2 = (Vector *) b;

    int minLen = (v1->len >= v2->len) ? v1->len : v2->len;
    for (int i = 0; i < minLen; i++)
    {
        if (v1->vector[i] > v2->vector[i])
        {
            return 1;
        }
        else if (v1->vector[i] < v2->vector[i])
        {
            return -1;
        }
        else
        {
            continue;
        }
    }
    return v1->len > v2->len;
}

/**
 * calculates the norm of a given vector.
 * @return norm.
 */
double calcVectorSquaredNorm(Vector *v)
{
    if (v != NULL && v->vector != NULL)
    {
        double sqrNorm = 0;
        for (int i = 0; i < v->len; ++i)
        {
            sqrNorm += (v->vector[i] * v->vector[i]);
        }
        return sqrNorm;
    }
    return 0;
}

int copyIfNormIsLarger(const void *pVector, void *pMaxVector)
{
    if (pVector == NULL || pMaxVector == NULL)
    {
        return 0;
    }
    Vector *srcVector = (Vector *) pVector;
    Vector *destVector = (Vector *) pMaxVector;

    if (destVector->vector == NULL)
    {
        destVector->vector = (double *) malloc(sizeof(double) * srcVector->len);
        if (destVector->vector == NULL)
        {
            return 0;
        }
    }
    else //dest->vector != NULL
    {
        if (srcVector->vector == NULL)
        {
            return 0;
        }
        double srcNorm = calcVectorSquaredNorm(srcVector);
        double destNorm = calcVectorSquaredNorm(destVector);
        if (srcNorm > destNorm)
        {
            destVector->vector = realloc(destVector->vector, sizeof(double) * srcVector->len);
            if (destVector->vector == NULL)
            {
                return 0;
            }
        }
        else // srcNorm <= destNorm
        {
            return 0;
        }
    }
    destVector->len = srcVector->len;
    memcpy(destVector->vector, srcVector->vector, srcVector->len * sizeof(double));
    return 1;
}

Vector *findMaxNormVectorInTree(RBTree *tree)
{
    Vector *pMaxVector = (Vector *) malloc(sizeof(Vector));
    if (pMaxVector == NULL)
    {
        return 0;
    }
    pMaxVector->vector = NULL;
    pMaxVector->len = 0;
    forEachRBTree(tree, copyIfNormIsLarger, pMaxVector);
    return pMaxVector;
}

void freeVector(void *pVector)
{
    Vector *v = (Vector *) pVector;
    if (v != NULL)
    {
        if (v->vector != NULL)
        {
            free(v->vector);
            v->vector = NULL;
        }
        free(v);
        v = NULL;
    }
}

int stringCompare(const void *a, const void *b)
{
    char *s1 = (char *) a;
    char *s2 = (char *) b;
    return strcmp(s1, s2);
}

int concatenate(const void *word, void *pConcatenated)
{
    char *str = (char *) word;
    if (str == NULL)
    {
        return 0;
    }
    char *dest = strcat(pConcatenated, str);
    if (dest == NULL)
    {
        return 0;
    }
    dest = strcat(pConcatenated, "\n");
    if (dest == NULL)
    {
        return 0;
    }
    return 1;
}

void freeString(void *s)
{
    char *str = (char *) s;
    if (s != NULL)
    {
        free(str);
        str = NULL;
    }
}

