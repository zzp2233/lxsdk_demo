/**
*@file
*@brief        文件系统模块
*@details  文件系统接口，单单针对nor flash
*@author       zn.zeng
*@date       Jul 01, 2021
*@version      v1.0
*/
#ifndef _UTE_MODULE_FILESYSTEM_H_
#define _UTE_MODULE_FILESYSTEM_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "lfs.h"


#define UTE_FILE_SYSTEM_DIR_INFO_FILE_MAX   50
#define UTE_FILE_SYSTEM_DIR_FILE_NAME_MAX   21
#define UTE_FILE_SYSTEM_ABS_PATH_MAX 100


enum fs_open_flags
{
    // open flags
    FS_O_RDONLY = 0x0001,         // Open a file as read only
    FS_O_WRONLY = 0x0002,         // Open a file as write only
    FS_O_CREAT  = 0x0004,    // Create a file if it does not exist
    FS_O_EXCL   = 0x0008,    // Fail if a file already exists
    FS_O_TRUNC  = 0x0010,    // Truncate the existing file to zero size
    FS_O_APPEND = 0x0020,    // Move to end of file on every write
};

enum fs_whence_flags
{
    FS_SEEK_SET = 0,   // Seek relative to an absolute position
    FS_SEEK_CUR = 1,   // Seek relative to the current file position
    FS_SEEK_END = 2,   // Seek relative to the end of the file
};
typedef struct
{
    int32_t totalSize;
    uint8_t filesCnt;
    uint8_t filesName[UTE_FILE_SYSTEM_DIR_INFO_FILE_MAX][UTE_FILE_SYSTEM_DIR_FILE_NAME_MAX];
    int32_t filesSize[UTE_FILE_SYSTEM_DIR_INFO_FILE_MAX];
} ute_module_filesystem_dir_t;
uint32_t uteModuleFilesystemGetFreeByte(void);
void uteModuleFilesystemDelAllData(void);
void uteModuleFilesystemInit(void);
bool uteModuleFilesystemLs(char *path,ute_module_filesystem_dir_t *dirInfo,uint8_t *findFileName);
bool uteModuleFilesystemOpenFile(char *path,void **file,int flags);
bool uteModuleFilesystemCloseFile(void *file);
bool uteModuleFilesystemWriteData(void *file,void *buff,uint32_t size);
bool uteModuleFilesystemReadData(void *file,void *buff,uint32_t size);
int32_t uteModuleFilesystemGetFileSize(void *file);
void uteModuleFilesystemSeek(void *file,int32_t offset,int whence);
bool uteModuleFilesystemDelFile(char *path);
bool uteModuleFilesystemCreateDirectory(char *path);
bool uteModuleFilesystemRenameFile(char *oldpath,char *newpath);
bool uteModuleFilesystemRenameDirectory(char *oldpath, char *newpath, bool isDelOld);
bool uteModuleFilesystemDelDirectoryAllFiles(char *path);

#endif //_UTE_MODULE_FILESYSTEM_H_

