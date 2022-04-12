#include "syscall.h"


char arr[255];
int main()
{
    
    PrintString("Enter string (length < 255) \n");
    ReadString(arr, ReadNum());

    PrintString(arr);
    PrintChar('\n');
    Halt();
}