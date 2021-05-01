#include "UtilityFunctions.h"

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> // for optionals
#include <syslog.h> // for logging
#include <dirent.h> // for dirent
#include <string.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <utime.h> // for updating time

void SimpleLog(char* message)
    { syslog (LOG_NOTICE, "%s", message); }

char DoesDirectoryExistsAt(char* directoryPath)
{
    DIR* directoryToCheck = opendir(directoryPath);

    if (directoryToCheck == NULL )
        return 0;

    closedir(directoryToCheck);
    return 1;
}

char DoesFileExistsAt(char* filePath)
{
    int fileToCheck = open(filePath, O_WRONLY);

    if (fileToCheck <0)
        return 0;

    close(fileToCheck);
    return 1;
}

/// return: File1.ModificationTime - File2.ModificationTime
char CompareModificationTimeOfFiles(char* filePath1, char* filePath2)
{
    struct stat file1_stat, file2_stat;
    stat(filePath1, &file1_stat);
    stat(filePath2, &file2_stat);
    return file1_stat.st_mtime - file2_stat.st_mtime;
}

//TODO_CLEAN: Cleanup commented lines.
/// Literally just sets receiver.ModificationTime = donor.ModificationTime
void EqualizeModificationTime(char* donorPath, char* receiverPath)
{
    struct stat donor_stat;
    // time_t modificationTime;
    struct utimbuf newTime;

    stat(donorPath, &donor_stat);
    // modificationTime = donor_stat.st_mtime;

    // new_times.actime = donor_stat.st_atime;
    newTime.modtime = donor_stat.st_mtime;
    // Update time.
    utime(receiverPath, &newTime);
}
void EqualizePrivilages(char* donorPath, char* receiverPath)
{
    struct stat donor_stat;
    stat(donorPath, &donor_stat);
    chmod(receiverPath, donor_stat.st_mode);
}

/// Taken from: https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529
char RemoveDirectoryAt(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    int r = -1;

    if (d)
    {
        struct dirent *p;

        r = 0;
        while (!r && (p=readdir(d)))
        {
            int r2 = -1;
            char *buf;
            size_t len;

            /* Skip the names "." and ".." as we don't want to recurse on them. */
            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
                continue;

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf)
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);
                if (!stat(buf, &statbuf)) {
                    if (S_ISDIR(statbuf.st_mode))
                        r2 = RemoveDirectoryAt(buf);
                    else
                        r2 = unlink(buf);
                }
                free(buf);
            }
            r = r2;
        }
        closedir(d);
    }

    if (!r)
        r = rmdir(path);

    return r;
}
