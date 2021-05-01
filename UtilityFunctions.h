#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H

/// For outputing to syslog simple messages.
void SimpleLog(char* message);

/// return: if there's directory at provided directoryPath.
char DoesDirectoryExistsAt(char* directoryPath);
/// return: if there's file at provided filePath.
char DoesFileExistsAt(char* filePath);

/// return: File1.ModificationTime - File2.ModificationTime.
char CompareModificationTimeOfFiles(char* filePath1, char* filePath2);

/// Sets receiver's (2nd param) modification time to equals of donor's.
void EqualizeModificationTime(char* donorPath, char* receiverPath);
/// Sets receiver's (2nd param) privilages to equals of donor's.
void EqualizePrivilages(char* donorPath, char* receiverPath);

/// Taken from: https://stackoverflow.com/questions/2256945/removing-a-non-empty-directory-programmatically-in-c-or-c/42978529
/// Removes directory at provided path.
char RemoveDirectoryAt(const char *path);

#endif // UTILITYFUNCTIONS_H
