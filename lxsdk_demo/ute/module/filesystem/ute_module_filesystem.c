/**
*@file
*@brief        文件系统模块
*@details  文件系统接口，单单针对nor flash，区别于平台的fat文件系统
*@author       zn.zeng
*@date       Jul 01, 2021
*@version      v1.0
*/
#include "ute_module_filesystem.h"
#include "ute_module_platform.h"
#include "ute_module_log.h"
#include "lfs.h"
/* zn.zeng 2022-02-14*/
void *uteModuleFilesystemMute;
/**
*@brief  little_fs 读数据接口
*@details  little_fs 访问flash的读接口
*@param[in] 地址转换，偏移，buff，size
*@author        zn.zeng
*@date        Jul 01, 2021
*/
int uteModuleLittleFsBlockRead(const struct lfs_config *c, lfs_block_t block,
                               lfs_off_t off, void *buffer, lfs_size_t size)
{
    uteModulePlatformFlashNorRead((uint8_t *)buffer, UTE_MODULE_FILESYSTEM_START_ADDRESS + block * 4096 + off, size);
    return 0;
}
/**
*@brief  little_fs 写数据接口
*@details  little_fs 访问flash的写接口
*@param[in] 地址转换，偏移，buff，size
*@author        zn.zeng
*@date        Jul 01, 2021
*/
int uteModuleLittleFsBlockWrite(const struct lfs_config *c, lfs_block_t block,
                                lfs_off_t off, const void *buffer, lfs_size_t size)
{
    uteModulePlatformFlashNorWrite((uint8_t *)buffer, UTE_MODULE_FILESYSTEM_START_ADDRESS + block * 4096 + off, size);
    return 0;
}
/**
*@brief  little_fs 擦除一个SECTOR接口
*@details  little_fs 访问flash的擦除一个SECTOR
*@param[in] 地址转换，偏移
*@author        zn.zeng
*@date        Jul 01, 2021
*/
int uteModuleLittleFsBlockErase(const struct lfs_config *c, lfs_block_t block)
{
    uteModulePlatformFlashNorErase(UTE_MODULE_FILESYSTEM_START_ADDRESS + block * 4096);
    return 0;
}
/**
*@brief  little_fs 同步接口
*@details  同步接口，未使用
*@author        zn.zeng
*@date        Jul 01, 2021
*/
int uteModuleLittleFsBlockEraseSync(const struct lfs_config *c)
{
    return 0;
}
/*!littleFs 配置参数  zn.zeng  Jul 01, 2021 */
const struct lfs_config uteModuleFilesystemFsConfig =
{
    // block device operations
    .read  = uteModuleLittleFsBlockRead,
    .prog  = uteModuleLittleFsBlockWrite,
    .erase = uteModuleLittleFsBlockErase,
    .sync  = uteModuleLittleFsBlockEraseSync,

    // block device configuration
    .read_size = 256,
    .prog_size = 256,
    .block_size = 4096,
    .block_count = UTE_MODULE_FILESYSTEM_MAX_SIZE/4096,
    .cache_size = 512,
    .lookahead_size = 128,
    .block_cycles = 800,
};
/*! 文件系统指针 zn.zeng  modify Jul 01, 2021 */
lfs_t uteModuleLfs;
/*! 文件指针 zn.zeng  modify Jul 01, 2021 */
lfs_file_t uteModuleLfsOpenFile;
/**
*@brief  读取little fs 是文件系统状态
*@details  读取little fs 是文件系统状态回调
*@author        zn.zeng
*@date        Jul 01, 2021
*/
static int uteModuleFilesystemFuseStatfsCountCb(void *p, lfs_block_t block)
{
    lfs_size_t *p1 = (lfs_size_t *)p;
    *p1 += 1;
    return 0;
}

