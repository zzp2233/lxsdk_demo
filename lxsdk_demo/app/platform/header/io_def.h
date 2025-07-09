#ifndef __IO_DEF_H
#define __IO_DEF_H

#include "global.h"
#include "config.h"
#include "sfr.h"


#if (SD0_MAPPING == SD0MAP_G1)
    //SDCLK(PB0), SDCMD(PB1), SDDAT0(PB2)
    #define SD_MUX_IO_INIT()        {GPIOBDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOBCLR  = BIT(0);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBPU  &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_IO_INIT()            {GPIOBDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOBDIR &= ~BIT(0);\
                                    GPIOBDIR |= BIT(1) | BIT(2);\
                                    GPIOBPU  |= BIT(1) | BIT(2);\
                                    GPIOBFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G1;}
    #define SD_CLK_DIR_IN()         {GPIOBDIR |= BIT(0); GPIOBPU  |= BIT(0);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOBDIR |= BIT(0); GPIOBPU  &= ~BIT(0);}
    #define SD_CLK_DIR_OUT()        {GPIOBPU  &= ~BIT(0); GPIOBDIR &= ~BIT(0);}
    #define SD_MUX_DETECT_INIT()    {GPIOBDE |= BIT(0); GPIOBPU  |= BIT(0); GPIOBDIR |= BIT(0);}
    #define SD_MUX_IS_ONLINE()      ((GPIOB & BIT(0)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOBDIR & BIT(0)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOBPU300 & BIT(1))
    #define SD_CMD_MUX_PU300R()     {GPIOBPU300 |= BIT(1); GPIOBPU &= ~BIT(1);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(1); GPIOBPU300 &= ~BIT(1);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(2); GPIOBPU &= ~BIT(2);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(2); GPIOBPU300 &= ~BIT(2);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(1)) == 0)

    #define SD_CLK_OUT_H()          {GPIOBSET = BIT(0);}
    #define SD_CLK_OUT_L()          {GPIOBCLR = BIT(0);}
    #define SD_CLK_STA()            (GPIOB & BIT(0))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(2); GPIOBDIR &= ~BIT(2);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(2); GPIOBPU  |= BIT(2);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(2);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(2);}
    #define SD_DAT_STA()            (GPIOB & BIT(2))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(1); GPIOADIR &= ~BIT(1);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(1); GPIOAPU  |= BIT(1);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(1);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(1);}
    #define SD_CMD_STA()            (GPIOB & BIT(1))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PB0
    #define SDCMD_IO                IO_PB1
    #define SDDAT_IO                IO_PB2

