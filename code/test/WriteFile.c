#include "syscall.h"

int main()
{
    int result;
    int fileID;
    char buffer[255];

    fileID = Open("test.txt");
    Close(fileID);
    result = Open("test.txt");
    PrintNum(result);
    //result = Seek(0, fileID);
    //if(result == -1)
      //  PrintString("Not permit");
    //result = Remove("test.txt");
    // if(result != -1)
    // {
    //     PrintString("file openning");
    //     Halt();
    // }
    // PrintString("Please enter the content of file: \n");
    // Read(buffer, 25, 0);
    // result = Write(buffer, 25, fileID);

    Halt();

}   