/**
*@brief  读取指定文件系统剩余空间
*@details  读取传入指定文件系统(盘)剩余空间
*@return uint32_t 返回值为剩余空间byte
*@param lfs_t *fs文件系统(盘),
*@param lfs_config fsConfig对应(盘)文件系统的配置参数；
*@author        ellison add ,
*@date       2022-May-20 11:38
*/
static uint32_t uteModuleFilesystemGetFreeByteCommon(lfs_t *fs,const struct lfs_config fsConfig)
{
    lfs_size_t use_space = 0;
    lfs_size_t free_space = 0;

    int err = lfs_fs_traverse(fs, uteModuleFilesystemFuseStatfsCountCb, &use_space);
    if (err)
    {
        return free_space;
    }
    free_space = (fsConfig.block_count - use_space) * fsConfig.block_size;
    return free_space;

}

/**
*@brief  读取文件系统剩余空间
*@details  读取文件系统剩余空间
*@return uint32_t 返回值为剩余空间byte
*@author        zn.zeng
*@date        Jul 01, 2021
*/
uint32_t uteModuleFilesystemGetFreeByte(void)
{
    lfs_size_t free_space = 0;
    free_space = uteModuleFilesystemGetFreeByteCommon(&uteModuleLfs,uteModuleFilesystemFsConfig);
    return free_space;
}

/**
*@brief  删除所有数据
*@details  把文件系统格式化，删除所有数据
*@author        zn.zeng
*@date        Jul 01, 2021
*/
//#include "ute_application_common.h"
void uteModuleFilesystemDelAllData(void)
{
    lfs_format(&uteModuleLfs, &uteModuleFilesystemFsConfig);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
}
/**
*@brief  文件系统初始化
*@details  文件系统初始化，配置参数
*@author        zn.zeng
*@date        Jul 01, 2021
*/

// static uint32_t fs_tick = 0;
void uteModuleFilesystemInit(void)
{
    // mount the filesystem
    uteModulePlatformWdgFeed();


    // fs_tick =  tick_get();
    int err = lfs_mount(&uteModuleLfs, &uteModuleFilesystemFsConfig);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,module err=%d", __func__,err);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err<0)
    {
        lfs_format(&uteModuleLfs, &uteModuleFilesystemFsConfig);
        lfs_mount(&uteModuleLfs, &uteModuleFilesystemFsConfig);
    }
    // printf("fs_tick = %d\n", tick_get() - fs_tick);

#if UTE_MODULE_ALI_UPAY_SUPPORT
    //mount the aliUpay filesystem
    err = lfs_mount(&uteModuleAliUpayLfs, &uteModuleAliUpayFilesystemFsConfig);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,uteModuleAliUpayLfs module err=%d", __func__,err);
    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err<0)
    {
        lfs_format(&uteModuleAliUpayLfs, &uteModuleAliUpayFilesystemFsConfig);
        lfs_mount(&uteModuleAliUpayLfs, &uteModuleAliUpayFilesystemFsConfig);
    }