#elif (SD0_MAPPING == SD0MAP_G2)
    //SDCMD(PB6), SDCLK(PB7), SDDAT0(PB5)
    #define SD_MUX_IO_INIT()        {GPIOBDE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOBCLR  = BIT(7);\
                                    GPIOBDIR &= ~BIT(7);\
                                    GPIOBPU &= ~BIT(7);\
                                    GPIOBDIR |= BIT(6) | BIT(5);\
                                    GPIOBPU  |= BIT(6) | BIT(5);\
                                    GPIOBSET |= BIT(7) | BIT(6) | BIT(5);\
                                    GPIOBFEN |= BIT(7) | BIT(6) | BIT(5);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_IO_INIT()            {GPIOBDE |= BIT(5)|BIT(6)|BIT(7);\
                                    GPIOBDIR &= ~BIT(7);\
                                    GPIOBDIR |= BIT(6) | BIT(5);\
                                    GPIOBPU  |= BIT(6) | BIT(5);\
                                    GPIOBFEN |= BIT(7) | BIT(6) | BIT(5);\
                                    FUNCMCON0 = SD0MAP_G2;}
    #define SD_CLK_DIR_IN()         {GPIOBDIR |= BIT(7); GPIOBPU  |= BIT(7);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOBDIR |= BIT(7); GPIOBPU  &= ~BIT(7);}
    #define SD_CLK_DIR_OUT()        {GPIOBPU  &= ~BIT(7); GPIOBDIR &= ~BIT(7);}
    #define SD_MUX_DETECT_INIT()    {GPIOBDE |= BIT(7); GPIOBPU  |= BIT(7); GPIOBDIR |= BIT(7);}
    #define SD_MUX_IS_ONLINE()      ((GPIOB & BIT(7)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOBDIR & BIT(7)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOBPU300 & BIT(6))
    #define SD_CMD_MUX_PU300R()     {GPIOBPU300 |= BIT(6); GPIOBPU &= ~BIT(6);}
    #define SD_CMD_MUX_PU10K()      {GPIOBPU |= BIT(6); GPIOBPU300 &= ~BIT(6);}
    #define SD_DAT_MUX_PU300R()     {GPIOBPU300 |= BIT(5); GPIOBPU &= ~BIT(5);}
    #define SD_DAT_MUX_PU10K()      {GPIOBPU |= BIT(5); GPIOBPU300 &= ~BIT(5);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOB & BIT(5)) == 0)

    #define SD_CLK_OUT_H()          {GPIOBSET = BIT(7);}
    #define SD_CLK_OUT_L()          {GPIOBCLR = BIT(7);}
    #define SD_CLK_STA()            (GPIOB & BIT(7))

    #define SD_DAT_DIR_OUT()        {GPIOBDE |= BIT(5); GPIOBDIR &= ~BIT(5);}
    #define SD_DAT_DIR_IN()         {GPIOBDIR |= BIT(5); GPIOBPU  |= BIT(5);}
    #define SD_DAT_OUT_H()          {GPIOBSET = BIT(5);}
    #define SD_DAT_OUT_L()          {GPIOBCLR = BIT(5);}
    #define SD_DAT_STA()            (GPIOB & BIT(5))

    #define SD_CMD_DIR_OUT()        {GPIOBDE |= BIT(6); GPIOBDIR &= ~BIT(6);}
    #define SD_CMD_DIR_IN()         {GPIOBDIR |= BIT(6); GPIOBPU  |= BIT(6);}
    #define SD_CMD_OUT_H()          {GPIOBSET = BIT(6);}
    #define SD_CMD_OUT_L()          {GPIOBCLR = BIT(6);}
    #define SD_CMD_STA()            (GPIOB & BIT(6))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PB7
    #define SDCMD_IO                IO_PB6
    #define SDDAT_IO                IO_PB5

