#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_lockScreen.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_module_filesystem.h" // 添加文件系统头文件
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

ute_module_password_data_t uteModulePasswordData;
void *uteModulePasswordMute; // 互斥锁

/**
* @brief  密码配置读取
* @details 从文件读取密码配置参数。
*/
void uteModulePasswordDataReadConfig(void)
{
    // printf("[PASSWORD] Reading password configuration...\n");

    uteModulePlatformTakeMutex(uteModulePasswordMute); // 加锁

    void *file;
    // 尝试打开密码配置文件
    uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_PASSWORD_INFO, &file, FS_O_RDONLY);

    // 读取密码数据
    uteModuleFilesystemReadData(file, &uteModulePasswordData, sizeof(ute_module_password_data_t));
    //printf("[PASSWORD] Read password data: cnt=%d, value={", uteModulePasswordData.password_cnt);
    for (int i = 0; i < uteModulePasswordData.password_cnt; i++)
    {
        printf("%d ", uteModulePasswordData.password_value[i]);
    }
    printf("}\n");

    // 关闭文件
    uteModuleFilesystemCloseFile(file);

    uteModulePlatformGiveMutex(uteModulePasswordMute); // 解锁
}

/**
* @brief 密码配置保存
* @details 将密码配置保存到文件。
*/
void uteModulePasswordDataSaveConfig(void)
{
    //  printf("[PASSWORD] Saving password configuration: cnt=%d, value={", uteModulePasswordData.password_cnt);
    for (int i = 0; i < uteModulePasswordData.password_cnt; i++)
    {
        printf("%d ", uteModulePasswordData.password_value[i]);
    }
    printf("}\n");

    uteModulePlatformTakeMutex(uteModulePasswordMute); // 加锁

    void *file;
    // 打开文件（假设总是成功）
    uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_PASSWORD_INFO, &file,
                                FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC);

    // 写入数据（假设总是成功）
    uteModuleFilesystemWriteData(file, &uteModulePasswordData, sizeof(ute_module_password_data_t));

    uteModuleFilesystemCloseFile(file);
    uteModulePlatformGiveMutex(uteModulePasswordMute); // 解锁
}

/**
* @brief 初始化密码模块
* @details 创建目录、初始化变量并读取配置。
*/
void uteModulePasswordInit(void)
{
    // printf("[PASSWORD] Initializing password module...\n");

    // 创建密码数据目录
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_SYSTEMPARM_PASSWORD_DATA_DIR);

    // 初始化密码数据
    uteModulePasswordData.password_cnt = 0;
    uteModulePasswordData.password_change = 0;
    uteModulePasswordData.password_flag = false;

    // 创建互斥锁
    uteModulePlatformCreateMutex(&uteModulePasswordMute);

    // 读取已保存的配置
    uteModulePasswordDataReadConfig();
}

/**
* @brief 设置新密码
* @details 设置密码值并保存配置。
*/
void uteModuleSetPassword(const u8 *password, u8 len)
{
    if (len > UTE_MODULE_PASSWORD_LEN)
        len = UTE_MODULE_PASSWORD_LEN;

    // printf("[PASSWORD] Setting new password: len=%d, value={", len);
    for (int i = 0; i < len; i++)
    {
        printf("%d ", password[i]);
    }
    printf("}\n");

    uteModulePlatformTakeMutex(uteModulePasswordMute);
    uteModulePasswordData.password_cnt = len;
    memcpy(uteModulePasswordData.password_value, password, len);
    uteModulePlatformGiveMutex(uteModulePasswordMute);

    uteModulePasswordDataSaveConfig(); // 保存配置
}

/**
* @brief 验证密码
* @details 验证输入的密码是否正确。
*/
bool uteModuleVerifyPassword(const u8 *password, u8 len)
{
    bool result = false;

    //  printf("[PASSWORD] Verifying password: len=%d, value={", len);
    for (int i = 0; i < len; i++)
    {
        printf("%d ", password[i]);
    }
    printf("}\n");

    if (len != uteModulePasswordData.password_cnt)
    {
//       printf("[PASSWORD] Verify failed: length mismatch (expected %d)\n", uteModulePasswordData.password_cnt);
        return false;
    }

    uteModulePlatformTakeMutex(uteModulePasswordMute);
    result = (memcmp(uteModulePasswordData.password_value, password, len) == 0);
    uteModulePlatformGiveMutex(uteModulePasswordMute);

    // printf("[PASSWORD] Verify result: %s\n", result ? "SUCCESS" : "FAIL");
    return result;
}

/**
* @brief 启用/禁用密码锁
* @details 设置密码锁标志并保存配置。
*/
void uteModuleSetPasswordLock(bool enable)
{
    // printf("[PASSWORD] Setting password lock: %s\n", enable ? "ENABLED" : "DISABLED");

    uteModulePlatformTakeMutex(uteModulePasswordMute);
    uteModulePasswordData.password_flag = enable;
    uteModulePlatformGiveMutex(uteModulePasswordMute);

    uteModulePasswordDataSaveConfig(); // 保存配置
}

/**
* @brief 检查是否需要密码解锁
* @details 返回当前是否启用了密码锁。
*/
bool uteModuleIsPasswordLockEnabled(void)
{
    bool result;
    uteModulePlatformTakeMutex(uteModulePasswordMute);
    result = uteModulePasswordData.password_flag;
    uteModulePlatformGiveMutex(uteModulePasswordMute);

    // printf("[PASSWORD] Password lock enabled: %s\n", result ? "YES" : "NO");
    return result;
}