#endif
    // uteModulePlatformWdgStart();
    uteModulePlatformCreateMutex(&uteModuleFilesystemMute);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,free space= %dbyte", __func__, uteModuleFilesystemGetFreeByte());
}
/**
*@brief  获取文件夹下的文件信息
*@details  获取传入指定文件系统(盘)的文件夹下的文件信息，查找指定的文件系统的文件夹下是否存在某个文件
*@param[in] path 指定文件系统(盘)的文件夹路径
*@param[out] ute_module_filesystem_dir_t *dirInfo，获取到的文件夹所有信息，可以为NULL
*@param[in] findFileName 要查找的文件名字，可以填NULL
*@return true 为查找到文件，false 为未找文件或者打开文件夹失败
*@author        // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemLsCommon(lfs_t *fs,char *path, ute_module_filesystem_dir_t *dirInfo,uint8_t *findFileName)
{
    lfs_dir_t dir;
    int err = LFS_ERR_OK;
    bool isHasFindFileSuccess = false;
    struct lfs_info info;
    if (dirInfo != NULL)
    {
        memset(dirInfo, 0, sizeof(ute_module_filesystem_dir_t));
    }
    if (lfs_dir_open(fs, &dir, (const char*)path) == LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,Directory %s", __func__, path);
        while (true)
        {
            err = lfs_dir_read(fs, &dir, &info);
            if (err != 1)
            {
                lfs_dir_close(fs, &dir);
                return isHasFindFileSuccess;
            }

            if (info.type == LFS_TYPE_DIR)
            {
                UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,dirname=%-20s", __func__, info.name);
            }
            else
            {
                if (dirInfo != NULL)
                {
                    dirInfo->totalSize = dirInfo->totalSize + info.size;
                    if (dirInfo->filesCnt < UTE_FILE_SYSTEM_DIR_INFO_FILE_MAX)
                    {
                        // snprintf((void *)&dirInfo->filesName[dirInfo->filesCnt][0], UTE_FILE_SYSTEM_DIR_FILE_NAME_MAX,"%.20s", info.name);
                        memcpy((void *)&dirInfo->filesName[dirInfo->filesCnt][0],(void*)&info.name[0], strlen((const char*)&info.name[0]) > UTE_FILE_SYSTEM_DIR_FILE_NAME_MAX ? UTE_FILE_SYSTEM_DIR_FILE_NAME_MAX : strlen((const char*)&info.name[0]));
                        dirInfo->filesSize[dirInfo->filesCnt] = info.size;
                    }
                    dirInfo->filesCnt++;
                }
                if (findFileName != NULL)
                {
                    if (memcmp((void*)&findFileName[0],(void*)&info.name[0], strlen((const char*)&findFileName[0])) == 0)
                    {
                        isHasFindFileSuccess = true;
                    }
                }
                UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,filename=%-20s,file size=%d", __func__, info.name, info.size);
            }
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,No such directory", __func__);
    }
    return isHasFindFileSuccess;
}


/**
*@brief  获取文件夹下的文件信息
*@details  获取文件夹下的文件信息，查找文件夹下是否存在某个文件
*@param[in] path 文件夹路径
*@param[out] ute_module_filesystem_dir_t *dirInfo，获取到的文件夹所有信息，可以为NULL
*@param[in] findFileName 要查找的文件名字，可以填NULL
*@return true 为查找到文件，false 为未找文件或者打开文件夹失败
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemLs(char *path, ute_module_filesystem_dir_t *dirInfo,uint8_t *findFileName)
{
    return uteModuleFilesystemLsCommon(&uteModuleLfs,path,dirInfo,findFileName);

}

/**
*@brief  打开文件
*@details  打开指定系统(盘)里面的系统文件
*@param[in] lfs_t lfs 指定的系统(盘)文件,
*@param[in] path 文件绝对路径，包括文件名
*@param[out] void **file，打开文件的指针
*@param[in] int flags 以什么方式打开文件，具体查看fs_open_flags
*@return true 为打开成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemOpenFileCommon(lfs_t *lfs,char *path, void **file, int flags)
{

    int lFsflags = 0;
    if (flags & FS_O_RDONLY)
    {
        lFsflags |= LFS_O_RDONLY;
    }
    if (flags & FS_O_WRONLY)
    {
        lFsflags |= LFS_O_WRONLY;
    }
    if (flags & FS_O_CREAT)
    {
        lFsflags |= LFS_O_CREAT;
    }
    if (flags & FS_O_EXCL)
    {
        lFsflags |= LFS_O_EXCL;
    }
    if (flags & FS_O_TRUNC)
    {
        lFsflags |= LFS_O_TRUNC;
    }
    if (flags & FS_O_APPEND)
    {
        lFsflags |= LFS_O_APPEND;
    }
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,path=%s", __func__,path);
    uteModulePlatformTakeMutex(uteModuleFilesystemMute);
    int err = lfs_file_open(lfs, &uteModuleLfsOpenFile, (const char *)path, lFsflags);
    if (err != LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        uteModulePlatformGiveMutex(uteModuleFilesystemMute);
        return false;
    }
    else
    {
        *file = (void *)&uteModuleLfsOpenFile;
        return true;
    }


}


/**
*@brief  打开文件
*@details  打开文件
*@param[in] path 文件绝对路径，包括文件名
*@param[out] void **file，打开文件的指针
*@param[in] int flags 以什么方式打开文件，具体查看fs_open_flags
*@return true 为打开成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemOpenFile(char *path, void **file, int flags)
{
    bool isOpen = false;
    isOpen = uteModuleFilesystemOpenFileCommon(&uteModuleLfs,path, file,  flags);
    return isOpen;
}

/**
*@brief  关闭文件
*@details  关闭指定文件系统(盘)的，指定文件
*@details  lfs_t *lfs, 指定的文件系统(盘)
*@param[in] void *file，关闭文件的指针
*@return true 为关闭成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemCloseFileCommon(lfs_t *lfs,void *file)
{
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s", __func__);
    int err = lfs_file_close(lfs, (lfs_file_t *)file);
    uteModulePlatformGiveMutex(uteModuleFilesystemMute);
    if (err != LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}

/**
*@brief  关闭文件
*@details
*@param[in] void *file，关闭文件的指针
*@return true 为关闭成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemCloseFile(void *file)
{
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s", __func__);

    return uteModuleFilesystemCloseFileCommon(&uteModuleLfs,file);
}



/**
*@brief  写入文件数据
*@details 从指定的文件系统(盘)的文件中，写入文件数据
*@param[in] lfs_t *lfs,指定的文件系统(盘)
*@param[in] void *file，文件的指针
*@param[in] void *buff 数据指针
*@param[in] uint32_t size 数据大小
*@return true 写入成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemWriteDataCommon(lfs_t *lfs,void *file, void *buff, uint32_t size)
{
    int err = lfs_file_write(lfs, (lfs_file_t *)file, buff, size);
    if (err < LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}


/**
*@brief  写入文件数据
*@details
*@param[in] void *file，文件的指针
*@param[in] void *buff 数据指针
*@param[in] uint32_t size 数据大小
*@return true 写入成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemWriteData(void *file, void *buff, uint32_t size)
{
    return uteModuleFilesystemWriteDataCommon(&uteModuleLfs,file, buff, size);
}


/**
*@brief  读入文件数据
*@details 从指定的系统盘中读取文件数据
*@param[in] lfs_t *lfs，文件的系统盘指针
*@param[in] void *file，文件的指针
*@param[in] void *buff 数据指针
*@param[in] uint32_t size 数据大小
*@return true 读入成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemReadDataCommon(lfs_t *lfs,void *file, void *buff, uint32_t size)
{
    int err = lfs_file_read(lfs, (lfs_file_t *)file, buff, size);
    if (err < LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}


/**
*@brief  读入文件数据
*@details
*@param[in] void *file，文件的指针
*@param[in] void *buff 数据指针
*@param[in] uint32_t size 数据大小
*@return true 读入成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/

bool uteModuleFilesystemReadData(void *file, void *buff, uint32_t size)
{
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,size=%d", __func__, size);
    return uteModuleFilesystemReadDataCommon(&uteModuleLfs,file, buff, size);

}

/**
*@brief  获取文件大小
*@details 从指定系统盘中，获取输入文件名的文件大小
*@param[in] lfs_t *lfs，文件系统（盘）的指针
*@param[in] void *file，文件的指针
*@return  返回文件大小，单位是byte
*@author       // * ellison add ,
*@date        2022-May-20
*/
int32_t uteModuleFilesystemGetFileSizeCommon(lfs_t *lfs,void *file)
{
    return lfs_file_tell(lfs, (lfs_file_t *)file);
}


