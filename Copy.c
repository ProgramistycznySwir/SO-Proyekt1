#include "Copy.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h> // for linux functions

#include <string.h> // for memcpy
#include <stdio.h>
#include <sys/mman.h> // for mmap

#include <syslog.h> // for logging

#include "UtilityFunctions.h"

static int CopyUsingReadWrite(char* fileName_source, char* fileName_target, long int bufferSize);
static int CopyUsingMMapWrite(char* fileName_source, char* fileName_target, long int fileSize);

/// Copies file from one filepath (source) to another (target).
int CopyFile(char* fileName_source, char* fileName_target, int mmapThreshold)
{
    struct stat fileStat;
    stat(fileName_source, &fileStat);
    long int fileSize = fileStat.st_size;

    int copyingStatus;
    if(fileSize > mmapThreshold)
        copyingStatus = CopyUsingMMapWrite(fileName_source, fileName_target, fileSize);
    else
        copyingStatus = CopyUsingReadWrite(fileName_source, fileName_target, fileSize);

    if(copyingStatus != EXIT_SUCCESS)
        return copyingStatus;

    // Setting privilages and modification time:
    EqualizePrivilages(fileName_source, fileName_target);
    EqualizeModificationTime(fileName_source, fileName_target);

    // syslog(LOG_NOTICE, "Successfully copied file.");
    return EXIT_SUCCESS;
}


#define MAX_COPYING_BUFFER_SIZE 65536
static int CopyUsingReadWrite(char* fileName_source, char* fileName_target, long int bufferSize)
{
    // To prevent creation of LHO.
    if(bufferSize > MAX_COPYING_BUFFER_SIZE)
        bufferSize = MAX_COPYING_BUFFER_SIZE;

    int in = open(fileName_source, O_RDONLY);
    int out = open(fileName_target, O_WRONLY | O_CREAT);

    // Error handling (bit primitive, but!):
    if ( in == -1 )
    {
        close(in);
        close(out);
        return EXIT_FAILURE;
    }
    if ( out == -1 )
    {
        close(in);
        close(out);
        return EXIT_FAILURE;
    }

    // Copying file:

    unsigned char* buffer =
        (unsigned char*) malloc(sizeof(unsigned char) * bufferSize);

    while ( read(in, buffer, bufferSize) )
        write(out, buffer, bufferSize);

    close(in);
    close(out);

    free(buffer);
    return EXIT_SUCCESS;
}

// Mostly sourced from: https://developpaper.com/using-mmap-to-copy-large-files-single-process-and-multi-process/
static int CopyUsingMMapWrite(char* fileName_source, char* fileName_target, long int fileSize)
{
    //Open target file
    //Calculate the size (in bytes) of the source file
    // unsigned long fileSize = get_file_byte_num(fileName_source);
    printf ("the length of the file is %ld bytes \n", fileSize);
    //--------Establish MMAP mapping area--------------
    //Get the file descriptor of the copied file
    int fd = open(fileName_source, O_RDWR|O_CREAT, 0644);
    int tfd = open(fileName_target, O_RDWR|O_CREAT, 0644);
    ftruncate(tfd, fileSize);

    char *mem =(char*) mmap(NULL, fileSize, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
    if (mem == MAP_FAILED)
    {
        syslog(LOG_NOTICE, "Map failed for file %s", fileName_source);
        close(fd);
        close(tfd);
        return EXIT_FAILURE;
    }
    char *tmem =(char*) mmap(NULL, fileSize, PROT_WRITE|PROT_READ, MAP_SHARED, tfd, 0);
    if (tmem == MAP_FAILED)
    {
        syslog(LOG_NOTICE, "Map failed for file %s", fileName_target);
        close(fd);
        close(tfd);
        return EXIT_FAILURE;
    }

    close(fd);
    close(tfd);

    memcpy(tmem, mem, fileSize);

    //Recycle subprocesses, wait for copy to end
    munmap(mem, fileSize);
    munmap(tmem, fileSize);
    return EXIT_SUCCESS;
}
