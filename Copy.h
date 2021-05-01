#ifndef COPY_H
#define COPY_H

/// Copies file from one filepath (source) to another (target).
int CopyFile(char* fileName_source, char* fileName_target, int mmapThreshold);

#endif