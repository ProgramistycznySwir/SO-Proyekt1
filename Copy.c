#include "Copy.h"

#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "UtilityFunctions.h"

#define COPYING_BUFFER_SIZE 65536
int CopyFile(char* fileName_source, char* fileName_target)
{
    // syslog(LOG_NOTICE, "Copying %s to %s...", fileName_source, fileName_target);
    int in = open(fileName_source, O_RDONLY);
    int out = open(fileName_target, O_WRONLY | O_CREAT);

    // Error handling (bit primitive, but!):
    if ( in == -1 )
    {
        // syslog(LOG_NOTICE, "Source file \"%s\" not found!", fileName_source);
        close(in);
        close(out);
        return 1;
    }
    if ( out == -1 )
    {
        // syslog(LOG_NOTICE, "Failed to target file \"%s\"", fileName_target);
        close(in);
        close(out);
        return 2;
    }

    // Copying file:

    unsigned char* buffer =
        (unsigned char*) malloc(sizeof(unsigned char) * COPYING_BUFFER_SIZE);

    while ( read(in, buffer, COPYING_BUFFER_SIZE) )
        write(out, buffer, COPYING_BUFFER_SIZE);

    close(in);
    close(out);

    free(buffer);

    // Setting privilages and modification time:
    EqualizePrivilages(fileName_source, fileName_target);
    EqualizeModificationTime(fileName_source, fileName_target);

    // syslog(LOG_NOTICE, "Successfully copied file.");
    return 0;
}