/**
*@brief  获取文件大小
*@details
*@param[in] void *file，文件的指针
*@return  返回文件大小，单位是byte
*@author        zn.zeng
*@date        Jul 01, 2021
*/
int32_t uteModuleFilesystemGetFileSize(void *file)
{
    uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
    return uteModuleFilesystemGetFileSizeCommon(&uteModuleLfs, (lfs_file_t *)file);
}

/**
*@brief  设置偏移量
*@details 从指定系统盘中，设置文件的偏移量
*@param[in] void *file，文件的指针
*@param[in] int32_t offset，游标偏移量
*@param[in] int whence，从哪里开始偏移，查看fs_whence_flags
*@author       // * ellison add ,
*@date        2022-May-20
*/
static void uteModuleFilesystemSeekCommon(lfs_t *lfs,void *file, int32_t offset, int whence)
{
    int lfsWhence = 0;
    if (whence & FS_SEEK_SET)
    {
        lfsWhence = LFS_SEEK_SET;
    }
    if (whence & FS_SEEK_CUR)
    {
        lfsWhence = LFS_SEEK_CUR;
    }
    if (whence & FS_SEEK_END)
    {
        lfsWhence = LFS_SEEK_END;
    }
    lfs_file_seek(lfs, (lfs_file_t *)file, offset, lfsWhence);
}


