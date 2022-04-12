#include "syscall.h"


int main()
{
    char buffer[255];
    int count = 0, fileID, length, result;

    fileID = Open("test.txt");
    
    Seek(-1, fileID);
    result = Read(buffer, 254, fileID);
    if(buffer[0] == '\0')
        PrintString("Read Empty File");
    while(buffer[count] != '\0') ++count;
    PrintString("Read ");
    PrintNum(count);
    PrintString("characters: ");
    PrintChar('\n');
    PrintString(buffer);
    PrintString("\n");
    Close(fileID);

    Halt();

}