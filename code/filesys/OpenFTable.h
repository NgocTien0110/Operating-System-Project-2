#ifndef OPENFTABLE_H
#define OPENFTABLE_H
#include "openfile.h"
#include "sysdep.h"
#include "debug.h"

/*
[XIN CHAO THAY CO] 
- DAY LA FILE MA DUOC NHOM 4 CHUNG EM TAO RA DE QUAN LY CAC FILE DANG DUOC OPEN.
- LY DO TAO FILE NAY LA BOI VI 1 PHAN DE QUAN LY 1 PHAN LA DE PHUC VU CHO VIEC TRA VE OPENFILEID TRONG SYSCALL OPEN
- TUY GOI LA FILE MOI NHUNG CHUNG EM CUNG THUA KE NHUNG HAM CO TRONG FILESYSTEM.H VA OPENFILE.H NEN MUC DO PHUC TAP CUNG DUOC GIAM XUONG
DANG KE
- DUOI DAY LA CAC DEFINE CAN THIET CHO FILE CUA CHUNG EM 

*/


#define FILE_MAX 20 // DONG NAY SE DEFINE SO PHAN TU TOI DA CUA MANG OPENFILE *, TUC LA SO PHAN TU MA MINH CO THE MO LEN TOI DA ^ ^
#define CONSOLE_IN 0 // DONG NAY DE PHUC VU CHO VIEC THAO TAC CUA CONSOLE, NO LA VI TRI TRONG DAU TIEN TRONG MANG, SU DUNG CHO CONSOLEINPUT
#define CONSOLE_OUT 1 // DONG NAY LA VI TRI TRONG THU 2 TRONG MANG, SU DUNG CHO CONSOLEOUTPUT

/*
- DUOI DAY LA CLASS CHUA 1 MANG CON TRO OPENFILE *. 
- CHUA CAC HAM DE THAO TAC VOI DOI VOI MOI PHAN TU, TUC LA DOI VOI MOI FILE.
*/

