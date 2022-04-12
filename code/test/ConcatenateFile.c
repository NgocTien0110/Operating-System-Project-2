#include "syscall.h"


int main()
{
    int fileID1, result, length, fileID2;
    char bufferName1[32];
    char bufferName2[32];
    char content[255];
    PrintString("Enter length file and name first file\n");
    ReadString(bufferName1, ReadNum());
    PrintString("Enter length file and name second file\n");
    ReadString(bufferName2, ReadNum());
    ReadString(bufferName2, ReadNum());
    fileID1 = OpenF(bufferName1);
    fileID2 = OpenF(bufferName2);
    if(fileID1 == -1)
    {
        PrintString("Open file source concatenate fail\n");
        Halt();
    }
    length = ReadFile(content, 255, fileID2);
    if(content[0] == '\0')
    {
        PrintString("Read a empty concatenate source file\n");
        Halt();
    }
    if(length == -1)
    {
        PrintString("Read file concatenate source fail\n");
        Halt();
    }
    
    if(fileID2 == -1)
    {
        PrintString("Open file destination concatenate fail\n");
        Halt();
    }
    SeekFile(-1, fileID1);
    result = WriteFile(content, length, fileID1);

    if(result == -1)
        PrintString("Concatenate fail\n");
    else
        PrintString("Concatenate sucessful\n");
    CloseFile(fileID1);
    CloseFile(fileID2);
    Halt();

}