#elif (SD0_MAPPING == SD0MAP_G3)
    //SDCMD(PE2), SDCLK(PE3), SDDAT0(PE1)
    #define SD_MUX_IO_INIT()        {GPIOEDE |= BIT(1)|BIT(2)|BIT(3);\
                                    GPIOECLR  = BIT(3);\
                                    GPIOEDIR &= ~BIT(3);\
                                    GPIOEPU  &= ~BIT(3);\
                                    GPIOEDIR |= BIT(1) | BIT(2);\
                                    GPIOEPU  |= BIT(1) | BIT(2);\
                                    GPIOEFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_IO_INIT()            {GPIOEDE |= BIT(1)|BIT(2)|BIT(3);\
                                    GPIOEDIR &= ~BIT(3);\
                                    GPIOEDIR |= BIT(1) | BIT(2);\
                                    GPIOEPU  |= BIT(1) | BIT(2);\
                                    GPIOEFEN |= BIT(1) | BIT(2);\
                                    FUNCMCON0 = SD0MAP_G3;}
    #define SD_CLK_DIR_IN()         {GPIOEDIR |= BIT(3); GPIOEPU  |= BIT(3);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOEDIR |= BIT(3); GPIOEPU  &= ~BIT(3);}
    #define SD_CLK_DIR_OUT()        {GPIOEPU  &= ~BIT(3); GPIOEDIR &= ~BIT(3);}
    #define SD_MUX_DETECT_INIT()    {GPIOEDE |= BIT(3); GPIOEPU  |= BIT(3); GPIOEDIR |= BIT(3);}
    #define SD_MUX_IS_ONLINE()      ((GPIOE & BIT(3)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOEDIR & BIT(3)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOEPU300 & BIT(2))
    #define SD_CMD_MUX_PU300R()     {GPIOEPU300 |= BIT(2); GPIOEPU &= ~BIT(2);}
    #define SD_CMD_MUX_PU10K()      {GPIOEPU |= BIT(2); GPIOEPU300 &= ~BIT(2);}
    #define SD_DAT_MUX_PU300R()     {GPIOEPU300 |= BIT(1); GPIOEPU &= ~BIT(1);}
    #define SD_DAT_MUX_PU10K()      {GPIOEPU |= BIT(1); GPIOEPU300 &= ~BIT(1);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOE & BIT(2)) == 0)

    #define SD_CLK_OUT_H()          {GPIOESET = BIT(3);}
    #define SD_CLK_OUT_L()          {GPIOECLR = BIT(3);}
    #define SD_CLK_STA()            (GPIOE & BIT(3))

    #define SD_DAT_DIR_OUT()        {GPIOEDE |= BIT(1); GPIOEDIR &= ~BIT(1);}
    #define SD_DAT_DIR_IN()         {GPIOEDIR |= BIT(1); GPIOEPU  |= BIT(1);}
    #define SD_DAT_OUT_H()          {GPIOESET = BIT(1);}
    #define SD_DAT_OUT_L()          {GPIOECLR = BIT(1);}
    #define SD_DAT_STA()            (GPIOE & BIT(1))

    #define SD_CMD_DIR_OUT()        {GPIOEDE |= BIT(2); GPIOEDIR &= ~BIT(2);}
    #define SD_CMD_DIR_IN()         {GPIOEDIR |= BIT(2); GPIOEPU  |= BIT(2);}
    #define SD_CMD_OUT_H()          {GPIOESET = BIT(2);}
    #define SD_CMD_OUT_L()          {GPIOECLR = BIT(2);}
    #define SD_CMD_STA()            (GPIOE & BIT(2))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PE3
    #define SDCMD_IO                IO_PE2
    #define SDDAT_IO                IO_PE1
