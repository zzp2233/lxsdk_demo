#include "include.h"
#include "chsc6x_comp.h"
#include "chsc6x_ramcode.h"
#include "chsc6x_platform.h"
// #include "chsc6x_flash_boot.h"
#if PROJECT_AB281B_SUPPORT
#include "YCY_AB281_801_chsc6x_upd_[304_17]_V241_V3.h" //801
#else
// #include "YCY_AB281_S81pro_chsc6x_upd_[304_18]_V240_V2.h"//s81 pro
#include "YCY_JLE012_MB_304_19__V243_V3.h"
#endif
#include "chsc6x_main.h"

#define TXRX_ADDR       (0x9000)
#define CMD_ADDR        (0x9f00)
#define RSP_ADDR        (0x9f40)
//#define MAX_CHIP_ID     (10)
//unsigned char chsc6x_chip_name[MAX_CHIP_ID][20];
// static unsigned int g_chsc6x_cfg_ver = 0;
// static unsigned int g_chsc6x_boot_ver = 0;

// static unsigned int g_mccode;    /* 1:3536 */
// static unsigned int g_upgrade_flag = 0; /* 0:driver init upgrade, 1:OTA upgrade */
// static unsigned char g_i2c_addr;
// struct ts_fw_infos *g_pfw_infos;


static uint32_t g_chsc6x_cfg_ver = 0;
static uint32_t g_chsc6x_boot_ver = 0;
static uint8_t g_cfg_update_flag = 0;
static uint8_t g_boot_update_flag = 0;
static uint8_t g_no_upd_req_flag = 0;//no updatae require
static uint8_t g_force_update_flag = 0;
static uint32_t g_mccode;    /* 1:3536 */
static uint32_t g_upgrade_flag = 0; /* 0:driver init upgrade, 1:OTA upgrade */
struct ts_fw_infos *g_pfw_infos;
uint8_t g_i2c_addr = CHSC6X_I2C_ID ;
uint32_t BOOTCRC = 0;

#define LEN_CMD_CHK_TX_SCAP  10
#define ABS(a)   (((a)>0)?((a)):(-(a)))


struct chsc6x_updfile_header
{
    unsigned int sig;
    unsigned int resv;
    unsigned int n_cfg;
    unsigned int n_match;
    unsigned int len_cfg;
    unsigned int len_boot;
};

typedef enum
{
    RESET_NONE,
    HW_CMD_RESET,
    HW_ACTIVE_RESET,
    RESET_MAX
} chsc6x_reset_e;

typedef struct _test_cmd_wr
{
    /* offset 0; */
    unsigned char id;    /* cmd_id; */
    unsigned char idv;    /* inverse of cmd_id */
    unsigned short d0;    /* data 0 */
    unsigned short d1;    /* data 1 */
    unsigned short d2;    /* data 2 */
    /* offset 8; */
    unsigned char resv;    /* offset 8 */
    unsigned char tag;    /* offset 9 */
    unsigned short chk;    /* 16 bit checksum */
    unsigned short s2Pad0;    /*  */
    unsigned short s2Pad1;    /*  */
} ctp_tst_wr_t;

typedef struct _test_cmd_rd
{
    /* offset 0; */
    unsigned char id;    /* cmd_id; */
    unsigned char cc;    /* complete code */
    unsigned short d0;    /* data 0 */
    unsigned short sn;    /* session number */
    unsigned short chk;    /* 16 bit checksum */
} ctp_tst_rd_t;

typedef enum
{
    CMD_DUT_CLK_CALIB   = 0x95,
} cmdTypeID;
static ctp_tst_rd_t chsc_rsp_buffer = {0};
ctp_tst_rd_t* const pRsp = &chsc_rsp_buffer;

#define EPERM           (1)
#define DIRECTLY_MODE   (0x0)
#define DEDICATE_MODE   (0x1)
#define LEN_CMD_CHK_TX  (10)
#define LEN_CMD_PKG_TX  (16)
#define LEN_RSP_CHK_RX  (8)
#define MAX_BULK_SIZE   (1024)

/* to direct memory access mode */
static unsigned char cmd_2dma_42bd[6] = { /*0x42, 0xbd, */ 0x28, 0x35, 0xc1, 0x00, 0x35, 0xae };


/* RETURN:0->pass else->fail */
static int chsc6x_read_bytes_u16addr(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
    int ret = 0;
    int retry;
    unsigned short ofs_adr;
    int len = lenth;
    int rd_len = 0;
    int offset = 0;

    while (len > 0)
    {
        ofs_adr = adr + offset;
        if (len > MAX_IIC_RD_LEN)
        {
            rd_len = MAX_IIC_RD_LEN;
            len -= MAX_IIC_RD_LEN;
        }
        else
        {
            rd_len = len;
            len = 0;
        }

        retry = 0;
        while (chsc6x_read_bytes_u16addr_sub(id, ofs_adr, &rxbuf[offset], rd_len) != 0)
        {
            if (retry++ == 3)
            {
                ret = -1;
                break;
            }
        }
        offset += MAX_IIC_RD_LEN;
        if (ret < 0)
        {
            break;
        }
    }

    return ret;
}

/* RETURN:0->pass else->fail */
static int chsc6x_write_bytes_u16addr(unsigned char id, unsigned short adr, unsigned char *rxbuf, unsigned short lenth)
{
    int ret = 0;
    int retry;
    unsigned short ofs_adr;
    int len = lenth;
    int wr_len = 0;
    int offset = 0;

    while (len > 0)
    {
        ofs_adr = adr + offset;
        if (len > MAX_IIC_WR_LEN)
        {
            wr_len = MAX_IIC_WR_LEN;
            len -= MAX_IIC_WR_LEN;
        }
        else
        {
            wr_len = len;
            len = 0;
        }

        retry = 0;
        while (chsc6x_write_bytes_u16addr_sub(id, ofs_adr, &rxbuf[offset], wr_len) != 0)
        {
            if (retry++ == 3)
            {
                ret = -1;
                break;
            }
        }
        offset += MAX_IIC_WR_LEN;
        if (ret < 0)
        {
            break;
        }
    }

    return ret;
}


void chsc6x_tp_reset_delay(chsc6x_reset_e type)
{
    switch (type)
    {
        case HW_CMD_RESET:
            // chsc6x_msleep(30);
            chsc6x_tp_reset();
            break;

        case HW_ACTIVE_RESET:
            // chsc6x_msleep(2);
            chsc6x_tp_reset_active();
            break;

        default:
            break;
    }

}

/* <0 : i2c error */
/* 0: direct address mode */
/* 1: protect mode */
static int chsc6x_get_i2cmode(void)
{
    uint8_t regData[4];

    if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x01, regData, 3))
    {
        return -OS_TPERROR;
    }

    if ((0x5c == regData[0]) && (regData[2] == 0X01))
    {
        return DIRECTLY_MODE;
    }

    return DEDICATE_MODE;
}

