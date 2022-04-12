/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__
#include "kernel.h"
#include "synchconsole.h"
#include "stdlib.h"
#include <stdint.h>

#define MAX_LEN 11
#define MAX_STRING_LEN 255
#define LF ((char)10)
#define CR ((char)13)
#define TAB ((char)9)
#define SPACE ((char)' ')

char NumberBuffer[MAX_LEN + 1]; // [NUMBER BUFFER]

//[CHECK CHARACTER] - USING FOR READ NUMBER
char isBlank(char c) // LF: ki tu chuyen dong, CR: Quay lai dau dong, TAB: tab, SPACE: space
{
  return c == LF || c == CR ||
         c == TAB || c == SPACE;
}

//----------------------------------[READ NUMBER]--------------------------------------------
void readUntilSpace()
{
  memset(NumberBuffer, 0, sizeof(NumberBuffer));
  char c = kernel->synchConsoleIn->GetChar();
  if (c == EOF) // [HANDLE IF C IS THE END OF FILE]
  {
    DEBUG(dbgSys, "Unexpected end of file - number expected");
    return;
  }
  if (isBlank(c)) // [HANDLE IF C IS LF, CR, TAB, SPACE]
  {
    DEBUG(dbgSys, "Unexpected white-space - number expected");
    return;
  }
  //-----[START READ]-----------
  int index = 0;
  while (!isBlank(c) || c == EOF)
  {
    NumberBuffer[index++] = c;
    if (index > MAX_LEN)
    {
      DEBUG(dbgSys, "Number is too long");
      return;
    }
    c = kernel->synchConsoleIn->GetChar();
  }
}

bool Compare(int arg, char *s)
{
  /*
  [CASE 1]:
  IF ARG = 0 AND S[] = {0} => RETURN 1 BECAUSE IT IS NOT OVER FLOW
  IF ARG = 0 AND S[] != {0} => OVERFLOW TAKE PLACE.EXAMPLE:
  USIGN INT A = INTMAX32 + 1 => A = 0
  */
  if (arg == 0)
    return strcmp(s, "0") == 0;

  int len = strlen(s);
  /*
  [CASE 2]:
  IF ARG < 0 AND S[0] != '-' => OVER FLOW TAKE PLACE
  EXAMPLE: INT A = INT32MAX + 1 => A = INT32MIN
  */
  if (arg < 0 && s[0] != '-')
    return false;
  /*
  [CASE 3]:
  IF ARG < 0, WE MAKE IT SIMPLE BY MAKE IT TO POSITIVE NUMBER AND COMPARE.
  */
  if (arg < 0)
    s++, len--, arg = -arg;
  //[COMPARE]
  while (arg > 0)
  {
    int digit = arg % 10;

    if (s[len - 1] - '0' != digit)
      return false;

    --len;
    arg /= 10;
  }

  return len == 0;
}

int SysReadNum()
{
  readUntilSpace(); //[READ TO NUMBER BUFFER]
  // [GET LENGTH OF BUFFER]
  int len = strlen(NumberBuffer);
  if (len == 0)
    return 0;

  if (strcmp(NumberBuffer, "-2147483648") == 0) // We must check it first because INT32_MAX = abs(INT32_MIN) - 1
    return INT32_MIN;

  bool negative = (NumberBuffer[0] == '-');
  int numZero = 0;   // Count the number of zero
  bool state = true; // continue if the number still have 0
  int number = 0;

  for (int i = negative; i < len; i++)
  {
    char temp = NumberBuffer[i];
    if (temp == '0' && state)
      numZero++;
    else
      state = false;
    //[CHECK IF THE INPUT IS NOT NUMERIC]
    if (temp < '0' || temp > '9')
    {
      DEBUG(dbgSys, "Expected number but " << NumberBuffer << " found");
      return 0;
    }
    // TRANSFER TO NUMBER
    number = number * 10 + (temp - '0');
  }
  // [HANDLE CASE 00 01 -0]
  if (numZero > 1 || (numZero && (number || negative)))
  {
    DEBUG(dbgSys, "Expected number but " << NumberBuffer << " found");
    return 0;
  }
  //[RETURN SIGN TO NUMBER]
  if (negative)
    number = -number;
  //[WE SURE THE NUMBER IS GOOD TO RETURN]
  if (len <= MAX_LEN - 2)
    return number;
  //[CHECK IF THE NUMBER IS TOO LONG OR TOO SHORT]
  if (Compare(number, NumberBuffer))
    return number;
  else
    DEBUG(dbgSys, "Expected int32 number but " << NumberBuffer << " found");
  return 0;
}
//--------------------------------------------------------------------------------
//---------------[SYSCALL ADD]---------------
int SysAdd(int op1, int op2)
{
  return op1 + op2;
}
//-------------------------------------------------
//---------------[SYSCALL READ CHAR]---------------
char SysReadChar()
{
  return kernel->synchConsoleIn->GetChar(); // Get character through kernel mode
}

//-------------------------------------------------
//---------------[SYSCALL PRINT CHAR]---------------
void SysPrintChar(char c)
{
  return kernel->synchConsoleOut->PutChar(c); // Print character through kernel mode
}

