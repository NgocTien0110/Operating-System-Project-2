#include "syscall.h"


int main()
{
    int result, fileID, length;
    char buffer[255];
    char content[300];
    //fileID = OpenF("test.txt");
    PrintString("Enter the length file and file name\n");
    ReadString(buffer, ReadNum());
    fileID = OpenF(buffer);
    if(fileID == -1)
    {
        PrintString("Open file fail\n");
        Halt();
    }
    result = ReadFile(content, 299, fileID);
    if(buffer[0] == '\0')
    {
        PrintString("Read a empty file\n");
        Halt();
    }
    if(result == -1)
    {
        PrintString("Read file fail\n");
        Halt();
    }
    PrintString("\tContent of file: \n");
    PrintString(content);
    PrintChar('\n');
    CloseFile(fileID);
    PrintChar('\n');
    PrintChar('\t');
    Halt();
}