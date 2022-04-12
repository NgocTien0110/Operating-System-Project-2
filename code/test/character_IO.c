#include "syscall.h"

int main()
{
    PrintChar(ReadChar());
    PrintChar('\n');
    Halt();
}