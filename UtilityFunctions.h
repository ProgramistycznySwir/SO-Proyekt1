#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H

void SimpleLog(char* message);

char DoesDirectoryExistsAt(char* directoryPath);
char DoesFileExistsAt(char* filePath);

/// return: File1.ModificationTime - File2.ModificationTime
char CompareModificationTimeOfFiles(char* filePath1, char* filePath2);

/// Literally just sets receiver.ModificationTime = donor.ModificationTime
void EqualizeModificationTime(char* donorPath, char* receiverPath);
void EqualizePrivilages(char* donorPath, char* receiverPath);

/// Taken from: https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529
char RemoveDirectoryAt(const char *path);

#endif UTILITYFUNCTIONS_H