/**
*@brief  设置偏移量
*@details
*@param[in] void *file，文件的指针
*@param[in] int32_t offset，游标偏移量
*@param[in] int whence，从哪里开始偏移，查看fs_whence_flags
*@author        zn.zeng
*@date        Jul 01, 2021
*/
void uteModuleFilesystemSeek(void *file, int32_t offset, int whence)
{
    uteModuleFilesystemSeekCommon(&uteModuleLfs, (lfs_file_t *)file, offset, whence);
}

/**
*@brief  删除文件
*@details   从指定的文件系统盘中，删除文件
*@param[in] lfs_t *lfs, 文件系统盘
*@param[in] path 文件绝对路径，包括文件名
*@return true 为删除文件成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemDelFileCommon(lfs_t *lfs,char *path)
{
    int err = lfs_remove(lfs, (const char *)path);
    if (err < LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}

/**
*@brief  删除文件
*@details
*@param[in] path 文件绝对路径，包括文件名
*@return true 为删除文件成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemDelFile(char *path)
{
    return uteModuleFilesystemDelFileCommon(&uteModuleLfs, path);
}

/**
*@brief  新建文件夹
*@details 在指定的系统盘中创建文件夹
*@param[in] path 文件夹名
*@return true 为新建文件夹成功
*@author       // * ellison add ,
*@date        2022-May-20
*/
static bool uteModuleFilesystemCreateDirectoryCommon(lfs_t *lfs,char *path)
{
    int err = lfs_mkdir(lfs, (const char *)path);
    if (err < LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}

/**
*@brief  新建文件夹
*@details
*@param[in] path 文件夹名
*@return true 为新建文件夹成功
*@author        zn.zeng
*@date        Jul 01, 2021
*/
bool uteModuleFilesystemCreateDirectory(char *path)
{
    return  uteModuleFilesystemCreateDirectoryCommon(&uteModuleLfs, path);
}

/**
*@brief  重命名文件
*@details 从指定的系统盘中，重新命名文件
*@param[in] oldPath 旧文件名,全路径
*@param[in] newPath 新文件名,全路径
*@return true 重命名成功
*@author       // * ellison add ,
*@date        2022-May-20
*/

static bool uteModuleFilesystemRenameFileCommon(lfs_t *lfs,char *oldpath,char *newpath)
{
    int err = lfs_rename(&uteModuleLfs,(const char *)oldpath,(const char *)newpath);
    if (err < LFS_ERR_OK)
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,err=%d", __func__, err);
        return false;
    }
    else
    {
        return true;
    }
}
/**
*@brief  重命名文件
*@details
*@param[in] oldPath 旧文件名,全路径
*@param[in] newPath 新文件名,全路径
*@return true 重命名成功
*@author        zn.zeng
*@date        2022-01-26
*/
bool uteModuleFilesystemRenameFile(char *oldpath,char *newpath)
{
    return uteModuleFilesystemRenameFileCommon(&uteModuleLfs,oldpath,newpath);
}

/**
 * @brief        重命名目录
 * @details
 * @param[in]    oldPath 旧目录名,全路径
 * @param[in]    newPath 旧目录名,全路径
 * @param[in]    isDelOld 是否删除旧目录
 * @return       true 为成功,false 为失败
 * @author       Wang.Luo
 * @date         2024-04-23
 */
