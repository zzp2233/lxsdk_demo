#include "config.h"
depend(0x01030600);
setresmode(2, 0, UI_EX_, UI_EX_ADDR_BASE);
loadresdir(ui_external);
makeres(ui_external_buf);
makeresdef(ui_external.h);
xcopy(ui_external.h, ../../ui_external.h);
save(ui_external_buf, ui_external.bin);