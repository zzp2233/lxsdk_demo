
#ifndef _UTE_FLASH_64MBIT_1024K_MAP_H_
#define _UTE_FLASH_64MBIT_1024K_MAP_H_

#define UTE_FLASH_SIZE                                  0x00800000 //8m

#define UTE_BOOT1_ADDRESS                               0x00000000
#define UTE_BOOT1_SIZE                                  0x00001000  //4K Bytes
#define UTE_BOOT2_ADDRESS                               0x00001000
#define UTE_BOOT2_SIZE                                  0x00001000  //4K Bytes
#define UTE_CODE_ADDRESS                                0x00002000
#define UTE_CODE_SIZE                                   0x00104000  //1280K Bytes
#define UTE_UI_ADDRESS                                  0x00106000
#define UTE_UI_SIZE                                     0x00560000  //5028K Bytes
#define UTE_OTA_TMP_ADDRESS                             0x0066B000
#define UTE_OTA_TMP_SIZE                                0x000C0000  //832K Bytes  UTE_CODE_SIZE*0.65
#define UTE_MODULE_WATCHONLINE_START_ADDRESS            0x006FB000
#define UTE_MODULE_WATCHONLINE_MAX_SIZE                 0x00080000  //512K Bytes
#define UTE_MODULE_FILESYSTEM_START_ADDRESS             0x0077B000
#define UTE_MODULE_FILESYSTEM_MAX_SIZE                  0x0007F000  //508K Bytes
#define UTE_USER_PARAM_ADDRESS                          0x007FA000
#define UTE_USER_PARAM_SIZE                             0x00001000  //4K Bytes
#define UTE_FACTORY_PARAM_ADDRESS                       0x007FB000
#define UTE_FACTORY_PARAM_SIZE                          0x00005000  //20K Bytes

#define UTE_SYSTEM_USER_DATA_START_ADDRESS UTE_MODULE_WATCHONLINE_START_ADDRESS
#endif /* _UTE_FLASH_64MBIT_1024K_MAP_H_ */