/* 0:successful */
int chsc6x_set_dd_mode(void)
{
    int mod = -1;
    int ret = 0;
    uint8_t retry = 0;

    ret = chsc6x_get_i2cmode();
    if (ret < 0)
    {
        return ret;
    }
    if (ret == DIRECTLY_MODE)
    {
        return 0;
    }
    while (retry++ < TP_RETRY_CNT3)
    {
        chsc6x_msleep(20);
        chsc6x_write_bytes_u16addr(g_i2c_addr, 0x42bd, cmd_2dma_42bd, 6);
        chsc6x_msleep(30);
        mod = chsc6x_get_i2cmode();
        if (mod == DIRECTLY_MODE)
        {
            break;
        }
    }

    return (mod == DIRECTLY_MODE) ? OS_OK : -OS_TPERROR;
}

/* ret=0 : successful */
/* write with read-back check, in dd mode */
int chsc6x_bulk_down_check(uint8_t *pbuf, uint16_t addr, uint16_t len)
{
    uint32_t j, k, retry;
    uint8_t rback[128];

    while (len)
    {
        k = (len < 128) ? len : 128;
        retry = 0;
        do
        {
            rback[k - 1] = pbuf[k - 1] + 1;
            chsc6x_write_bytes_u16addr(g_i2c_addr, addr, pbuf, k);
            chsc6x_read_bytes_u16addr(g_i2c_addr, addr, rback, k);
            for (j = 0; j < k; j++)
            {
                if (pbuf[j] != rback[j])
                {
                    break;
                }
            }
            if (j >= k)
            {
                break;    /* match */
            }
        }
        while (++retry < 3);

        if (j < k)
        {
            break;
        }

        addr += k;
        pbuf += k;
        len -= k;
    }

    return (int)len;
}

static uint16_t chsc6x_checksum_u16(uint16_t *buf, uint16_t length)
{
    uint16_t sum, len, i;

    sum = 0;
    len = length >> 1;
    for (i = 0; i < len; i++)
    {
        sum += buf[i];
    }

    return sum;
}

static uint32_t chsc6x_checksumEx(uint8_t *buf, uint16_t length)
{
    uint32_t combChk;
    uint16_t k, check, checkEx;

    check = 0;
    checkEx = 0;
    for (k = 0; k < length; k++)
    {
        WDT_CLR();
        check += buf[k];
        checkEx += (uint16_t) (k * buf[k]);
    }
    combChk = (checkEx << 16) | check;

    return combChk;
}

/* 0:successful */
static int chsc6x_download_ramcode(const uint8_t *pcode, uint16_t len)
{
    uint8_t dwr, retry, regData;
    int ret = -2;
    int sig;
    const uint8_t * ramcode;
    ramcode = pcode;

    if (chsc6x_set_dd_mode())
    {
        return -OS_TPERROR;
    }

    sig = (int) ramcode[3];
    sig = (sig<<8) + (int) ramcode[2];
    sig = (sig<<8) + (int) ramcode[1];
    sig = (sig<<8) + (int) ramcode[0];

    if (sig == 0x6d6f8008)
    {
        sig = 0;
        chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8000, (uint8_t *) &sig, 4);
        if (sig == 0x6d6f8008)
        {
            return 0;
        }
    }

    regData = 0x66;
    //Hal_Write_Packet(0x040f, &regData, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x040f, (uint8_t *)&regData, 1);
    regData = 0x00;
    //Hal_Write_Packet(0x0622, &regData, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0622, (uint8_t *)&regData, 1);

    dwr = 0x05;
    if (0 == chsc6x_bulk_down_check(&dwr, 0x0602, 1))/* stop mcu */
    {
        dwr = 0x00;
        chsc6x_bulk_down_check(&dwr, 0x0643, 1);/* disable irq */

        regData = 0x00;
        chsc6x_bulk_down_check(&dwr, 0x0068, 1);
    }
    else
    {
        return -OS_TPERROR;
    }
    if (0 == chsc6x_bulk_down_check((uint8_t *)ramcode, 0x8000, len))
    {
        dwr = 0x88;
        retry = 0;
        do
        {
            ret = chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0602, &dwr, 1);
        }
        while ((++retry < 3) && (ret != 0));
    }

    chsc6x_msleep(30);// let caller decide the delay time ?

    return ret;
}

/* return 0=successful: send cmd and get rsp. */
static int chsc6x_cmd_send(ctp_tst_wr_t *ptchcw, ctp_tst_rd_t *pcr)
{
    int ret;
    uint32_t retry;

    retry = 0;
    chsc6x_write_bytes_u16addr(g_i2c_addr, RSP_ADDR, (uint8_t *) &retry, 1);

    /* send command */
    ptchcw->idv = ~(ptchcw->id);
    ptchcw->tag = 0x35;
    ptchcw->chk = 1 + ~(chsc6x_checksum_u16((uint16_t *) ptchcw, LEN_CMD_CHK_TX));
    ptchcw->tag = 0x30;
    ret = chsc6x_write_bytes_u16addr(g_i2c_addr, CMD_ADDR, (uint8_t *) ptchcw, LEN_CMD_PKG_TX);
    if (ret)
    {
        goto exit;
    }
    ptchcw->tag = 0x35;
    ret = chsc6x_write_bytes_u16addr(g_i2c_addr, CMD_ADDR + 9, (uint8_t *) &(ptchcw->tag), 1);
    if (ret)
    {
        goto exit;
    }
    /* polling rsp, the caller must init rsp buffer. */
    ret = -1;
    retry = 0;
    while (retry++ < 100)/* 2s */
    {
        chsc6x_msleep(20);
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, RSP_ADDR, (uint8_t *) pcr, 1))
        {
            break;
        }

        if (ptchcw->id != pcr->id)
        {
            continue;
        }
        /* chsc6x_msleep(50); */
        chsc6x_read_bytes_u16addr(g_i2c_addr, RSP_ADDR, (uint8_t *) pcr, LEN_RSP_CHK_RX);
        if (!chsc6x_checksum_u16((uint16_t *) pcr, LEN_RSP_CHK_RX))
        {
            if ((ptchcw->id == pcr->id) && (0 == pcr->cc))
            {
                ret = 0;
            }
        }
        break;
    }
exit:
    return ret;

}

/* return 0=successful */
static int chsc6x_read_burn_space(uint8_t *pdes, uint16_t adr, uint16_t len)
{
    int rsp = -1;
    uint32_t left = len;
    uint32_t combChk, retry;
    ctp_tst_wr_t m_cmd;
    ctp_tst_rd_t m_rsp;

    m_cmd.id = 0x31;
    m_cmd.resv = 0x03;
    while (left)
    {
        len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;

        m_cmd.d0 = adr;
        m_cmd.d1 = len;
        m_cmd.d2 = 0x0;

        rsp = -1;
        retry = 0;
        while (retry++ < 3)
        {
            m_rsp.id = 0;
            if (chsc6x_cmd_send(&m_cmd, &m_rsp) == 0X0)
            {
                chsc6x_read_bytes_u16addr(g_i2c_addr, TXRX_ADDR, pdes, len);
                combChk = chsc6x_checksumEx(pdes, len);
                if (m_rsp.d0 == (uint16_t)combChk)
                {
                    if (m_rsp.sn == (uint16_t)(combChk >> 16))
                    {
                        rsp = 0;
                        break;
                    }
                }
            }
        }

        if (rsp < 0)
        {
            break;
        }
        left -= len;
        adr += len;
        pdes += len;
    }

    return rsp;
}