//-------------------------------------------------
//---------------[SYSCALL SYSHALT]---------------
void SysHalt()
{
  kernel->interrupt->Halt();
}
//-------------------------------------------------
//---------------[SYSCALL PRINT NUM]---------------
void SysPrintNum(int arg)
{
  if (arg == 0)
  {
    kernel->synchConsoleOut->PutChar('0'); // case error
  }
  if (arg == INT32_MIN) // case INT32 MIN
  {

    kernel->synchConsoleOut->PutChar('-');
    for (int i = 0; i < 10; ++i)
      kernel->synchConsoleOut->PutChar("2147483648"[i]); // in theo thu tu
    return;
  }
  // case arg < 0
  if (arg < 0)
  {
    kernel->synchConsoleOut->PutChar('-');
    arg = -arg;
  }
  int n = 0; // length
  while (arg)
  {
    NumberBuffer[n++] = arg % 10; // lay tung ki tu luu vao mang
    arg /= 10;
  }

  for (int i = n - 1; i >= 0; --i)
    kernel->synchConsoleOut->PutChar(NumberBuffer[i] + '0'); // in ra
}
//-------------------------------------------------
//---------------[SYSCALL RANDOM NUM]---------------
int SysRandomNum()
{
  return random();
}
//-------------------------------------------------
//---------------[SYSCALL READ STRING]---------------
char *SysReadString(int length)
{
  char *buffer = new char[length + 1];
  // for (int i = 0; i < length; i++)
  // {
  //   buffer[i] = kernel->synchConsoleIn->GetChar();
  // }
  kernel->synchConsoleIn->GetString(buffer, length);
  buffer[length] = '\0';

  return buffer;
}
//-------------------------------------------------
//---------------[SYSCALL PRINT STRING]---------------
void SysPrintString(char *buffer, int length)
{
  for (int i = 0; i < length; i++)
    kernel->synchConsoleOut->PutChar(buffer[i]);
}
#endif /* ! __USERPROG_KSYSCALL_H__ */
/*
HAM GIAI QUYET CREATEFILE 
*/
int SysCreateFile(char *buffer)
{
  int length = strlen(buffer);
  // DAY LA BUFFER CO LENGTH = 0 - MOT EXCEPTION
  if (length == 0)
  {
    DEBUG(dbgSys, "The length of buffer is a negative number");
    return -1;
  }
  // DAY LA LOI CAP PHAT VUNG NHO
  if (buffer == NULL)
  {
    DEBUG(dbgSys, "Allocate memory fail");
    return -1;
  }
  // DAY LA FILE DA TON TAI
  if (kernel->fileSystem->Open(buffer) != NULL)
  {
    DEBUG(dbgSys, "File is exist");
    return -2;
  }
  // DAY LA TAO FILE RUIIII BUTTT NO BI FALSE LA NO KHONG CO TAO RA DUOC
  if (kernel->fileSystem->Create(buffer) == FALSE)
  {
    DEBUG(dbgSys, "Fail create file");
    return -1;
  }
  // SAY OHH YEAHH NO DA DUOC TAO RA THANH CONG RUIII
  return 0;
}
// DA DUNG NHU CAI TEN THI DAY LA OPENFILE A.
// TU NHUNG HAM NAY TRO XUONG DA SO SE DUA VAO FILESYSTEM DE XU LY NEN O DAY NHIN RAT LA SIMPLE
int SysOpenFile(char *buffer)
{
  int result;
  result = kernel->fileSystem->Openf(buffer);

  return result;
}
// CON DAY LA DONG FILE 
int SysCloseFile(int index)
{
  int result;
  result = kernel->fileSystem->CloseFile(index);

  return result;
}
// DAY LA READ FILE
int SysReadFile(char *buffer, int size, int fileID)
{
  /*
  KHI FILEID = 0 TUC LA NO GOI RA DE LAY NOI DUNG TU CONSOLE
  */
  if (fileID == 0)
  {
    DEBUG(dbgSys, "CALL INPUT IN SYNCONSOLE");
    return kernel->synchConsoleIn->GetString(buffer, size);
  }
  
  return kernel->fileSystem->ReadFile(buffer, size, fileID);
}
// DAY LA DOC FILE
int SysWriteFile(char *buffer, int size, int fileID)
{
  /*
  DAY LA FILEID == 1 NEN NO GOI RA DE DAT NOI DUNG CUA BUFFER RA OUTPUT
  */
  if (fileID == 1)
  {
    DEBUG(dbgSys, "CALL OUTPUT IN SYNCONSOLE");
    return kernel->synchConsoleOut->PutString(buffer, size);
  }
  
  return kernel->fileSystem->WriteFile(buffer, size, fileID);
}
// DAY LA SEEKFILE
int SysSeekFile(int position, int fileID)
{
  return kernel->fileSystem->SeekPosition(position, fileID);
}
// DAY LA REMOVE FILE
int SysRemoveFile(char *name)
{
  return kernel->fileSystem->RemoveFile(name);
}