#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <dirent.h>
#include <string.h>

// #include <ctype.h>
// #include <fcntl.h>

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
char flag_recursion = 0;
char flag_sigusr1 = 0;

char* sourceDirPath;
char* targetDirPath;

#pragma endregion


#pragma region >>> Utility Functions <<<

void SimpleLog(char* message)
    { syslog (LOG_NOTICE, "%s", message); }

char DirectoryExistsAt(char* directoryPath)
{
    DIR* directoryToCheck = opendir(directoryPath);

    if (directoryToCheck == NULL )
        return 0;

    closedir(directoryToCheck);
    return 1;
}

char FileExistsAt(char* filePath)
{
    int fileToCheck = open(filePath, O_WRONLY);

    if (fileToCheck <0)
        return 0;

    close(fileToCheck);
    return 1;
}

/// return: File1.Age - File2.Age
char CompareModificationTimeOfFiles(char* filePath1, char* filePath2)
{
    struct stat file1_stat, file2_stat;
    stat(filePath1, &file1_stat);
    stat(filePath2, &file2_stat);
    return file1_stat.st_mtime - file2_stat.st_mtime;
}

char RemoveDirectoryAt(const char *path)
{
    DIR *d = opendir(path);
    size_t path_len = strlen(path);
    char r = -1;
    if (d)      //Jeżeli otwieranie katalogu zakończy się powodzeniem
    {
        struct dirent *p;
        r = 0;

        while (!r && (p=readdir(d)))    //Czytanie po katalogu
        {
            int r2 = -1;
            char *buf;
            size_t len;

            if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))    //Pomijamy dowiązania symboliczne
            {
                continue;
            }

            len = path_len + strlen(p->d_name) + 2;
            buf = malloc(len);

            if (buf)    //Jeżeli alokowanie pamięci zakończy się powodzeniem
            {
                struct stat statbuf;

                snprintf(buf, len, "%s/%s", path, p->d_name);   //Stworzenie ścieżki do odczytanego pliku

                if (!stat(buf, &statbuf))
                {
                    if (S_ISDIR(statbuf.st_mode))       //Jeśli odczytany plik jest katalogiem
                    {
                        r2 = RemoveDirectoryAt(buf);

                        time_t now;
                        struct tm * timeinfo;
                        time ( &now );
                        timeinfo = localtime ( &now );

                        syslog(LOG_NOTICE, "%s: usunięto katalog %s", asctime(timeinfo), buf);
                    }
                    else
                    {
                        r2 = unlink(buf);       //Usunięcie pojedynczego pliku

                        time_t now;
                        struct tm * timeinfo;
                        time ( &now );
                        timeinfo = localtime ( &now );

                        syslog(LOG_NOTICE, "%s: usunięto plik %s", asctime(timeinfo), buf);
                    }
                }

                free(buf);      //Zwolnienie pamięci zaalokowanej przez bufor
            }

            r = r2;
        }

        closedir(d);
    }

    if (!r)     //Jeżeli usuwanie plików z katalogu zakończy się powodzeniem (funkcja remove_directory zwróci 0)
    {
        r = rmdir(path);    //Usunięcie pustego katalogu
    }

    return r;
}

#pragma endregion


#pragma region >>> Initialization Functions <<<

static void InitializeDaemon()
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


static void Daemon_SignalHandler(int signalCode)
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

#define COPYING_BUFFER_SIZE 65 536
int CopyFile(const char* fileName_source, const char* fileName_target)
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

    while ( read(in, buffer, bufferSize) )
        write(out, buffer, bufferSize);

    close(in);
    close(out);

    free(buffer);

    // syslog(LOG_NOTICE, "Successfully copied file.");
    return 0;
}

void Daemon_SynchronizeDirectories(char* sourceDirPath, char* targetDirPath)
{
    // if (!DirectoryExistsAt(sourceDirPath) || !DirectoryExistsAt(targetDirPath))
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
    while (currentEntry = readdir(sourceDir) != NULL)
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
            if(!dir_exist(nestedTargetDirPath))
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

            if (!FileExistsAt(targetFilePath))
            {
                //NOTE:
                if(CopyFile(sourceFilePath, targetFilePath))
                    syslog(LOG_NOTICE, "Daemon failed to create file %s",
                        targetFilePath);
                else
                    syslog(LOG_NOTICE, "Daemon created file %s", targetFilePath);

            }
            else if(CompareModificationTimeOfFiles(sourceFilePath, targetFilePath) <= 0)
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

    while ((currentEntry = readdir(targetDirPath)) != NULL )
    {
        if (recursion_flag
            && currentEntry->d_type == DT_DIR
            && strcmp(currentEntry->d_name, ".") != 0
            && strcmp(currentEntry->d_name, "..") != 0 )
        {
            char nestedSourceDirPath[1024], nestedTargetDirPath[1024];

            snprintf(nestedSourceDirPath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(nestedTargetDirPath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            if (!DirectoryExistsAt(nestedSourceDirPath))
            {
                RemoveDirectoryAt(nestedTargetDirPath);
                syslog(LOG_NOTICE, "Daemon deleted directory at %s",
                    nestedTargetDirPath);
            }
        }

        if (dest_ent->d_type == DT_REG)
        {
            char sourceFilePath[1024], targetFilePath[1024];

            snprintf(sourceFilePath, 1024,"%s/%s",
                sourceDirPath, currentEntry->d_name);
            snprintf(targetFilePath, 1024,"%s/%s",
                targetDirPath, currentEntry->d_name);

            if (!FileExistsAt(sourceFilePath))
            {
                unlink(targetFilePath);
                syslog(LOG_NOTICE, "Daemon deleted file at %s",
                    targetFilePath);
            }
        }

    }

    closedir(targetDirPath);
}

int main(int argc, char* argv[])
{
    ///FUNC: Handle parameters:
    if (argc < 2)
    {
        printf("Error: Programm takes at least 2 arguments!\n");
        exit(EXIT_FAILURE);
    }
    //STINK: I'm not sure about if this way of allocation is propper, may need
    //        to change to malloc().
    sourceDirPath = char[sizeof(argv[1])];
    targetDirPath = char[sizeof(argv[2])];

    strcpy(sourceDirPath, argv[1]);
    strcpy(targetDirPath, argv[2]);

    /// Handle if directories even exist:
    if (!DirectoryExistsAt(sourceDirPath))
    {
        printf("Source directory doesn't exist at %s", sourceDirPath);
        return EXIT_FAILURE;
    }
    if (!DirectoryExistsAt(targetDirPath))
    {
        printf("Source directory doesn't exist at %s", targetDirPath);
        return EXIT_FAILURE;
    }


    ///FUNC: Handle optional arguments:
    int c;
    while (c = getopt(argc, argv, "RtT:") != -1)
    {
        switch(c)
        {
            case 't': // (t)ime - optarg
                sleepTimeInSeconds=atoi(optarg);
                if(sleepTimeInSeconds == 0)
                {
                    printf("Impropper sleep time provided (%d s) overrided to default (300s)"
                    ,sleepTimeInSeconds);
                    sleepTimeInSeconds = 300;
                }
                break;
            case 'R': // (R)ecursion
                //TODO: Implement.
                recursion_flag=1;
                break;
            case 'T': // (T)hreshold - optarg
                //TODO: Implement.
                break;

        }
    }


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

        sleep (sleepTimeInSeconds);
        if(sigusr1 == 0)
            SimpleLog("Natural awakening of daemon!");
        // To make daemon stop after some time.
        iterationsLifespan--;
        // break;
    }

    SimpleLog("Daemon terminated qwq");
    closelog();

    return EXIT_SUCCESS;
}