class TableOFile
{
private:
    /*
    DAY LA MANG CON TRO, MOI CON TRO SE TUONG UNG VOI 1 FILE.
    */
    OpenFile **Table;
public:
    /*
    DAY LA CONSTRUCTOR CUA CLASS. DON THUAN CHI LA KHOI TAO MOT MANG CON TRO THUIIII
    */
    TableOFile()
    {
        Table = new OpenFile*[FILE_MAX];
    }
    /*
    CO VAY THI PHAI CO TRA !
    DAY LA DESTRUCTOR CUA CLASS, NHAM GIAI PHONG BO NHO DA DUOC CAP PHAT
    */
    ~TableOFile()
    {
        for(int i = 0; i < FILE_MAX; i++)
            if(Table[i])
                delete Table[i];
        delete [] Table;
    }
    /*
    VAY MUON XONG THI PHAI SU DUNG 
    DUOI DAY LA HAM THEM PHAN TU VAO MANG CON TRO NEU NO CON TRONG
    */
    int InsertF(char *namef)
    {
        int freeIn = -1;
    // KIEM TRA THU LA CON CHO TRONG HAY KHONG
        for(int i = 2; i < FILE_MAX; i++)
        {
            if(Table[i] == NULL)
            {
                freeIn = i;
                break;
            }
        }
        // TOI DAY LA BIET HET CHO TRONG RUIII NHA THAYYYY
        if(freeIn == -1)
        {
            DEBUG(dbgSys, "NOT FREE POSITION IN TABLE");
            return -1;
        }
        /* TOI CHO NAY LA TUI EM CHO NO LAY CAI SECTOR TRONG DISK NHA THAYYY
        VI THAY THUONG SINH VIEN NEN TUI EM DUNG LUON MODE READVOIWRITE LUON NHAAA THAYYY
        */
        int fileDescriptor;
        fileDescriptor = OpenForReadWrite(namef, FALSE);
        /*
        DA TOI DAY LA KHI MINH LAY CAI SECTOR TRONG DISK KHONG CO DUOC
        CO THE LA DO CAI FILE NAY CHUA DUOC TAO RA HOAC LA DO NO LAY FAIL NHA THAY.!
        */
        if(fileDescriptor == -1)
            return -1;
        
        /*
        DA TOI DAY THI LA TUI EM DA CO VI TRI, CO LUON SECTOR TRONG DISK DE KHOI TAO CHO CON TRO OPENFILE
        */
        Table[freeIn] = new OpenFile(fileDescriptor);
        /*
        DA CHO DAY LA MOT DIEM DAC BIET, TUI EM SE GAN THEM TEN CUA NO DE PHUC VU CHO CAC SYSCAL KHAC, NHU LA OPEN, CLOSE.
        */
        Table[freeIn]->getName(namef);
        // TRA VE INDEX
        return freeIn;
    }
    /*
    DA HAM DAY TRA VE VI TRI CUA FILE, LAY VAO THAM SO LA TEN FILE
    KIEM TRA XEM CO FILE KHONG, NEU CO TRA VE INDEX
    KHONG CO TRA VE -1 
    */
    int Opened(char *name)
    {
        for(int i = 2; i < FILE_MAX; i++)
            if(Table[i] != NULL)
            {
                if(strcmp(Table[i]->name, name) == 0)
                    return i; 
            }
        DEBUG(dbgSys, "NOT FOUND FILE IN TABLE");
        return -1;
    }
    /*
    HAM NAY THUC HIEN VIEC DONG FILE, DON GIAN LA XAC DINH TABLE[INDEX] VA GIAI PHONG VI TRI DO DI THUIII
    */
    int Close(int index)
    {
        if(index < 2 || index >= FILE_MAX)
        {
            return -1;
            DEBUG(dbgSys, "INDEX OUT OF RANGE");
        }
        else 
        {
            if(Table[index] != NULL)
            {
                delete Table[index];
                Table[index] = NULL;
                return 0;
            }
            /*
            CHO NAY LA VI TRI TRONG BANG LA NULL NEN TUI EM GHI NO LA NULL
            */
            DEBUG(dbgSys, "TABLE IN POSITION IS NULL");
            return -1;
        }

    }
    /*
    HAM NAY LA HAM DOC FILE
    NHAN VAO MOT BUFFER, SIZE CUA BUFFER VA ID CUA FILE TRONG BANG TABLE
    */
    int Readf(char *buffer, int size, int fileID)
	{
        /*
        TRUONG HOP INDEX OUT OF RANGE
        */
		if(fileID >= FILE_MAX || fileID < 0)
        {
            DEBUG(dbgSys, "INDEX OUT OF RANGE");
			return -1;
        }
        /*
        TRUONG HOP TAI VI TRI NAY THI FILE LA NULL
        */
		if(Table[fileID] == NULL)
        {
            DEBUG(dbgSys, "TABLE IN POSITION IS NULL");
			return -1;
        }
		int result;

		result = Table[fileID]->Read(buffer, size);
        return result;

	}
    /*
    HAM NAY SE TUONG TU HAM READ
    */
    int Writef(char *buffer, int size, int fileID)
    {
        if(fileID >= FILE_MAX || fileID < 0)
        {
            DEBUG(dbgSys, "INDEX OUT OF RANGE");
            return -1;
        }
            
        if(Table[fileID] == NULL)
        {
            DEBUG(dbgSys, "TABLE IN POSITION IS NULL");
            return -1;
        }

        int result;
        result = Table[fileID]->Write(buffer, size);

        return result;

    }
    /*
    HAM NAY LA DI CHUYEN CON TRO TOI MOT VI TRI BAT KI TRONG FILE
    */
    int Seekf(int position, int fileID)
    {
        // XU LI TRUONG HOP OUT OF RANGE
        if(fileID >= FILE_MAX || Table[fileID] == NULL)
        {
            DEBUG(dbgSys, "INDEX OUT OF RANGE");
            return -1;
        }
        // Handle call Seek in console ^_^
        if(fileID == 0 || fileID == 1)
        {
            DEBUG(dbgSys, "CALL SEEK IN CONSOLE");
            return -1;
        }
        int length;
        length = Table[fileID]->Length();
        // Jump to over length then return -2
        if(position > length)
            return -2;
        // Set position to the end of file ^_^
        if(position == -1)  
            position = length;
        // Set the current pointer within file to the position respective
        Table[fileID]->Seek(position);

        return position;
    }
    /*
    HAM NAY CHECK XEM FILE CO DANG TON TAI TRONG BANG TABLE HAY KHONG
    NEU CO TRA VE TRUE, NGUOC LAI LA FALSE
    */
    bool isOpen(char *name)
    {
        for(int i = 2; i < FILE_MAX; i++)
        {
            if(Table[i] != NULL)
            {
                // If file is openning we cannot remove it 
                if(strcmp(Table[i]->name, name) == 0)
                {
                    return TRUE;
                }
  
            }
        }
        DEBUG(dbgSys, "DONT EXSIT IN THE TABLE");
        return FALSE;
      
    }

};


#endif // OPENFTABLE
