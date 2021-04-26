///
/// daemonize.c
/// This example daemonizes a process, writes a few log messages,
/// sleeps 20 seconds and terminates afterwards.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#define DAEMON_NAME "My Directory Synchronizing Daemon :P"

static void InitializeDaemon()
{
    pid_t pid;

    /// Fork off the parent process
    pid = fork();

    /// An error occurred
    if (pid < 0)
        exit(EXIT_FAILURE);

    /// Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /// On success: The child process becomes session leader
    if (setsid() < 0)
        exit(EXIT_FAILURE);

    /// Catch, ignore and handle signals
    //TODO: Implement a working signal handler
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /// Fork off for the second time
    pid = fork();

    /// An error occurred
    if (pid < 0)
        exit(EXIT_FAILURE);

    /// Success: Let the parent terminate
    if (pid > 0)
        exit(EXIT_SUCCESS);

    /// Set new file permissions
    umask(0);

    /// Change the working directory to the root directory
    ///  or another appropriated directory
    chdir("/");

    /// Close all open file descriptors
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
    {
        close (x);
    }

    /// Opens log file (first parameter is daemon's name)
    openlog (DAEMON_NAME, LOG_PID, LOG_DAEMON);
}

static void Log(char* message)
{
    syslog (LOG_NOTICE, "%s", message);
}

char* sourceDirPath;
char* targetDirPath;
int main(int argc, char* argv[])
{
    InitializeDaemon();
    int iterationsLifespan = 5;
    Log("Daemon has started working");
    // syslog (LOG_NOTICE, "Daemon has started working");
    while (iterationsLifespan)
    {
        //TODO: Insert daemon code here.

        Log("Daemon iteration uwu");

        sleep (4);
        // To make daemon stop after some time.
        iterationsLifespan--;
        // break;
    }

    syslog (LOG_NOTICE, "First daemon terminated.");
    closelog();

    return EXIT_SUCCESS;
}