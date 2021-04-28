#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

// Tags description:
//TODO - thing to do.
//TODO_CLEAN - direct command to do when cleaning code off trash and debug lines.
//FUNC - below lines of code could be moved to function.

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
    //TODO: Implement a working signal handler
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

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


static void SignalHandler(int signalCode)
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
    syslog(LOG_NOTICE, "Copying %s to %s...", fileName_source, fileName_target);
    int in = open(fileName_source, O_RDONLY);
    int out = open(fileName_target, O_WRONLY | O_CREAT);

    // Error handling (bit primitive, but!):
    if ( in == -1 )
    {
        syslog(LOG_NOTICE, "Source file \"%s\" not found!", fileName_source);
        close(in);
        close(out);
        return -1;
    }
    if ( out == -1 )
    {
        syslog(LOG_NOTICE, "Failed to target file \"%s\"", fileName_target);
        close(in);
        close(out);
        return -2;
    }

    // Copying file:

    unsigned char* buffer =
        (unsigned char*) malloc(sizeof(unsigned char) * COPYING_BUFFER_SIZE);

    while ( read(in, buffer, bufferSize) )
        write(out, buffer, bufferSize);

    close(in);
    close(out);

    free(buffer);

    syslog(LOG_NOTICE, "Successfully copied file.");
    return 0;
}

char DirectoryExists(char* directortPath)
{
    DIR* directoryToCheck = opendir(directortPath);

    if (directoryToCheck == NULL )
        return 0;

    closedir(directoryToCheck);
    return 1;
}

void DaemonUtility_Synchronize()
{
    if (!DirectoryExists(sourceDirPath) || !DirectoryExists(targetDirPath))
    {
        Log("Incorrect directory path!");
        return;
    }

}

int main(int argc, char* argv[])
{
    ///FUNC: Handle parameters:
    if(argc < 2)
    {
        printf("Programm takes at least 2 arguments");
        exit(EXIT_FAILURE);
    }
    sourceDirPath = char[sizeof(argv[1])];
    targetDirPath = char[sizeof(argv[2])];

    strcpy(sourceDirPath, argv[1]);
    strcpy(targetDirPath, argv[2]);

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
        DaemonUtility_Synchronize();
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