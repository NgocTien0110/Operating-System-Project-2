#include "syscall.h"


int main()
{
    int a;
    PrintString("Enter the number: \n");
    PrintNum(ReadNum());
    PrintChar('\n');
    Halt();
}