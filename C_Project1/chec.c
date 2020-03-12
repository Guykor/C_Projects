//
// Created by Guy on 11/14/2019.
//

#include <stdio.h>

unsigned int countSubStr(const char *str1, const char *str2, int isCyclic)
{
    int c1 = 0;
    int i = c1;
    int c2 = 0;
    unsigned int counter = 0;
    int cycled = 0;

    while (str1[c1] != '\0' || str2[c2] != '\0')
    {
        if (str2[c2] == '\0')
        {
            ++counter;
            c2 = 0;
            c1++;
            i = c1;
        }
        else if (str1[c1] == '\0' || str1[i] == '\0')
        {
            if (isCyclic == 0 || cycled == 1)
            {
                return counter;
            }
            c1 = 0;
            i = c1;
            cycled = 1;
        }
        if (str1[i] != str2[c2])
        {
            c2 = 0;
            c1++;
            i = c1;
        }
        else if (str1[i] == str2[c2] && str2[c2] != '\0')
        {
            i++;
            c2++;
        }
    }
    return ++counter;
}



int main()
{
    printf("%u\n", countSubStr("aaa", "aa", 1));
}