static int chsc6x_write_burn_space(uint8_t *psrc, uint16_t adr, uint16_t len)
{
    int rsp = 0;
    uint16_t left = len;
    uint32_t retry, combChk;
    ctp_tst_wr_t m_cmd;
    ctp_tst_rd_t m_rsp;

    m_cmd.id = 0x30;
    m_cmd.resv = 0x11;

    while (left)
    {
        len = (left > MAX_BULK_SIZE) ? MAX_BULK_SIZE : left;
        combChk = chsc6x_checksumEx(psrc, len);

        m_cmd.d0 = adr;
        m_cmd.d1 = len;
        m_cmd.d2 = (uint16_t) combChk;
        m_cmd.s2Pad0 = (uint16_t) (combChk >> 16);

        rsp = -1;    /* avoid dead loop */
        retry = 0;
        while (retry < 3)
        {
            chsc6x_write_bytes_u16addr(g_i2c_addr, TXRX_ADDR, psrc, len);
            m_rsp.id = 0;
            rsp = chsc6x_cmd_send(&m_cmd, &m_rsp);
            if (rsp < 0)
            {
                if ((m_rsp.d0 == 0X05) && (m_rsp.cc == 0X09))/* fotal error */
                {
                    break;
                }
                retry++;
            }
            else
            {
                left -= len;
                adr += len;
                psrc += len;
                break;
            }
        }

        if (rsp < 0)
        {
            break;
        }
    }

    return (!left) ? 0 : -1;
}

static int is_valid_cfg_data(uint16_t *ptcfg)
{
    if (0 == ptcfg)
    {
        return 0;
    }

    if ((uint8_t) (ptcfg[53] & 0xff) != 0x5c)
    {
        chsc6x_err("chsc6x: cfg iic adr error! \r\n");
        return 0;
    }

    if (chsc6x_checksum_u16(ptcfg, 204))
    {
        chsc6x_err("chsc6x: cfg chk_sum error! \r\n");
        return 0;
    }

    return 1;
}

/*return: 1 allow; 0 no-allow*/
static int is_tpcfg_update_allow(uint16_t *ptcfg)
{
    uint32_t u32tmp;
    uint16_t vnow, vbuild;



    if (0 == is_valid_cfg_data(ptcfg))
    {
        return 0;
    }

    u32tmp = ptcfg[1];
    u32tmp = (u32tmp << 16) | ptcfg[0];
#if DEFAULT_TP_UPDATE_VER_CHECKOUT_OPEN
    if (((g_chsc6x_cfg_ver & 0x3ffffff) != (u32tmp & 0x3ffffff)) && (0 == g_force_update_flag))
    {
        chsc6x_info("chsc6x: prj info not match,now_cfg=0x%x:build_cfg=0x%x!\r\n",(g_chsc6x_cfg_ver&0x3ffffff), (u32tmp&0x3ffffff));
        return 0;
    }
#endif

    vnow = (g_chsc6x_cfg_ver >> 26) & 0x3f;
    vbuild = (u32tmp >> 26) & 0x3f;
    chsc6x_info("chsc6x: cfg_vnow: 0x%x,cfg_vbuild: 0x%x \r\n", vnow, vbuild);
#if DEFAULT_TP_UPDATE_VER_CHECKOUT_OPEN
    if (0 == g_upgrade_flag && vbuild <= vnow)
    {
        return 0; //driver init upgrade, must vbuild > vnow
    }
    if(1 == g_upgrade_flag && vbuild == vnow)
    {
        return 0; //OTA upgrade just vbuild != vnow
    }
#endif

    return 1;
}

static int chsc6x_update_fcomp_cfg(uint16_t *ptcfg)
{
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn)))
    {
        chsc6x_err("chsc6x: update fcomp_cfg: ram-code error! \r\n");
        return -OS_TPERROR;
    }

    if (chsc6x_write_burn_space((uint8_t *)ptcfg, 0x8000, 204))
    {
        chsc6x_err("chsc6x: update fcomp_cfg fail! \r\n");
        return -OS_TPERROR;
    }

    g_chsc6x_cfg_ver = (ptcfg[1] << 16) | ptcfg[0];
    g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;
    g_cfg_update_flag = 1;
    return 0;
}

static int chsc6x_update_fcomp_boot(uint8_t *pdata, uint16_t len)
{
    uint8_t buf[1] = {0x4b};
    uint8_t* ch = (uint8_t*)&BOOTCRC;
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn)))
    {
        chsc6x_err("chsc6x: update fcomp_boot: ram-code error! \r\n");
        return -OS_TPERROR;
    }

    //pdata[8] = 0xff;//const
    if (chsc6x_write_burn_space((uint8_t *)pdata, 0x00, len))
    {
        chsc6x_err("chsc6x: update fcomp_boot fail! \r\n");
        return -OS_TPERROR;
    }
#if 1 //const
    if (chsc6x_write_burn_space((uint8_t *)buf, 0x08, 1))
    {
        chsc6x_err("chsc6x:update fcomp_boot last sig-byte fail! \r\n");
        return -OS_TPERROR;
    }
#endif

    if (chsc6x_write_burn_space(ch, len, 4))//write crc
    {
        chsc6x_err("chsc6x:update fcomp_boot checksum fail! \r\n");
        return -OS_TPERROR;
    }

    g_chsc6x_boot_ver = pdata[5];
    g_chsc6x_boot_ver = (g_chsc6x_boot_ver<<8) + pdata[4];
    g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
    g_boot_update_flag = 1;
    chsc6x_info("chsc6x: fcomp_boot update pass! \r\n");

    return 0;
}


/*return: 0 SUCESS else FAIL*/
static int chsc6x_get_running_cfg(uint16_t *ptcfg, uint16_t addr)
{
    int retry, err_type;
    retry = 0;
    err_type = 0;

    chsc6x_set_dd_mode();
    while (++retry < TP_RETRY_CNT2)
    {
        err_type = 0;
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, addr, (uint8_t *) ptcfg, 204))
        {
            chsc6x_msleep(20);
            err_type = 2;    /* i2c error */
            continue;
        }

        if (0 == is_valid_cfg_data(ptcfg))
        {
            chsc6x_set_dd_mode();
            err_type = 1;    /* data error or no data */
            chsc6x_msleep(20);
            continue;
        }
        break;
    }

    return err_type;

}

