#include "syscall.h"

int main()
{
    int result, fileID;
    char buffer[20];
    PrintString("Enter name length and namefile\n");
    ReadString(buffer, ReadNum());
    fileID = OpenF(buffer);
    CloseFile(fileID);
    result = RemoveFile(buffer);
    if(result == -1)
        PrintString("File is open so we cannot remove\n");
    if(result == 0)
        PrintString("Remvove fail || Cannot find file\n");
    if(result == 1)
        PrintString("Remove successful\n");

    Halt();


}