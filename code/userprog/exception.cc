// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "filesys.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

//----------------------[INCREASE PC REGISTER]----------
void IncreasePC()
{
	/* set previous programm counter (debugging only)
	 * similar to: registers[PrevPCReg] = registers[PCReg];*/
	kernel->machine->WriteRegister(PrevPCReg,
								   kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction
	 * similar to: registers[PCReg] = registers[NextPCReg]*/
	kernel->machine->WriteRegister(PCReg,
								   kernel->machine->ReadRegister(NextPCReg));

	/* set next programm counter for brach execution
	 * similar to: registers[NextPCReg] = pcAfter;*/
	kernel->machine->WriteRegister(
		NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

//------------------[SWITCH USER MEMORY TO KERNEL MEMORY]------------------------
char* User2Sys(int addr, int convert = -1) 
{
    int length = 0; // [LENGTH BUFFER.]
    bool stop = false; // 
    char* str;
	// ------[WHILE FOLLOW GET THE LENGTH OF THE STRING.]
    do 
	{
        int oneChar;
        kernel->machine->ReadMem(addr + length, 1, &oneChar);
        length++;
        stop = ((oneChar == 0 && convert == -1) || length == convert); // [CONDITION THE END OF LOOP.]
    } 
	while (!stop);

    str = new char[length]; // [ALLOCATE THE MEMORY TO GET]

    for (int i = 0; i < length; i++) {
        int oneChar;
        kernel->machine->ReadMem(addr + i, 1,&oneChar);  // [COPY 1 CHARATER AND SAVE TO THE BUFFER.]
        str[i] = (unsigned char)oneChar; // [THE NUMBER IS SAVED IN THE REGISTER SO WE MUST CAST TO CHARACTER AND SAVE TO THE BUFFER.]
    }
    return str;
}


//-------------[SWITCH SYSTEM MEMORY TO USER MEMORY]-------------------------------------
void Sys2User(char* str, int addr, int convert_length = -1) 
{
    int length;

	if(convert_length == -1) // [THE CODITION SURE THE CONVER_LENGTH ALWAYS > 0]
		length = strlen(str);
	else
		length = convert_length;

    for (int i = 0; i < length; i++) 
	{
        kernel->machine->WriteMem(addr + i, 1,str[i]);  // [WRITE TO THE BUFFER OF USER]
    }
    kernel->machine->WriteMem(addr + length, 1, '\0'); // [PUT THE END OF STRING.]
}


/*-------------------[HANDLE SYSCALL]----------------------
- WE WILL CALL THE HANDLE FUNCTION IN FILE KSYCALL.H 
*/


//-------------------[HANDLE SYSCALL HALT]----------------------------------
void HD_SC_Halt()
{
	DEBUG(dbgSys, "Shutdown, initiated by user program.\n");
	SysHalt();
	ASSERTNOTREACHED();
}

//--------------------[HANDLE SYSCALL ADD INTEGER NUMBER]---------------------
void HD_SC_Add()
{
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
					/* int op2 */ (int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);

	/* Modify return point */
	return IncreasePC();
}

//--------------------[HANDLE SYSCALL READ INTEGER NUMBER]---------------------
void HD_SC_ReadNum()
{
	int result = SysReadNum();
	kernel->machine->WriteRegister(2, result);
	return IncreasePC();
}

//--------------------[HANDLE SYSCALL PRINT INTEGER NUMBER]---------------------
void HD_SC_PrintNum()
{
	int integer = kernel->machine->ReadRegister(4);
	SysPrintNum(integer);
	return IncreasePC();
}

//--------------------[HANDLE SYSCALL READ CHARACTER]---------------------
void HD_SC_ReadChar()
{
	char result = SysReadChar();
	kernel->machine->WriteRegister(2, (int)result);
	return IncreasePC();
}

//--------------------[HANDLE SYSCALL PRINT CHARACTER]---------------------

void HD_SC_PrintChar() // print character
{
	char result = (char)kernel->machine->ReadRegister(4);
	SysPrintChar(result);
	return IncreasePC();
}


//--------------------[HANDLE SYSCALL RANDOM INTEGER]---------------------
void HD_SC_RamdonNum()
{
	int result = SysRandomNum();
	kernel->machine->WriteRegister(2, result);
	return IncreasePC();
}

//--------------------[HANDLE SYSCALL READ STRING ]---------------------
void HD_SC_ReadString()
{
	/*
	[EXPLAIN]: 
	1. VARRIABLE [ADDRESS] HAVE TYPE INT BECASUE THE REGISTER JUST CONTAIN THE INTEGER
	2. WHEN WE GET THE [ADDRESS], IT BELONG TO USER SPACE SO WE MUST SWITCH USERSPACE TO KERNEL SPACE.
	3. WE CREAT A BUFFER TO GET STRING FROM TERMINAL (KERNEL SPACE) AND SAVE IN BUFFER IN USER SPACE.
	4. AFTER WE SWITCH, WE MUST DELOCATE THE BUFFER IN KERNEL SPACE.
	*/
	int address = kernel->machine->ReadRegister(4);
	int length = kernel->machine->ReadRegister(5);
	if (length > MAX_STRING_LEN) // [CHECK LIMIT OF LENGTH OF STRING]
	{
		DEBUG(dbgSys, "String length out of range " << MAX_STRING_LEN);
		SysHalt();
	}
	// [READ STRING]
	char *buffer = SysReadString(length);
	// [SWITCH SYSTEM SPACE TO USER SPACE]
	Sys2User(buffer, address, strlen(buffer));
	// [DELOCATE THE BUFFER]
	delete[] buffer;

	return IncreasePC();
}


//--------------------[HANDLE SYSCALL READ STRING ]---------------------
void HD_SC_PrintString()
{
	/*
	[EXPLAIN]:
	1.ADDRESS VARRIABLE [ADDRESS] HAVE TYPE INT BECASUE THE REGISTER JUST CONTAIN THE INTEGER
	2.IF WE WANT TO SAVE THE CHARACTER WHICH READ FROM THE TERMINAL WE MUST SWITCH THE SYSTEM SPACE TO USER SPACE.
	3.SIMILAR TO READSTRING() WE MUST DELOCATE THE BUFFER IN KERNEL SPACE.
	*/

	int address = kernel->machine->ReadRegister(4); // ADDRESS OF USER SPACE
	char *buffer = User2Sys(address); // THE BUFFER IN KERNEL SPACE
	
	//[CALL HANDLE SYSCALL IN FILE KSYCALL.H]
	SysPrintString(buffer, strlen(buffer));
	// DELOCATE THE BUFFER
	delete[] buffer;

	return IncreasePC();
}


void HD_SC_CreateFile()
{
	int address = kernel->machine->ReadRegister(4); // LAY DIA CHI VUNG NHO TU USER
	char *buffer = User2Sys(address); // CHUYEN NO VE BUFFER CUA HDH
	
	int result;
	result = SysCreateFile(buffer); // GOI DEN HAM DE XU LY


	delete[] buffer; // GIAI PHONG VUNG NHO
	kernel->machine->WriteRegister(2, result); // GHI KET QUA TRA VE

	return IncreasePC();
}
void HD_SC_OpenFile()
{
	int address = kernel->machine->ReadRegister(4); // LAY DIA CHI VUNG NHO TU USER
	char *buffer = User2Sys(address);// CHUYEN NO VE BUFFER CUA HDH

	int result;
	result = SysOpenFile(buffer); // GOI DEN HAM XU LY OPENFILE

	delete [] buffer;
	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	return IncreasePC();
}

void HD_SC_CloseFile()
{
	int index = kernel->machine->ReadRegister(4);

	int result;
	result = SysCloseFile(index); // GOI DEN HAM XU LY CLOSE FILE

	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	return IncreasePC();

}

void HD_SC_ReadFile()
{
	int address = kernel->machine->ReadRegister(4); // LAY DIA CHI VUNG NHO TU USER
	int size = kernel->machine->ReadRegister(5); // LAY SIZE CUA BUFFER
	int fileID = kernel->machine->ReadRegister(6); // LAY ID CUA FILE

	char *buffer;
	buffer = User2Sys(address, size);// CHUYEN NO VE BUFFER CUA HDH
	int result;
	result = SysReadFile(buffer, size, fileID); // GOI HAM XU LY

	Sys2User(buffer, address); // CHUYEN VE BUFFER CHO USER SAU KHI DOC DUOC
	
	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	delete [] buffer;
	return IncreasePC();
}

void HD_SC_WriteFile()
{
	int address = kernel->machine->ReadRegister(4);// LAY DIA CHI VUNG NHO TU USER
	int size = kernel->machine->ReadRegister(5); // LAY KICH THUOC MUON GHI
	int fileID = kernel->machine->ReadRegister(6); // LAY ID FILE

	char *buffer;
	buffer = User2Sys(address, size);// CHUYEN NO VE BUFFER CUA HDH

	int result;
	result = SysWriteFile(buffer, size, fileID); // GOI HAM XU LY
	Sys2User(buffer, address, size); // TRA VE BUFFER CHO USER


	delete [] buffer;
	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	return IncreasePC();
}

void HD_SC_SeekFile()
{
	int position = kernel->machine->ReadRegister(4); // LAY VI TRI 
	int fileID = kernel->machine->ReadRegister(5); // LAY ID FILE

	int result;
	result = SysSeekFile(position, fileID);

	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	return IncreasePC();

}

void HD_SC_RemoveFile()
{
	int address = kernel->machine->ReadRegister(4);// LAY DIA CHI VUNG NHO TU USER
	char *buffer;
	buffer = User2Sys(address);// CHUYEN NO VE BUFFER CUA HDH

	int result;

	result = SysRemoveFile(buffer); // GOI HAM XU LY

	delete [] buffer;
	kernel->machine->WriteRegister(2, result);// GHI KET QUA TRA VE

	

	return IncreasePC();
}
//------------------[CATCH EXCEPTION]-------------------------------------

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:
		DEBUG('u', "Switch to system mode\n");
		return;
		//break;
	case PageFaultException:
		DEBUG('a', "\nNo valid translation found.\n");
		printf("\n\nNo valid translation found.\n");
		SysHalt();
		break;
	case ReadOnlyException:
		DEBUG('a', "\nWrite attempted to page marked (read-only).\n");
		printf("\n\nWrite attempted to page marked (read-only).\n");
		SysHalt();
		break;
	case BusErrorException:
		DEBUG('a', "\nTranslation resulted in an invalid physical address.\n");
		printf("\n\nTranslation resulted in an invalid physical address.\n");
		SysHalt();
		break;
	case AddressErrorException:
		DEBUG('a', "\nUnaligned reference or one that was beyond the end of the address space.\n");
		printf("\n\nUnaligned reference or one that was beyond the end of the address space.\n");
		SysHalt();
		break;
	case OverflowException:
		DEBUG('a', "\nInteger overflow in add or sub.\n");
		printf("\n\nInteger overflow in add or sub.\n");
		SysHalt();
		break;
	case IllegalInstrException:
		DEBUG('a', "\nUnimplemented or reserved instr.\n");
		printf("\n\nUnimplemented or reserved instr.\n");
		SysHalt();
		break;
	case NumExceptionTypes:
		DEBUG('a', "\nNot numeric data.\n");
		printf("\n\nNot numeric data.\n");
		SysHalt();
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt: // [HALT SYSCALL]
			HD_SC_Halt();
			return;
		case SC_Add: // [ADD INTEGER SYSCALL]
			HD_SC_Add();
			return;
		case SC_ReadNum: // [READ INTEGER SYSCALL]
			HD_SC_ReadNum();
			return;
		case SC_PrintNum: // [PRINT INTEGER SYSCALL]
			HD_SC_PrintNum();
			return;
		case SC_ReadChar:
			HD_SC_ReadChar(); // [READ CHAR SYSCALL]
			return;
		case SC_PrintChar: // [PRINT CHAR SYSCALL]
			HD_SC_PrintChar();
			return;
		case SC_RandomNum: // [RANDOM INTEGER SYSCALL]
			HD_SC_RamdonNum();
			return;
		case SC_ReadString: // [READ STRING SYSCALL]
			HD_SC_ReadString();
			return;
		case SC_PrintString: // [PRINT STRING SYSCALL]
			HD_SC_PrintString();
			return;
		case SC_CreateFile:
			HD_SC_CreateFile();
			return;
		case SC_OpenFile:
			HD_SC_OpenFile();
			return;
		case SC_CloseFile:
			HD_SC_CloseFile();
			return;
		case SC_ReadFile:
			HD_SC_ReadFile();
			return;
		case SC_WriteFile:
			HD_SC_WriteFile();
			return;
		case SC_SeekFile:
			HD_SC_SeekFile();
			return;
		case SC_RemoveFile:
			HD_SC_RemoveFile();
			return;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}