#if 0
/*find factory first burnning cfg info*/
/*return: 0 SUCESS else FAIL*/
static int chsc6x_find_ver(void)
{
    uint16_t buf_tmpcfg[102];
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn)))
    {
        chsc6x_err("chsc6x: find factory cfg: ram-code error! \r\n");
        return -OS_TPERROR;
    }

    if (chsc6x_read_burn_space((uint8_t *) buf_tmpcfg, 0xf000, 204))
    {
        chsc6x_err("chsc6x: read factory cfg fail! \r\n");
        return -OS_TPERROR;
    }

    if (is_valid_cfg_data(buf_tmpcfg))
    {
        chsc6x_info("chsc6x: read factory cfg OK. \r\n");
        g_chsc6x_cfg_ver = (uint32_t)buf_tmpcfg[1];
        g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (uint32_t)buf_tmpcfg[0];
        g_chsc6x_cfg_ver = g_chsc6x_cfg_ver&0x3ffffff;
        g_pfw_infos->chsc6x_cfg_version = 0;
        g_pfw_infos->chsc6x_vendor_id = (((g_chsc6x_cfg_ver>>9)&0x7F) | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
        g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
    }

    return 0;
}
#endif

static int chsc6x_cfg_update(uint16_t *parray, uint32_t cfg_num)
{
    uint32_t  k;
    int new_idx_active = -1;

    chsc6x_info("chsc6x: g_chsc6x_cfg_ver is 0x%x \r\n",g_chsc6x_cfg_ver);



    for (k = 0; k < cfg_num; k++)
    {
        if (1 == is_tpcfg_update_allow(parray))
        {
            new_idx_active = k;
            chsc6x_info("chsc6x: new_idx_active is %d \r\n", new_idx_active);
            break;
        }
        parray = parray + 102;
    }

    if (new_idx_active < 0)
    {
        chsc6x_info("chsc6x: 3536 cfg not need update! \r\n");
        g_no_upd_req_flag = 1;
        return -OS_TPERROR;
    }

    if (chsc6x_set_dd_mode())
    {
        chsc6x_err("chsc6x: cfg update error:can't control hw mode! \r\n");
        return -OS_TPERROR;
    }

    if (0 == chsc6x_update_fcomp_cfg(parray))
    {
        chsc6x_info("chsc6x: fcomp_cfg update pass! \r\n");
    }
    else
    {
        chsc6x_err("chsc6x: fcomp_cfg update fail! \r\n");
    }

    return 0;        /* need hw reset */
}

static int chsc6x_boot_ver_comp(uint32_t ver)
{
    if (0 == g_chsc6x_boot_ver || g_force_update_flag)
    {
        return 1; //try to force update
    }
    chsc6x_info("chsc6x: boot_vnow: 0x%x,boot_vbuild: 0x%x \r\n", g_chsc6x_boot_ver, ver);
    if (0 == g_upgrade_flag && ver > g_chsc6x_boot_ver )
    {
        return 1; //driver init upgrade, must vbuild > vnow
    }
    if (1 == g_upgrade_flag && ver != g_chsc6x_boot_ver )
    {
        return 1; //OTA upgrade just vbuild != vnow */
    }
    return 0; //no-need update boot
}

/*return: 0,no-need update | update succeed; 1,update failed*/
static int chsc6x_boot_update(uint8_t *pdata, uint16_t boot_len)
{
    uint32_t ver = 0;

    ver = pdata[5];
    ver = (ver<<8) + pdata[4];

    if (0 == chsc6x_boot_ver_comp(ver))
    {
        chsc6x_info("chsc6x: 3536 boot not need update! \r\n");
        g_no_upd_req_flag = 1;
        return 0;
    }

    return  chsc6x_update_fcomp_boot(pdata, boot_len);
}

/*analysis the ".h fw" and check burn*/
/*return: 0 SUCESS else FAIL*/
static int chsc6x_update_compat_ctl(uint8_t *pupd, int len)
{
    uint32_t k;
    uint32_t n;
    uint32_t offset;
    uint32_t *vlist;

    int ret = -1;
    struct chsc6x_updfile_header *upd_header;

    if (len < sizeof(struct chsc6x_updfile_header))
    {
        return -OS_TPERROR;
    }

    upd_header = (struct chsc6x_updfile_header *) pupd;

    if (upd_header->sig != 0x43534843)
    {
        return -OS_TPERROR;
    }

    n = upd_header->n_cfg;
    offset = (upd_header->n_match * 4) + sizeof(struct chsc6x_updfile_header);

    if ((offset + upd_header->len_cfg + upd_header->len_boot) != len)
    {
        return -OS_TPERROR;
    }
    if ((n * 204) != upd_header->len_cfg)
    {
        return -OS_TPERROR;
    }
    if (n != 0)
    {
        chsc6x_cfg_update((uint16_t *) (pupd + offset), n);
    }

    n = upd_header->n_match;
    if (n != 0)
    {
        vlist = (uint32_t *) (pupd + sizeof(struct chsc6x_updfile_header));
        offset = offset + upd_header->len_cfg;
        for (k=0; k < n; k++)
        {
            if ((vlist[k] == (g_chsc6x_cfg_ver & 0xffffff)) || g_force_update_flag)
            {
                BOOTCRC = chsc6x_checksumEx((pupd + offset), upd_header->len_boot);
                chsc6x_info("chsc6x: boot checksum is %x. \r\n",BOOTCRC);
                ret = chsc6x_boot_update((pupd + offset), upd_header->len_boot);
                if(0 == ret)
                {
                    break;
                }
                else
                {
                    return -OS_TPERROR;
                }
            }
        }
    }
    return 0;
}

/*return: 0 SUCESS else FAIL*/
static int chsc6x_do_update_ifneed(uint8_t* p_fw_upd, uint32_t fw_len)
{
    const uint8_t *fupd;
    uint32_t fw_size;
    int ret = -1;

#if CHSC6X_MUL_VENDOR_UPGRADE
    if(41 == g_pfw_infos->chsc6x_vendor_id && 27 == g_pfw_infos->chsc6x_project_id)
    {
        fupd = chsc_boot_41_27;
        fw_size = sizeof(chsc_boot_41_27);
        ret = chsc6x_update_compat_ctl((uint8_t *) fupd, fw_size);
    }
    else if(29 == g_pfw_infos->chsc6x_vendor_id && 08 == g_pfw_infos->chsc6x_project_id)
    {
        fupd = chsc_boot_29_08;
        fw_size = sizeof(chsc_boot_29_08);
        ret = chsc6x_update_compat_ctl((uint8_t *) fupd, fw_size);
    }
#else
    if(1 == g_upgrade_flag)   //ota upgrade
    {
        fupd = p_fw_upd;
        fw_size = fw_len;
    }
    else
    {
#ifdef UTE_DRV_TP_FIRMWARE_ADDRESS
        fupd = (const uint8_t *)UTE_DRV_TP_FIRMWARE_ADDRESS;
        fw_size = UTE_DRV_TP_FIRMWARE_SIZE;
#else
        fupd = chsc_boot;
        fw_size = sizeof(chsc_boot);
#endif
    }
    ret = chsc6x_update_compat_ctl((uint8_t *) fupd, fw_size);
#endif

    return ret;
}

