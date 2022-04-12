#include "syscall.h"

int main()
{
    char *filename;

    int result;

    // ReadString(filename, ReadNum());
    result = CreateFile("Newfile.txt"); 
    if(result == -1)
        PrintString("Create file fail\n");
    if(result == -2)
        PrintString("File is exist\n");
    if(result == 0)
        PrintString("Successful\n");

    Halt();

}