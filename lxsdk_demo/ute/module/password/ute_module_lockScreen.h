#ifndef _UTE_MODULE_LOCK_SCREEN_H_
#define _UTE_MODULE_LOCK_SCREEN_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_PASSWORD_DATA_DIR  "password"
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_PASSWORD_INFO  "password/passwordInfo"
#define UTE_MODULE_PASSWORD_LEN                         4   /* 密码长度 */


typedef struct
{
    u8 password_cnt;                // 密码有效长度
    u8 password_value[UTE_MODULE_PASSWORD_LEN];  // 密码有效值
    u8 password_change;              // 改密码标志
    bool password_flag;              // 开启密码锁标志
} ute_module_password_data_t;
extern ute_module_password_data_t uteModulePasswordData;

// 函数声明
void uteModulePasswordInit(void);
void uteModulePasswordDataSaveConfig(void);
void uteModuleSetPassword(const u8 *password, u8 len);
bool uteModuleVerifyPassword(const u8 *password, u8 len);
void uteModuleSetPasswordLock(bool enable);
bool uteModuleIsPasswordLockEnabled(void);

#endif