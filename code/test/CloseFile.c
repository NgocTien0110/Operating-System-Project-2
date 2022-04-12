#include "syscall.h"


int main()
{
    int result, result1;
    // char filename[20];

    // ReadString(filename, ReadNum());
    // result = 10;
    //result = Open("input.txt");
    result = Close(result);

    if(result == 0)
        PrintString("Close file sucessful");

    else
        PrintString("Close file failure");

    PrintChar('\n');
    Halt();
}