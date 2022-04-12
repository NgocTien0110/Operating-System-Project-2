#include "syscall.h"

#define MAX_ARRAY 100
int main()
{
    int length, direct;
    int i, arr[MAX_ARRAY + 1], temp, j;
    do
    {
        PrintString("Enter the length of array[length <= 100]: ");
        length = ReadNum();
        if (length < 0 || length > MAX_ARRAY)
            PrintString("Negative length of array was enterned. Please try again!\n");

    } while (length < 0 || length > MAX_ARRAY);

    PrintString("\tEnter the element of array \n");
   
    for (i = 0; i < length; i++)
    {
        PrintString("arr[");
        PrintNum(i);
        PrintString("]: ");
        arr[i] = ReadNum();
    }

    do
    {
        PrintString("\nEnter direction \n");
        PrintString("[1] INCREASE. \n[2] DECREASE. \n");
        direct = ReadNum();
        if (direct != 1 && direct != 2)
            PrintString("Negative selection was enterned. Please try again!\n");
    } while (direct != 1 && direct != 2);

    for (i = 0; i < length; i++)
    {
        for (j = 0; j < length - 1; j++)
        {
            if (direct == 1)
            {
                if (arr[j] > arr[j + 1])
                {
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
            else if (direct == 2)
            {
                if (arr[j] < arr[j + 1])
                {
                    temp = arr[j];
                    arr[j] = arr[j + 1];
                    arr[j + 1] = temp;
                }
            }
        }
    }
    PrintString("Sorted array: ");
    for (i = 0; i < length; i++) {
        PrintNum(arr[i]);
        PrintChar(' ');
    }
    PrintChar('\n');
    Halt();
}