bool uteModuleFilesystemRenameDirectory(char *oldpath, char *newpath, bool isDelOld)
{
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL,"%s,rename directory:%s->%s", __func__, oldpath, newpath);
    uteModuleFilesystemCreateDirectory(newpath);
    lfs_dir_t old_dir;
    lfs_dir_t new_dir;
    int err = lfs_dir_open(&uteModuleLfs, &old_dir, (const char *)oldpath);
    if (err == LFS_ERR_OK)
    {
        uteModulePlatformTakeMutex(uteModuleFilesystemMute);
        err = lfs_dir_open(&uteModuleLfs, &new_dir, (const char *)newpath);
        if (err == LFS_ERR_OK)
        {
            struct lfs_info info;
            while (lfs_dir_read(&uteModuleLfs, &old_dir, &info) > 0)
            {
                // 忽略. 和 .. 目录
                if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0)
                {
                    continue;
                }
                // 生成原路径和新路径
                char *old_path = (char *)uteModulePlatformMemoryAlloc(strlen((const char *)oldpath) + strlen(info.name) + 2);
                char *new_path = (char *)uteModulePlatformMemoryAlloc(strlen((const char *)newpath) + strlen(info.name) + 2);
                sprintf(old_path, "/%s/%s", oldpath, info.name);
                sprintf(new_path, "/%s/%s", newpath, info.name);
                // 移动文件
                err = lfs_rename(&uteModuleLfs, old_path, new_path);
                UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,rename sta:%d,old_path:%s,new_path:%s", __func__, err, old_path, new_path);
                uteModulePlatformMemoryFree(old_path);
                uteModulePlatformMemoryFree(new_path);
            }
            lfs_dir_close(&uteModuleLfs, &new_dir);
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,new dir open err:%d", __func__, err);
        }
        lfs_dir_close(&uteModuleLfs, &old_dir);
        // 删除原目录
        if (isDelOld)
        {
            err = lfs_remove(&uteModuleLfs, (const char *)oldpath);
        }
        uteModulePlatformGiveMutex(uteModuleFilesystemMute);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,old dir open err:%d", __func__, err);
    }
    return (err == LFS_ERR_OK ? true : false);
}

/**
 * @brief        删除目录下的所有文件
 * @details
 * @param[in]    path 目录名,全路径
 * @return       true 为成功,false 为失败
 * @author       Wang.Luo
 * @date         2024-04-24
 */
bool uteModuleFilesystemDelDirectoryAllFiles(char *path)
{
    UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,del all files:%s", __func__, path);
    // 声明目录句柄
    lfs_dir_t dir;
    // 打开目录
    int err = lfs_dir_open(&uteModuleLfs, &dir, (const char *)path);
    if (err == LFS_ERR_OK)
    {
        uteModulePlatformTakeMutex(uteModuleFilesystemMute);
        // 读取目录中的文件并删除每个文件
        struct lfs_info info;
        while (lfs_dir_read(&uteModuleLfs, &dir, &info) > 0)
        {
            // 忽略 . 和 .. 目录
            if (strcmp(info.name, ".") == 0 || strcmp(info.name, "..") == 0)
            {
                continue;
            }
            // 生成文件路径
            char *file_path = (char *)uteModulePlatformMemoryAlloc(strlen((const char *)path) + strlen(info.name) + 2);
            sprintf(file_path, "%s/%s", path, info.name);
            // 删除文件
            err = lfs_remove(&uteModuleLfs, file_path);
            if (err)
            {
                UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,%s,del err:%d", __func__, file_path, err);
            }
            uteModulePlatformMemoryFree(file_path);
        }
        // 关闭目录
        lfs_dir_close(&uteModuleLfs, &dir);
        uteModulePlatformGiveMutex(uteModuleFilesystemMute);
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_FILE_SYSTEM_LVL, "%s,%s,open dir err:%d", __func__, path, err);
    }
    return (err == LFS_ERR_OK ? true : false);
}


