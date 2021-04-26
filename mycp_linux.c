#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int MyMV(const char* fileName_source, const char* fileName_target, int bufferSize)
{
    printf("Processing copying file...");
    int in = open(fileName_source, O_RDONLY);
    int out = open(fileName_target, O_WRONLY | O_CREAT);

    if ( in == -1 )
    {
        printf("\nSource file \"%s\" not found!\n", fileName_source);
        close(in);
        close(out);
        return -1;
    }
    if ( out == -1 )
    {
        printf("\nFailded to target file \"%s\"\n", fileName_target);
        close(in);
        close(out);
        return -2; 
    }

    unsigned char* buffer = (unsigned char*) malloc(sizeof(unsigned char) * bufferSize);

    while ( read(in, buffer, bufferSize) )
        write(out, buffer, bufferSize);

    close(in);
    close(out);

    free(buffer);

    printf("Success!\n");
    return 0;
}

int main(int argc, char* argv[])
{
    // MyMV("test.txt", "test_out.txt", 4);
    
    if(argc < 4)
    {
        printf("Programm takes 3 arguments not %d", argc);
        return 0;
    }

    printf("%s\n", argv[0]);
    printf("%s\n", argv[1]);
    printf("%s\n", argv[2]);
    printf("%s\n", argv[3]);

    MyMV(argv[1], argv[2], atoi(argv[3]));

    // printf("HEWWOOO!!\n");
    return 0;
}