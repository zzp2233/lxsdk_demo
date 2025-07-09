#ifndef __UTE_MODULE_MEM_H__
#define __UTE_MODULE_MEM_H__
#include "include.h"
#include "ute_project_config.h"

#if UTE_MODULE_USER_MALLOC_SUPPORT 

void uteModulePlatformMemoryInitPool(void);
void *uteModulePlatformMemoryAlloc(size_t size);
void uteModulePlatformMemoryFree(void *ptr);
size_t uteModulePlatformMemoryGetFree(void);

#endif

#endif