static void chsc6x_tp_mccode(void)
{
    uint32_t tmp[3];

    g_mccode = 0xff;

    if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8000, (uint8_t *) tmp, 12))
    {
        chsc6x_err("chsc6x: read 0x8000-12 fail! \r\n");
        return;
    }
    if ( 0x544c4e4b == tmp[2] )    /*  boot code  */
    {
        if (tmp[0] == 0x35368008)
        {
            g_mccode = 1;
            g_chsc6x_boot_ver = tmp[1] & 0xffff;
            g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
            chsc6x_info("chsc6x: read 0x8000-12 OK! \r\n");
        }
    }
    else if ( 0x544c4e00 == (tmp[2]&0xffffff00))
    {
        if (tmp[0] == 0x35368008)
        {
            g_mccode = 1;
            g_chsc6x_boot_ver = 0;
            g_pfw_infos->chsc6x_boot_version = g_chsc6x_boot_ver;
            chsc6x_err("chsc6x: IC run flag error! \r\n");
        }
    }
    else
    {
        /* none code */
        tmp[0] = 0;
        if (chsc6x_read_bytes_u16addr(g_i2c_addr, 0x09, (uint8_t *) tmp, 3))
        {
            chsc6x_err("chsc6x: read 0x09-3 fail! \r\n");
            return;
        }
        if (tmp[0] == 0x5c5c5c)
        {
            g_mccode = 1;
            chsc6x_info("chsc6x: empty ic! \r\n");
        }
    }
}

void chsc6x_get_normal_chip_id(uint8_t *id)
{
    uint8_t cnt = 0;
    uint8_t data_t[4] = {0};

    for(cnt=0; cnt < TP_RETRY_CNT; cnt++)
    {
        chsc6x_set_dd_mode();
        if(0 != chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e6a, data_t, 2))
        {
            continue;
        }
        if(CHSC6X_I2C_ID == data_t[0])
        {
            *id = data_t[0];
        }
        else
        {
            continue;
        }
        break;
    }
}

void chsc6x_get_boot_chip_id(uint8_t *id)
{
    uint8_t try_cnt = 0;
    uint8_t dwr=0x05;

    for(try_cnt=0; try_cnt<TP_RETRY_CNT2; try_cnt++)
    {
        if (chsc6x_set_dd_mode())
        {
            chsc6x_tp_reset_delay(HW_ACTIVE_RESET);
            if (OS_OK != chsc6x_bulk_down_check(&dwr, 0x0602, 1))
            {
                continue;
            }
            dwr = 0x00;
            chsc6x_bulk_down_check(&dwr, 0x0643, 1);
        }
        else
        {
            *id = CHSC6X_I2C_ID;
            break;
        }
    }
    if(try_cnt >= TP_RETRY_CNT2)
    {
        chsc6x_err("chsc6x: get_boot_chip_id set_direct_mode failed. \r\n");
    }
}

/* FUNC: get fw info in struct ts_fw_infos you can call this func anytime.
 PARM iic_addr: your iic addr.
 PARM pfw_infos: can get all fw infos in struct ts_fw_infos, after call this interface.
*/
void chsc6x_get_chip_info(struct ts_fw_infos *infos)
{
    int ret, cnt = 0;
    uint8_t data_t[4] = {0};

    for(cnt=0; cnt<TP_RETRY_CNT2; cnt++)
    {
        chsc6x_tp_reset_delay(HW_CMD_RESET);
        ret = chsc6x_set_dd_mode();
        if(0 != ret)
        {
            chsc6x_err("chsc6x: Change to dd mode failed! \r\n");
        }
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e00, data_t, 4);
        if ((0 != ret) || (0x0 == data_t[0]) || (0x0 == data_t[2]))
        {
            continue;
        }
        else
        {
            infos->chsc6x_cfg_version = data_t[3]>>2;
            infos->chsc6x_vendor_id = (data_t[1]>>1) | ((data_t[2]&0xc0)<<1);
            infos->chsc6x_project_id = (((data_t[1]&1)<<8) | data_t[0]) | ((data_t[2]&0x30)<<5);
            chsc6x_info("chsc6x: TP chip_info OK::fw_ver:%d, vendor_id:%d, prj_id:%d \r\n", infos->chsc6x_cfg_version, infos->chsc6x_vendor_id, infos->chsc6x_project_id);
        }
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x8004, data_t, 2);
        if(0 == ret)
        {
            infos->chsc6x_boot_version = (data_t[1]<<8)|data_t[0];
            chsc6x_info("chsc6x: boot_ver OK::0x%02x  \r\n", infos->chsc6x_boot_version);
        }
        else
        {
            chsc6x_err("chsc6x: Get boot_ver failed! \r\n");
            continue;
        }
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e4c, data_t, 4);
        if(0 == ret)
        {
            infos->chsc6x_rpt_lcd_x = (data_t[1]<<8) + data_t[0];
            infos->chsc6x_rpt_lcd_y = (data_t[3]<<8) + data_t[2];
            chsc6x_info("chsc6x: rpt_lcd_x & rpt_lcd_y OK::%d:%d \r\n", infos->chsc6x_rpt_lcd_x, infos->chsc6x_rpt_lcd_y);
        }
        else
        {
            chsc6x_err("chsc6x: Get rpt_lcd_x & rpt_lcd_y failed! \r\n");
            continue;
        }
        chsc6x_msleep(2);
        ret = chsc6x_read_bytes_u16addr(g_i2c_addr, 0x9e6a, data_t, 2);
        if(0 == ret)
        {
            infos->chsc6x_chip_id = data_t[0];
            infos->chsc6x_chip_type = data_t[1]&0xf;
            chsc6x_info("chsc6x: chsc6x_chip_id & chsc6x_chip_type OK::0x%02x:0x%x \r\n", infos->chsc6x_chip_id, infos->chsc6x_chip_type);
        }
        else
        {
            chsc6x_err("chsc6x: Get chsc6x_chip_id & chsc6x_chip_type failed! \r\n");
            continue;
        }
        break;
    }
    chsc6x_tp_reset_delay(HW_CMD_RESET);
    if(cnt >= 3)
    {
        chsc6x_err("chsc6x: Get chip_info failed! \r\n");
    }
}

int chsc_flash_bulk_write( uint16_t adr, uint8_t* data,uint16_t len )
{
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn)))
    {
        chsc6x_err("chsc_flash_bulk_write: ram-code error! \r\n");
        return -OS_TPERROR;
    }
    if (chsc6x_write_burn_space((uint8_t *)data, adr, len))
    {
        chsc6x_err("chsc6x: write addr %x fail! \r\n", adr);
        return -OS_TPERROR;
    }
    return OS_OK;
}

int chsc_flash_bulk_read(uint16_t adr, uint8_t* data, uint16_t len )
{
    if (chsc6x_download_ramcode(fw_fcode_burn, sizeof(fw_fcode_burn)))
    {
        chsc6x_err("chsc_flash_bulk_read: ram-code error! \r\n");
        return -OS_TPERROR;
    }

    if (chsc6x_read_burn_space(data, adr, len))
    {
        chsc6x_err("chsc6x: read addr %x fail! \r\n",adr);
        return -OS_TPERROR;
    }
    return OS_OK;
}

