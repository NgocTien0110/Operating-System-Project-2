#include"syscall.h"


int main()
{
    int fileID1, result, length, fileID2;
    char bufferName1[32];
    char bufferName2[32];
    char content[255];
    PrintString("Enter length file and name Source file\n");
    ReadString(bufferName1, ReadNum());
    PrintString("\nEnter length file and name Destination file\n");
    ReadString(bufferName2, ReadNum());
    ReadString(bufferName2, ReadNum());
    fileID1 = OpenF(bufferName1);
    fileID2 = OpenF(bufferName2);
    if(fileID1 == -1)
    {
        PrintString("Open file source fail\n");
        Halt();
    }
    length = ReadFile(content, 254, fileID1);
    if(content[0] == '\0')
    {
        PrintString("Read a empty file\n");
        Halt();
    }
    if(length == -1)
    {
        PrintString("Read file fail\n");
        Halt();
    }
    if(fileID2 == -1)
    {
        PrintString("Open file destination fail\n");
        Halt();
    }
    result = WriteFile(content, length, fileID2);

    if(result == -1)
        PrintString("Copy fail\n");
    else
        PrintString("Copy sucessful\n");

    CloseFile(fileID2);
    Halt();


}