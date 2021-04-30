#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <string.h>

#include <fcntl.h>
#include <utime.h>
// #include <ctype.h>

// Tags description:
//TODO - thing to do.
//TODO_CLEAN - direct command to do when cleaning code off trash and debug lines.
//FUNC - below lines of code could be moved to function.
//STINK - potential error source.
//NOTE - potential for refactoring or other thing not covered by above, this
//  comment also is not part of documentation.

#define DAEMON_NAME "My Directory Synchronizing Daemon :P"


#pragma region >>> Global Variables <<<

//TODO_CLEAN: Set sleepTimeInSeconds (below) to 300.
int sleepTimeInSeconds = 4;
int mmapThreshold = 65536;
char flag_recursion = 0;
char flag_sigusr1 = 0;

// char* sourceDirPath;
// char* targetDirPath;

#pragma endregion


#pragma region >>> Utility Functions <<<

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

#pragma endregion


#pragma region >>> Initialization Functions <<<

void Daemon_SignalHandler(int signalCode);

void InitializeDaemon()
{
    // Fork off the parent process
    /// Note: fork makes 2 processes to spark out after this line, one - child-
    ///  process has pid= 0 and parent - pid>0.
    pid_t pid = fork();

    // An error occurred
    if (pid < 0)
        exit(EXIT_FAILURE);
    // Success: Let the parent terminate
    else if (pid > 0)
        exit(EXIT_SUCCESS);

    // On success: The child process becomes session leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    // Catch, ignore and handle signals
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGUSR1, Daemon_SignalHandler);

    // Fork off for the second time
    pid = fork();

    // An error occurred
    if (pid < 0)
        exit(EXIT_FAILURE);

    // Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);

    // Set new file permissions
    // umask(0);

    // Change the working directory to the root directory.
    // chdir("/");

    // Close all open file descriptors that may be inherited from parent process.
    // int i;
    for (int i = sysconf(_SC_OPEN_MAX); i>=0; i--)
        close(i);

    /// Opens log file (first parameter is daemon's name)
    openlog (DAEMON_NAME, LOG_PID, LOG_DAEMON);
}

#pragma endregion


void Daemon_SignalHandler(int signalCode)
{
    switch (signalCode)
    {
    case SIGUSR1:
        //TODO: insert propper signal handling.
        syslog(LOG_NOTICE, "Daemon awakens due to SIGUSR1 stimulant ~w~");
        break;

    default:
        break;
    }
}

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