int lib_3536_alg_reg_sw(uint8_t addr, uint8_t data)
{
    uint16_t ustmp = 0, uschk = 0;

    if(chsc6x_set_dd_mode())
    {
        return -OS_TPERROR;
    }

    ustmp = (uint16_t)data;
    ustmp = (ustmp << 8)+(uint16_t)addr;

    //Hal_Write_Packet(0xb8, (uint8_t *)&ustmp, 2);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0xb8, (uint8_t *)&ustmp, 2);

    uschk = 0;
    //Hal_Read_Packet(0xb8, (uint8_t *)&uschk, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0xb8, (uint8_t *)&uschk, 1);

    if(uschk != (uint16_t)addr)
    {
        return -OS_TPERROR;
    }

    uschk = 0x60;
    //Hal_Write_Packet( 0xba, (uint8_t *)&uschk, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0xba, (uint8_t *)&uschk, 1);

    while(1)
    {
        //Hal_Read_Packet(0xba, (uint8_t *)&uschk, 1);
        chsc6x_read_bytes_u16addr(g_i2c_addr, 0xba, (uint8_t *)&uschk, 1);
        if(0 == (uschk & 1))break;
    }

    return OS_OK;

}

int lib_3536_alg_reg_sr(uint8_t addr, uint8_t *pdata)
{
    uint16_t ustmp = 0, uschk = 0;

    if(chsc6x_set_dd_mode())
    {
        return -OS_TPERROR;
    }

    ustmp = (uint16_t)addr;
    //Hal_Write_Packet(0xb8, (unsigned char *)&ustmp, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0xb8, (uint8_t *)&ustmp, 1);

    uschk = 0;
    //Hal_Read_Packet(0xb8, (uint8_t *)&uschk, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0xb8, (uint8_t *)&uschk, 1);

    if(ustmp != uschk)
    {
        return -OS_TPERROR;
    }

    uschk = 0x40;
    //Hal_Write_Packet(0xba, (uint8_t *)&uschk, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0xba, (uint8_t *)&uschk, 1);

    while(1)
    {
        //Hal_Read_Packet(0xba, (uint8_t *)&uschk, 1);
        chsc6x_read_bytes_u16addr(g_i2c_addr, 0xba, (uint8_t *)&uschk, 1);
        if(0 == (uschk & 1))break;
    }

    //Hal_Read_Packet(0xb9, (uint8_t *)&uschk, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0xb9, (uint8_t *)&uschk, 1);

    *pdata = (uint8_t)uschk;

    return 0;

}

uint32_t GetOscTrimTick3536( uint8_t osc48_trim, uint32_t dutTrimTick )
{
    uint32_t trimTick = 0;
    uint8_t osc48_chk = 0;
    uint8_t hwdog = 0, reg = 0;

    hwdog = 0x66;
    //Hal_Write_Packet(0x040f, (unsigned char *)&hwdog, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x040f, (uint8_t *)&hwdog, 1);

    lib_3536_alg_reg_sw(0x03, osc48_trim);

    lib_3536_alg_reg_sr(0x03, (unsigned char *)&osc48_chk);
    if(osc48_trim != osc48_chk)
    {
        return 0;
    }

    //Hal_Read_Packet(0x0620, (uint8_t*)&reg, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg, 1);
    if( 1 != reg )
    {
        reg = 1;
        //Hal_Write_Packet(0x0620, (uint8_t *)&reg, 1);
        chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg, 1);
    }

    chsc6x_msleep(5);

    trimTick = 0;
    //Hal_Write_Packet(0x0634, (uint8_t*)&trimTick, 4);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0634, (uint8_t *)&trimTick, 4);

    reg = 0x09;
    //Hal_Write_Packet(0x0620, (uint8_t*)&reg, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg, 1);

    chsc6x_msleep(25);

    reg = 0x01;
    //Hal_Write_Packet(0x0620, (uint8_t *)&reg, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg, 1);

    //Hal_Read_Packet(0x0634, (uint8_t *)&trimTick, 4);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0x0634, (uint8_t *)&trimTick, 4);

    return trimTick;
}

int Cmd_Tp_Calibrate3536(ctp_tst_wr_t* ptrWr)
{

    uint32_t trimTick = 0, trimTickBackUp = 0;
    uint32_t dutTrimTick = 0, iRetry = 0;
    uint8_t osc48_trim = 0x80, osc48_mark = 0x80, osc48_trimBackUp = 0;
    uint8_t reg068 = 0, reg620 = 0, hwdog = 0, reg = 0;


    dutTrimTick = (uint32_t)ptrWr->d1;
    dutTrimTick = ( dutTrimTick << 16 ) + (uint32_t)ptrWr->d0;

    dutTrimTick = dutTrimTick + 3000;

    if(chsc6x_set_dd_mode())
    {
        return -OS_TPERROR;
    }

    hwdog = 0x66;
    //Hal_Write_Packet(0x040f, (uint8_t *)&hwdog, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x040f, (uint8_t *)&hwdog, 1);

    //Hal_Read_Packet(0x0068, (uint8_t *)&reg068, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0x0068, (uint8_t *)&reg068, 1);

    //Hal_Read_Packet(0x0620, (uint8_t *)&reg620, 1);
    chsc6x_read_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg620, 1);

    reg = 0x00;
    //Hal_Write_Packet(0x0622, (uint8_t *)&reg, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0622, (uint8_t *)&reg, 1);
    //Hal_Write_Packet(0x0068, (uint8_t *)&reg, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0068, (uint8_t *)&reg, 1);

    reg = 0x01;
    //Hal_Write_Packet(0x0620, (uint8_t *)&reg, 1);//enable timer2?
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg, 1);

    for( iRetry = 0; iRetry < 20; iRetry++ )
    {
        if( 0 == osc48_mark )
        {
            trimTick = GetOscTrimTick3536( osc48_trim, dutTrimTick );
            break;
        }

        trimTick = GetOscTrimTick3536( osc48_trim, dutTrimTick );

        if( trimTick > dutTrimTick )
        {
            osc48_mark = osc48_mark>>1;
            osc48_trim = osc48_trim|osc48_mark;
        }
        else if( trimTick < dutTrimTick )
        {
            osc48_trim = osc48_trim & (~osc48_mark);
            osc48_mark = osc48_mark>>1;
            osc48_trim = osc48_trim|osc48_mark;
        }
        else
        {
            osc48_mark = 0;
            break;
        }
    }


    trimTickBackUp = trimTick;
    osc48_trimBackUp = osc48_trim;

    if( trimTick > dutTrimTick )
    {
        osc48_trim = osc48_trim + 1;
        trimTick = GetOscTrimTick3536( osc48_trim, dutTrimTick );
    }
    else
    {
        osc48_trim = osc48_trim - 1;
        trimTick = GetOscTrimTick3536( osc48_trim, dutTrimTick );
    }

    if( ABS( (int)trimTickBackUp - (int)dutTrimTick ) > ABS( (int)trimTick - (int)dutTrimTick ) )
    {
        osc48_trim = osc48_trim;
    }
    else
    {
        osc48_trim = osc48_trimBackUp;
    }

    //Hal_Write_Packet(0x0068, (uint8_t *)&reg068, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0068, (uint8_t *)&reg068, 1);

    //Hal_Write_Packet(0x0620, (uint8_t *)&reg620, 1);
    chsc6x_write_bytes_u16addr(g_i2c_addr, 0x0620, (uint8_t *)&reg620, 1);


    if( iRetry >= 20 )
    {
        pRsp->cc = OS_ERR_X;
    }
    else
    {
        pRsp->d0 = osc48_trim;
        pRsp->cc = OS_OK;
    }

    return OS_OK;
}