#elif (SD0_MAPPING == SD0MAP_G4)
    //SDCMD(PH7), SDCLK(PH4), SDDAT0(PH5)
    #define SD_MUX_IO_INIT()        {GPIOHDE |= BIT(4)|BIT(5)|BIT(7);\
                                    GPIOHCLR  = BIT(4);\
                                    GPIOHDIR &= ~BIT(4);\
                                    GPIOHPU  &= ~BIT(4);\
                                    GPIOHDIR |= BIT(7) | BIT(5);\
                                    GPIOHPU  |= BIT(7) | BIT(5);\
                                    GPIOHFEN |= BIT(7) | BIT(5);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_IO_INIT()            {GPIOHDE |= BIT(4)|BIT(5)|BIT(7);\
                                    GPIOHDIR &= ~BIT(4);\
                                    GPIOHDIR |= BIT(7) | BIT(5);\
                                    GPIOHPU  |= BIT(7) | BIT(5);\
                                    GPIOHFEN |= BIT(7) | BIT(5);\
                                    FUNCMCON0 = SD0MAP_G4;}
    #define SD_CLK_DIR_IN()         {GPIOHDIR |= BIT(4); GPIOHPU  |= BIT(4);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOHDIR |= BIT(4); GPIOHPU  &= ~BIT(4);}
    #define SD_CLK_DIR_OUT()        {GPIOHPU  &= ~BIT(4); GPIOHDIR &= ~BIT(4);}
    #define SD_MUX_DETECT_INIT()    {GPIOHDE |= BIT(4); GPIOHPU  |= BIT(4); GPIOHDIR |= BIT(4);}
    #define SD_MUX_IS_ONLINE()      ((GPIOH & BIT(4)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOHDIR & BIT(4)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOHPU300 & BIT(7))
    #define SD_CMD_MUX_PU300R()     {GPIOHPU300 |= BIT(7); GPIOHPU &= ~BIT(7);}
    #define SD_CMD_MUX_PU10K()      {GPIOHPU |= BIT(7); GPIOHPU300 &= ~BIT(7);}
    #define SD_DAT_MUX_PU300R()     {GPIOHPU300 |= BIT(5); GPIOHPU &= ~BIT(5);}
    #define SD_DAT_MUX_PU10K()      {GPIOHPU |= BIT(5); GPIOHPU300 &= ~BIT(5);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOH & BIT(7)) == 0)

    #define SD_CLK_OUT_H()          {GPIOHSET = BIT(4);}
    #define SD_CLK_OUT_L()          {GPIOHCLR = BIT(4);}
    #define SD_CLK_STA()            (GPIOH & BIT(4))

    #define SD_DAT_DIR_OUT()        {GPIOHDE |= BIT(5); GPIOHDIR &= ~BIT(5);}
    #define SD_DAT_DIR_IN()         {GPIOHDIR |= BIT(5); GPIOHPU  |= BIT(5);}
    #define SD_DAT_OUT_H()          {GPIOHSET = BIT(5);}
    #define SD_DAT_OUT_L()          {GPIOHCLR = BIT(5);}
    #define SD_DAT_STA()            (GPIOH & BIT(5))

    #define SD_CMD_DIR_OUT()        {GPIOHDE |= BIT(7); GPIOHDIR &= ~BIT(7);}
    #define SD_CMD_DIR_IN()         {GPIOHDIR |= BIT(7); GPIOHPU  |= BIT(7);}
    #define SD_CMD_OUT_H()          {GPIOHSET = BIT(7);}
    #define SD_CMD_OUT_L()          {GPIOHCLR = BIT(7);}
    #define SD_CMD_STA()            (GPIOH & BIT(7))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PH4
    #define SDCMD_IO                IO_PH7
    #define SDDAT_IO                IO_PH5