void Daemon_SynchronizeDirectories(char* sourceDirPath, char* targetDirPath)
{
    // if (!DoesDirectoryExistsAt(sourceDirPath) || !DoesDirectoryExistsAt(targetDirPath))
    // {
    //     Log("Incorrect directory path!");
    //     return;
    // }

    DIR* sourceDir = opendir(sourceDirPath);
    DIR* targetDir = opendir(targetDirPath);
    if (sourceDir == NULL)
    {
        syslog (LOG_NOTICE, "Deamon couldn't find source directory at %s",
            sourceDirPath);
        return;
    }
    if (sourceDir == NULL)
    {
        syslog (LOG_NOTICE, "Deamon couldn't find target directory at %s",
            sourceDirPath);
        return;
    }

    struct dirent* currentEntry;

    //STINK: Not sure about this way of handling null equation, this is not
    //        normal lang with bools and stuff, here 1 is fine condition output.
    while ((currentEntry = readdir(sourceDir)) != NULL)
    {
        // If readdir() entry is a DIRectory.
        if(flag_recursion
            && currentEntry->d_type == DT_DIR
            && strcmp(currentEntry->d_name, ".") != 0
            && strcmp(currentEntry->d_name, "..") != 0 )
        {
            char nestedSourceDirPath[1024], nestedTargetDirPath[1024];

            snprintf(nestedSourceDirPath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(nestedTargetDirPath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            // Handle if target directory don't exist yet.
            if(!DoesDirectoryExistsAt(nestedTargetDirPath))
            {
                mkdir(nestedTargetDirPath, 0x0777);
                syslog(LOG_NOTICE, "Daemon created new directory at %s",
                    nestedTargetDirPath);
            }

            Daemon_SynchronizeDirectories(nestedSourceDirPath, nestedTargetDirPath);
        }

        // If readdir() entry is a REGular file.
        if(currentEntry->d_type == DT_REG)
        {
            char sourceFilePath[1024], targetFilePath[1024];

            snprintf(sourceFilePath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(targetFilePath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            if (!DoesFileExistsAt(targetFilePath))
            {
                //NOTE:
                if(CopyFile(sourceFilePath, targetFilePath))
                    syslog(LOG_NOTICE, "Daemon failed to create file %s",
                        targetFilePath);
                else
                    syslog(LOG_NOTICE, "Daemon created file %s", targetFilePath);

            }
            else if(CompareModificationTimeOfFiles(sourceFilePath, targetFilePath) < 0)
            {
                if(CopyFile(sourceFilePath, targetFilePath))
                    syslog(LOG_NOTICE, "Daemon failed to update file %s",
                        targetFilePath);
                else
                    syslog(LOG_NOTICE, "Daemon has updated file %s",
                        targetFilePath);
            }
        }
    }
    closedir(sourceDir);

    /// Sweepback:
    while ((currentEntry = readdir(targetDir)) != NULL )
    {
        // If readdir() entry is a DIRectory.
        if (flag_recursion
            && currentEntry->d_type == DT_DIR
            && strcmp(currentEntry->d_name, ".") != 0
            && strcmp(currentEntry->d_name, "..") != 0 )
        {
            char nestedSourceDirPath[1024], nestedTargetDirPath[1024];

            snprintf(nestedSourceDirPath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(nestedTargetDirPath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            if (!DoesDirectoryExistsAt(nestedSourceDirPath))
            {
                RemoveDirectoryAt(nestedTargetDirPath);
                syslog(LOG_NOTICE, "Daemon deleted directory at %s",
                    nestedTargetDirPath);
            }
        }

        // If readdir() entry is a REGular file.
        if (currentEntry->d_type == DT_REG)
        {
            char sourceFilePath[1024], targetFilePath[1024];

            snprintf(sourceFilePath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(targetFilePath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            if (!DoesFileExistsAt(sourceFilePath))
            {
                unlink(targetFilePath);
                syslog(LOG_NOTICE, "Daemon deleted file at %s",
                    targetFilePath);
            }
        }

    }

    closedir(targetDir);
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Error: Programm takes at least 2 arguments!\n");
        exit(EXIT_FAILURE);
    }

    ///FUNC: Handle optional arguments:
    int c;
    while (c = getopt(argc, argv, "Rt:T:") != -1)
    {
        printf("Looping at least...");
        switch(c)
        {
            case 't': // (t)ime - optarg
                sleepTimeInSeconds = atoi(optarg);
                if(sleepTimeInSeconds <= 0)
                {
                    printf("Impropper sleep time provided (%d s) overrided to default (300s)"
                    ,sleepTimeInSeconds);
                    sleepTimeInSeconds = 300;
                }
                break;
            case 'R': // (R)ecursion
                //TODO: Implement.
                flag_recursion = 1;
                break;
            case 'T': // (T)hreshold - optarg
                //TODO: Implement.
                mmapThreshold = atoi(optarg);
                if(mmapThreshold <= 0)
                {
                    printf("Impropper mmapThreshold provided (%d b) overrided to default (65536b)"
                    ,mmapThreshold);
                    mmapThreshold = 65536;
                }
                break;

        }
    }

    ///FUNC: Handle parameters:
    //TODO_CLEAN: Those comments:
    // printf("argc: %d\n", argc);
    // printf("strlen: %ld\n", strlen(argv[1]));
    // printf("sizeof: %ld\n", sizeof(argv[1]));
    //STINK: I'm not sure about if this way of allocation is propper, may need
    //        to change to malloc().
    char sourceDirPath[strlen(argv[optind + 1]) + 1];
    char targetDirPath[strlen(argv[optind + 2]) + 1];

    strcpy(sourceDirPath, argv[optind + 1]);
    strcpy(targetDirPath, argv[optind + 2]);

    /// Handle if directories even exist:
    if (!DoesDirectoryExistsAt(sourceDirPath))
    {
        printf("Source directory doesn't exist at %s", sourceDirPath);
        return EXIT_FAILURE;
    }
    if (!DoesDirectoryExistsAt(targetDirPath))
    {
        printf("Source directory doesn't exist at %s", targetDirPath);
        return EXIT_FAILURE;
    }




    // Confirming daemon configuration.
    printf("Daemon configuration:\n");
    printf("  Source path: %s\n", sourceDirPath);
    printf("  Target path: %s\n", targetDirPath);
    printf("  R= %d, t= %d, T= %d\n", flag_recursion, sleepTimeInSeconds, mmapThreshold);
    printf("\n");

    // Start daemon.
    InitializeDaemon(); // DEAMONIZED DOWN FROM HERE ---------------------------

    // Setting mask for newly created files.
    umask(0);
    // Changing working directory to root.
    chdir("/");

    int iterationsLifespan = 5;
    SimpleLog("Daemon has started working");

    while (iterationsLifespan)
    {
        // Daemon runs first, then falls asleep.
        //TODO: Insert daemon code here:
        Daemon_SynchronizeDirectories(sourceDirPath, targetDirPath);
        //TODO_CLEAN: Delete below line.
        SimpleLog("Daemon iteration uwu");

        sleep(sleepTimeInSeconds);
        if(flag_sigusr1 == 0)
            SimpleLog("Natural awakening of daemon!");
        // To make daemon stop after some time.
        iterationsLifespan--;
        // break;
    }

    SimpleLog("Daemon terminated qwq");
    closelog();

    return EXIT_SUCCESS;
}