int chsc_6400_osc_trim(uint32_t target, uint8_t* ptrim_value)
{
    int iRet = OS_OK;
    uint8_t osc_trim = 0,diff = 0;
    short osc_trim1 = 0, osc_trim2 = 0;
    ctp_tst_wr_t chsc_m_cmd;
    uint8_t bulk_read[16] = {0};
    uint32_t* pIntData = NULL;
    uint16_t* pShortData = NULL;

    iRet = chsc_flash_bulk_read( 0xe000, bulk_read, 16 );
    if(iRet != OS_OK) return iRet;

    chsc_m_cmd.d0 = target & 0xffff;
    chsc_m_cmd.d1 = target >> 16;
    chsc_m_cmd.d2 = 250;

    chsc_m_cmd.id = CMD_DUT_CLK_CALIB;
    chsc_m_cmd.idv = ~(chsc_m_cmd.id);
    chsc_m_cmd.tag = 0x35;
    chsc_m_cmd.chk = 1+ ~(chsc6x_checksum_u16((unsigned short*)&chsc_m_cmd, LEN_CMD_CHK_TX_SCAP));

    iRet = Cmd_Tp_Calibrate3536(&chsc_m_cmd);
    if(iRet != OS_OK) return iRet;

    osc_trim1 = pRsp->d0 & 0xff;
    chsc6x_info( "chsc: osc_trim1 = 0x%2x\r\n",osc_trim1);

    iRet = Cmd_Tp_Calibrate3536(&chsc_m_cmd);
    if(iRet != OS_OK) return iRet;

    osc_trim2 = pRsp->d0 & 0xff;
    chsc6x_info( "chsc: osc_trim2 = 0x%2x\r\n",osc_trim2);

    if((osc_trim1 - osc_trim2) * (osc_trim1 - osc_trim2) > 49)
    {
        iRet =  -OS_TPERROR;
        chsc6x_info( "chsc: OSTrim Value Out Of Range\r\n");
        return iRet;
    }

    osc_trim = (osc_trim2 + osc_trim1)>>1;

    if(osc_trim <= 0x50 || osc_trim>=0xcf)
    {
        iRet = -OS_ERR_DCHECK;
        return iRet;
    }

    diff =  bulk_read[0] > osc_trim ? bulk_read[0] - osc_trim : osc_trim - bulk_read[0];
    if(diff < 7)
    {
        if(((~(bulk_read[0])&0xff)==(bulk_read[1])))
        {
            if((bulk_read[0]!=0x5a) && (bulk_read[0]!=0xa5))
            {
                *ptrim_value = bulk_read[0];
                chsc6x_info( "chsc: |Valid OSTrim = 0x%2x - Current OSTrim = 0x%2x|<7\r\n",bulk_read[0],osc_trim);
                return iRet;
            }

        }
        else
        {
            chsc6x_info( "chsc: 0x%2x + 0x%2x !=0xff\r\n",bulk_read[0], bulk_read[1]);
        }
    }
    else
    {
        chsc6x_info( "chsc: |Valid OSTrim = 0x%2x - Current OSTrim = 0x%2x|>=7\r\n",bulk_read[0],osc_trim);
    }

    bulk_read[0] = osc_trim;
    bulk_read[1] = ~osc_trim;
    //*ptrim_value = osc_trim;

    pIntData = (uint32_t*)&bulk_read[8];
    pShortData = (uint16_t*)&bulk_read[8];
    if(0xffff != (pShortData[2] + pShortData[3]))
    {
        pIntData[0] = 0x01020304;
        pShortData[2] = 0x1122;
        pShortData[3] = ~pShortData[2];
    }

    iRet = chsc_flash_bulk_write( 0xe000, bulk_read, 16 );
    if(iRet != OS_OK) return iRet;

    iRet = chsc_flash_bulk_read( 0xe000, bulk_read, 16 );
    if(iRet != OS_OK) return iRet;
    chsc6x_info( "chsc: write at 0xe000 OSTrim = 0x%2x \r\n",bulk_read[0]);
    *ptrim_value = bulk_read[0];

    return iRet;
}

int chsc6x_osc_trim(void)
{
    int ret = -OS_TPERROR;
    int i;

    chsc6x_info("enter chsc6x_osc_trim. \r\n");

    for(i = 0; i < TP_RETRY_CNT3; i++)
    {
        unsigned char trim_value = 0xff;
        ret = chsc_6400_osc_trim( 607500, &trim_value);
        if(OS_OK == ret)
        {
            if(trim_value>0x50 && trim_value < 0xcf)
            {
                chsc6x_info( "chsc: osc trim value = 0x%2x\r\n",trim_value);
                break;
            }
            else
            {
                chsc6x_info( "chsc: osc trim value = 0x%2x Not in scope 0x50-0xcf\r\n",trim_value);
            }
        }
    }

    return ret;

}


/* FUNC In your systerm init process,Must call this interface function to detec if the TP IC is Chipsemi corp'.
 PARM pfw_infos: to get top 5 fw info in struct ts_fw_infos.
 PARM update_ret_flag: point value=1 update succeed; point value=0 update failed, If opend CHSC6X_AUTO_UPGRADE macro.
 RETURN 1:is chsc chip, 0:is not chsc chip
 */