#elif (SD0_MAPPING == SD0MAP_G5)
    //SDCMD(PH0), SDCLK(PH2), SDDAT0(PH1)
    #define SD_MUX_IO_INIT()        {GPIOHDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOHCLR  = BIT(2);\
                                    GPIOHDIR &= ~BIT(2);\
                                    GPIOHPU  &= ~BIT(2);\
                                    GPIOHDIR |= BIT(0) | BIT(1);\
                                    GPIOHPU  |= BIT(0) | BIT(1);\
                                    GPIOHFEN |= BIT(0) | BIT(1);\
                                    FUNCMCON0 = SD0MAP_G5;}
    #define SD_IO_INIT()            {GPIOHDE |= BIT(0)|BIT(1)|BIT(2);\
                                    GPIOHDIR &= ~BIT(2);\
                                    GPIOHDIR |= BIT(0) | BIT(1);\
                                    GPIOHPU  |= BIT(0) | BIT(1);\
                                    GPIOHFEN |= BIT(0) | BIT(1);\
                                    FUNCMCON0 = SD0MAP_G5;}
    #define SD_CLK_DIR_IN()         {GPIOHDIR |= BIT(2); GPIOHPU  |= BIT(2);}
    #define SD_CLK_IN_DIS_PU10K()   {GPIOHDIR |= BIT(2); GPIOHPU  &= ~BIT(2);}
    #define SD_CLK_DIR_OUT()        {GPIOHPU  &= ~BIT(2); GPIOHDIR &= ~BIT(2);}
    #define SD_MUX_DETECT_INIT()    {GPIOHDE |= BIT(2); GPIOHPU  |= BIT(2); GPIOHDIR |= BIT(2);}
    #define SD_MUX_IS_ONLINE()      ((GPIOH & BIT(2)) == 0)
    #define SD_MUX_IS_BUSY()        ((GPIOHDIR & BIT(2)) == 0)
    #define SD_MUX_CMD_IS_BUSY()    (GPIOHPU300 & BIT(0))
    #define SD_CMD_MUX_PU300R()     {GPIOHPU300 |= BIT(0); GPIOHPU &= ~BIT(0);}
    #define SD_CMD_MUX_PU10K()      {GPIOHPU |= BIT(0); GPIOHPU300 &= ~BIT(0);}
    #define SD_DAT_MUX_PU300R()     {GPIOHPU300 |= BIT(1); GPIOHPU &= ~BIT(1);}
    #define SD_DAT_MUX_PU10K()      {GPIOHPU |= BIT(1); GPIOHPU300 &= ~BIT(1);}
    #define SD_CMD_MUX_IS_ONLINE()  ((GPIOH & BIT(0)) == 0)

    #define SD_CLK_OUT_H()          {GPIOHSET = BIT(2);}
    #define SD_CLK_OUT_L()          {GPIOHCLR = BIT(2);}
    #define SD_CLK_STA()            (GPIOH & BIT(2))

    #define SD_DAT_DIR_OUT()        {GPIOHDE |= BIT(1); GPIOHDIR &= ~BIT(1);}
    #define SD_DAT_DIR_IN()         {GPIOHDIR |= BIT(1); GPIOHPU  |= BIT(1);}
    #define SD_DAT_OUT_H()          {GPIOHSET = BIT(1);}
    #define SD_DAT_OUT_L()          {GPIOHCLR = BIT(1);}
    #define SD_DAT_STA()            (GPIOH & BIT(1))

    #define SD_CMD_DIR_OUT()        {GPIOHDE |= BIT(0); GPIOHDIR &= ~BIT(0);}
    #define SD_CMD_DIR_IN()         {GPIOHDIR |= BIT(0); GPIOHPU  |= BIT(0);}
    #define SD_CMD_OUT_H()          {GPIOHSET = BIT(0);}
    #define SD_CMD_OUT_L()          {GPIOHCLR = BIT(0);}
    #define SD_CMD_STA()            (GPIOH & BIT(0))

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_PH2
    #define SDCMD_IO                IO_PH0
    #define SDDAT_IO                IO_PH1
#else
    #define SD_MUX_IO_INIT()
    #define SD_IO_INIT()
    #define SD_CLK_DIR_IN()
    #define SD_CLK_IN_DIS_PU10K()
    #define SD_CLK_DIR_OUT()
    #define SD_MUX_DETECT_INIT()
    #define SD_MUX_IS_ONLINE()      0
    #define SD_MUX_IS_BUSY()        0
    #define SD_MUX_CMD_IS_BUSY()    0
    #define SD_CMD_MUX_PU300R()
    #define SD_CMD_MUX_PU10K()
    #define SD_CMD_MUX_IS_ONLINE()  0

    #define SD_CLK_OUT_H()
    #define SD_CLK_OUT_L()
    #define SD_CLK_STA()            0

    #define SD_DAT_DIR_OUT()
    #define SD_DAT_DIR_IN()
    #define SD_DAT_OUT_H()
    #define SD_DAT_OUT_L()
    #define SD_DAT_STA()            0

    #define SD_CMD_DIR_OUT()
    #define SD_CMD_DIR_IN()
    #define SD_CMD_OUT_H()
    #define SD_CMD_OUT_L()
    #define SD_CMD_STA()            0

    #define SD_DAT_DIS_UP()
    #define SD_DAT_RES_UP()

    #define SDCLK_IO                IO_NONE
    #define SDCMD_IO                IO_NONE
    #define SDDAT_IO                IO_NONE
#endif


#endif //__IO_DEF_H

