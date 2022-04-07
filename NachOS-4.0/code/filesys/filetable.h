#ifndef FILETABLE_H
#define FILETABLE_H
#include "openfile.h"
#include "sysdep.h"

#define FILE_MAX 10
#define CONSOLE_IN 0
#define CONSOLE_OUT 1
#define MODE_READWRITE 0
#define MODE_READ 1
#define MODE_WRITE 2

class FileTable
{
private:
    OpenFile **openFile;
    char **filename;
    int *fileOpenMode;

public:
    FileTable()
    {
        openFile = new OpenFile *[FILE_MAX];
        filename = new char *[FILE_MAX];
        fileOpenMode = new int[FILE_MAX];
        fileOpenMode[CONSOLE_IN] = MODE_READ;
        fileOpenMode[CONSOLE_OUT] = MODE_WRITE;
    }

    int Insert(char *fileName, int openMode)
    {
        int freeIndex = -1;
        int fileDescriptor = -1;
        for (int i = 2; i < FILE_MAX; i++)
        {
            if (openFile[i] == NULL)
            {
                freeIndex = i;
                break;
            }
        }

        if (freeIndex == -1)
        {
            return -1;
        }

        if (openMode == MODE_READWRITE)
            fileDescriptor = OpenForReadWrite(fileName, FALSE);
        // if (openMode == MODE_READ)
        //     fileDescriptor = OpenForRead(fileName, FALSE);

        if (fileDescriptor == -1)
            return -1;
        openFile[freeIndex] = new OpenFile(fileDescriptor);
        fileOpenMode[freeIndex] = openMode;
        filename[freeIndex] = fileName;

        return freeIndex;
    }

    bool IsOpeningFilename(char *fileName)
    {
        for (int i = 0; i < FILE_MAX; ++i)
            if (filename[i] == fileName && openFile[i] != NULL)
            {
                return true;
            }
        return false;
    }

    bool IsOpeningID(int fileId)
    {
        if (fileId < 2 || fileId >= FILE_MAX)
            return false;
        return (openFile[fileId] != NULL);
    }

    int Remove(int index)
    {
        if (index < 2 || index >= FILE_MAX)
            return -1;
        if (openFile[index])
        {
            delete openFile[index];
            openFile[index] = NULL;
            filename[index] = "";
            return 0;
        }
        return -1;
    }

    int Read(char *buffer, int charCount, int index)
    {
        if (index >= FILE_MAX)
            return -1;
        if (openFile[index] == NULL)
            return -1;
        int result = openFile[index]->Read(buffer, charCount);
        // if we cannot read enough bytes, we should return -2
        if (result != charCount)
            return -2;
        return result;
    }

    int Write(char *buffer, int charCount, int index)
    {
        if (index >= FILE_MAX)
            return -1;
        if (openFile[index] == NULL || fileOpenMode[index] == MODE_READ)
            return -1;
        return openFile[index]->Write(buffer, charCount);
    }

    int Seek(int pos, int index)
    {
        if (index <= 1 || index >= FILE_MAX)
            return -1;
        if (openFile[index] == NULL)
            return -1;
        // use seek(-1) to move to the end of file
        if (pos == -1)
            pos = openFile[index]->Length();
        if (pos < 0 || pos > openFile[index]->Length())
            return -1;
        return openFile[index]->Seek(pos);
    }

    ~FileTable()
    {
        for (int i = 0; i < FILE_MAX; i++)
        {
            if (openFile[i])
                delete openFile[i];
        }
        delete[] openFile;
        delete[] fileOpenMode;
    }
};

#endif