int chsc6x_tp_dect(struct ts_fw_infos *pfw_infos, uint8_t *update_ret_flag)
{
    uint8_t try_cnt;
    uint8_t dwr=0x05;
    uint16_t buf_tmpcfg[102];

    g_mccode = 0xff;        /* default */
    g_pfw_infos = pfw_infos;

    for(try_cnt=0; try_cnt<TP_RETRY_CNT2; try_cnt++)
    {
        if (chsc6x_set_dd_mode())
        {
            chsc6x_tp_reset_delay(HW_ACTIVE_RESET);
            if (0 != chsc6x_bulk_down_check(&dwr, 0x0602, 1))
            {
                continue;
            }
            dwr = 0x00;
            chsc6x_bulk_down_check(&dwr, 0x0643, 1);
        }
        else
        {
            break;
        }
    }
    if(try_cnt >= TP_RETRY_CNT2)
    {
        chsc6x_info("chsc6x: chsc6x_set_dd_mode failed! \r\n");//get chip id fail exit drivers init
        return 0;
    }
    chsc6x_tp_mccode();    /* MUST: call this function there!!! */
    chsc6x_info("chsc6x: g_mccode is 0x%x \r\n",g_mccode);

    if(0xff == g_mccode)
    {
        chsc6x_err("chsc6x: get mccode fail! \r\n");
        return 0;
    }

    /*try to get running time tp-cfg. if fail : wrong boot? wrong rom-cfg?*/
    if (0 == chsc6x_get_running_cfg(buf_tmpcfg, 0x9e00))
    {
        g_chsc6x_cfg_ver = (uint32_t)buf_tmpcfg[1];
        g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (uint32_t)buf_tmpcfg[0];

        g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;
        g_pfw_infos->chsc6x_vendor_id = (((g_chsc6x_cfg_ver>>9)&0x7F) | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
        g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
        g_pfw_infos->chsc6x_rpt_lcd_x = buf_tmpcfg[38];
        g_pfw_infos->chsc6x_rpt_lcd_y = buf_tmpcfg[39];
        g_pfw_infos->chsc6x_chip_id = buf_tmpcfg[53]&0xff;
        g_pfw_infos->chsc6x_chip_type = (buf_tmpcfg[53]>>8)&0xf;
        chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
                    g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id,g_pfw_infos->chsc6x_boot_version, \
                    g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
                   );
    }
    else
    {

        g_force_update_flag = 1;
        g_chsc6x_cfg_ver = 0;
        g_chsc6x_boot_ver = 0;
    }



#if CHSC6X_AUTO_UPGRADE
    chsc6x_info("enter CHSC6X_AUTO_UPGRADE. \r\n");
    if(0 != chsc6x_do_update_ifneed(0, 0))
    {
        *update_ret_flag = 0;
        chsc6x_err("chsc6x: do fw update failed! \r\n");
    }
    else
    {
        if(g_cfg_update_flag || g_boot_update_flag)
        {
            if (chsc6x_osc_trim())
            {
                chsc6x_err( "chsc: osc trim error\r\n" );
            }
            *update_ret_flag = 1;
            chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
                        g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id, \
                        g_pfw_infos->chsc6x_boot_version,g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
                       );
        }
        else if(g_no_upd_req_flag)
        {
            *update_ret_flag = 2;
        }
    }
#endif
    chsc6x_tp_reset_delay(HW_CMD_RESET);

    return 1;
}

/* FUNC You can call this interfacce function to realize upgrade TP Firmware by OTA.
 PARM pfw_infos: to get top 6 fw infos in struct ts_fw_infos, after ota upgrade.
 PARM p_fw_upd: array address of the upgrade firmware array
 PARM fw_len: total size of the upgrade firmware array
 RETURN NULL
*/
// void chsc6x_ota_upgrade_tp_fw(struct ts_fw_infos *pfw_infos, uint8_t* p_fw_upd, uint32_t fw_len)
// {
//     uint8_t try_cnt;
//     uint8_t dwr=0x05;
//     uint16_t buf_tmpcfg[102];

//     g_mccode = 0xff;        /* default */
//     g_pfw_infos = pfw_infos;

//     for(try_cnt=0; try_cnt<TP_RETRY_CNT2; try_cnt++)
//     {
//         if (chsc6x_set_dd_mode())
//         {
//             chsc6x_tp_reset_delay(HW_ACTIVE_RESET);
//             if (0 != chsc6x_bulk_down_check(&dwr, 0x0602, 1))
//             {
//                 continue;
//             }
//             dwr = 0x00;
//             chsc6x_bulk_down_check(&dwr, 0x0643, 1);
//         }
//         else
//         {
//             break;
//         }
//     }
//     if(try_cnt >= TP_RETRY_CNT2)
//     {
//         chsc6x_info("chsc6x: chsc6x_set_dd_mode failed! \r\n");
//         goto exit;
//     }
//     chsc6x_tp_mccode();    /* MUST: call this function there!!! */
//     chsc6x_info("chsc6x: g_mccode is 0x%x \r\n",g_mccode);

//     if(0xff == g_mccode)
//     {
//         chsc6x_err("chsc6x: get mccode fail! \r\n");
//         goto exit;
//     }

//     /*try to get running time tp-cfg. if fail : wrong boot? wrong rom-cfg?*/
//     if (0 == chsc6x_get_running_cfg(buf_tmpcfg, 0x9e00))
//     {
//         g_chsc6x_cfg_ver = (uint32_t)buf_tmpcfg[1];
//         g_chsc6x_cfg_ver = (g_chsc6x_cfg_ver<<16) + (uint32_t)buf_tmpcfg[0];

//         g_pfw_infos->chsc6x_cfg_version = g_chsc6x_cfg_ver>>26;
//         g_pfw_infos->chsc6x_vendor_id = (((g_chsc6x_cfg_ver>>9)&0x7F) | ((g_chsc6x_cfg_ver>>22&0x03)<<7));
//         g_pfw_infos->chsc6x_project_id = ((g_chsc6x_cfg_ver&0x01FF) | ((g_chsc6x_cfg_ver>>20&0x03)<<9));
//         g_pfw_infos->chsc6x_rpt_lcd_x = buf_tmpcfg[38];
//         g_pfw_infos->chsc6x_rpt_lcd_y = buf_tmpcfg[39];
//         g_pfw_infos->chsc6x_chip_id = buf_tmpcfg[53]&0xff;
//         g_pfw_infos->chsc6x_chip_type = (buf_tmpcfg[53]>>8)&0xf;
//         chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
//             g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id,g_pfw_infos->chsc6x_boot_version, \
//             g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
//         );
//     }
//     else
//     {
//          //chsc6x_find_ver();
//         g_force_update_flag = 1;
//         g_chsc6x_cfg_ver = 0;
//         g_chsc6x_boot_ver = 0;
//     }

//     if (0 == g_chsc6x_cfg_ver)
//     {
//         chsc6x_err("chsc6x: get tp-info fail! \r\n");
//         goto exit;
//     }

//     chsc6x_info("enter CHSC6X_OTA_UPGRADE. \r\n");
//     g_upgrade_flag = 1;
//     if(0 != chsc6x_do_update_ifneed(p_fw_upd, fw_len))
//     {
//         chsc6x_err("chsc6x: do fw update failed! \r\n");
//     }
//     else
//     {
//         if(g_cfg_update_flag || g_boot_update_flag)
//         {
//             if (chsc6x_osc_trim())
//             {
//                 chsc6x_err( "chsc: osc trim error\r\n" );
//             }
//             chsc6x_info("chsc6x: vid=%d,pid=%d,boot_ver=0x%x,cfg_ver=%d,chip_id=0x%x\r\n", \
//                 g_pfw_infos->chsc6x_vendor_id,g_pfw_infos->chsc6x_project_id, \
//                 g_pfw_infos->chsc6x_boot_version,g_pfw_infos->chsc6x_cfg_version,g_pfw_infos->chsc6x_chip_id \
//             );
//         }
//     }

//     exit:
//     chsc6x_tp_reset_delay(HW_CMD_RESET);
// }

