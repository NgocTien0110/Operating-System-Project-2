// filesys.h
//	Data structures to represent the Nachos file system.
//
//	A file system is a set of files stored on disk, organized
//	into directories.  Operations on the file system have to
//	do with "naming" -- creating, opening, and deleting files,
//	given a textual file name.  Operations on an individual
//	"open" file (read, write, close) are to be found in the OpenFile
//	class (openfile.h).
//
//	We define two separate implementations of the file system.
//	The "STUB" version just re-defines the Nachos file system
//	operations as operations on the native UNIX file system on the machine
//	running the Nachos simulation.
//
//	The other version is a "real" file system, built on top of
//	a disk simulator.  The disk is simulated using the native UNIX
//	file system (in a file named "DISK").
//
//	In the "real" implementation, there are two key data structures used
//	in the file system.  There is a single "root" directory, listing
//	all of the files in the file system; unlike UNIX, the baseline
//	system does not provide a hierarchical directory structure.
//	In addition, there is a bitmap for allocating
//	disk sectors.  Both the root directory and the bitmap are themselves
//	stored as files in the Nachos file system -- this causes an interesting
//	bootstrap problem when the simulated disk is initialized.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#ifndef FS_H
#define FS_H

#include "copyright.h"
#include "sysdep.h"
#include "openfile.h"
#include "OpenFTable.h"

#define MANAGE_MAX 20
#ifdef FILESYS_STUB // Temporarily implement file system calls as
// calls to UNIX, until the real file system
// implementation is available
class FileSystem
{
public:
	// FileSystem() {}
	/*
	- TUI EM KHOI TAO MOT CAI MANG CO DANG LA TABLEOFFILE, MANG NAY THAT RA NO TRO TOI MOT CAI MANG DE QUAN LY FILE MA THOI
	- THAT RA THI TUI EM CUNG CO THE TAO MOT CAI MANG OPENFTABLE O DAY, NHUNG DE DAM BAO TINH LOGIC THI TUI EM PHAI 
	TAO MOT CON TRO O DAY ^_^ 
	
	*/
	TableOFile *ManageT;
	// DAY LA CONSTRUCTOR MA TUI EM VIET THEM DE KHOI TAO CHO CAI CON TRO CUA MINH
	FileSystem()
	{
		ManageT = new TableOFile;
	}
	// TUI EM GIAI PHONG NO KHI KHONG CON SU DUNG NUA
	~FileSystem()
	{
		delete ManageT;
	}
	
	bool Create(char *name)
	{
		int fileDescriptor = OpenForWrite(name);

		if (fileDescriptor == -1)
			return FALSE;
		Close(fileDescriptor);
		return TRUE;
	}
	
	OpenFile *Open(char *name)
	{
		int fileDescriptor = OpenForReadWrite(name, FALSE);

		if (fileDescriptor == -1)
			return NULL;

		return new OpenFile(fileDescriptor);
	}

	bool Remove(char *name) { return Unlink(name) == 0; }
	/*
	DAY LA HAM OPEN TUI EM TAO RA
	*/
	int Openf(char *nameF)
	{
		int result;
		result = ManageT->Opened(nameF);
		/*
		NEU MA RETURN -1 THI FILE DA TON TAI ROI MINH CHI VIEC RETURN LAI CAI INDEX DO MA THOI
		*/
		if (result != -1)
		{
			return result;
		}
		/*
		NEU CHUA TON TAI THI SAO? THI MINH PHAI THEM NO VAO BANG TABLE 
		*/
		return ManageT->InsertF(nameF);
	}
	/*
	DAY LA HAM DONG FILE CUA CHUNG EM
	GOI DEN HAM CLOSE TRONG OPENFTABLE.H
	*/
	int CloseFile(int index)
	{
		int result;
		result = ManageT->Close(index);

		return result;
	}
	/*
	DAY LA HAM READ, GOI DEN MOT HAM READF DA DUOC DINH NGHIA TRONG OPENFTABLE
	*/
	int ReadFile(char *buffer, int size, int fileID)
	{
		return ManageT->Readf(buffer, size, fileID);

	}
	/*
	TUONG TU READFLE
	*/
	int WriteFile(char *buffer, int size, int fileID)
	{
		return ManageT->Writef(buffer, size, fileID);
	}
	/*
	DI CHUYEN TOI 1 VI TRI BAT KI TRONG FILE
	SEEKF LA HAM CHUNG EM TAO RA TRONG OPENFTABLE NHAM GIAI QUYET MOT SO EXCEPTION 
	*/
	int SeekPosition(int position, int fileID)
	{
		return ManageT->Seekf(position, fileID);
	}
	/*
	VA CUOI CUNG THI DAY LA REMOVEFILE
	*/
	int RemoveFile(char *name)
	{
		if(ManageT->isOpen(name))
		{
			// Becasue the file is open so we can not remove it this time 
			return -1;
		}
		// Otherwise the file is not open so we can remove it from disk
		return Remove(name);
	}
	
};

#else // FILESYS
class FileSystem
{
public:
	TableOFile *ManageT;

	
	FileSystem()
	{
		ManageT = new TableOFile;
	}
	~FileSystem()
	{
		delete ManageT;
	}
	bool Create(char *name);
	FileSystem(bool format); // Initialize the file system.
							 // Must be called *after* "synchDisk"
							 // has been initialized.
							 // If "format", there is nothing on
							 // the disk, so initialize the directory
							 // and the bitmap of free blocks.

	bool Create(char *name, int initialSize);
	// Create a file (UNIX creat)

	OpenFile *Open(char *name); // Open a file (UNIX open)

	bool Remove(char *name); // Delete a file (UNIX unlink)

	void List(); // List all the files in the file system

	void Print(); // List all the files and their contents
	int Openf(char *name);
	int CloseFile(int index);
	int ReadFile(char *buffer, int size, int fileID);
	int WriteFile(char *buffer, int size, int fileID);
	int SeekPosition(int position, int fileID);
	int RemoveFile(char *name);
private:
	OpenFile *freeMapFile;	 // Bit map of free disk blocks,
							 // represented as a file
	OpenFile *directoryFile; // "Root" directory -- list of
							 // file names, represented as a file
};

#endif // FILESYS

#endif // FS_H
