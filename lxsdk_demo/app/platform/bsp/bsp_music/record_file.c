#include "include.h"
#include "record_file.h"

#define SFN_NUM_LEN               4
#define MIC_SFN_NUM_OFS           3

extern unsigned char avio_buf[556];

//录音根目录
AT(.rodata.record.file)
const char root_dir_path[7] = "RECORD";

AT(.rodata.record.file)
const char file_name_mic[12]  = "MIC0001.mp3";


//打开录音目录
AT(.text.record.file)
bool record_directory_open(u8 *fname_buf, u32 size)
{
    FRESULT res;
    const char *path;
    u32 len;

    if (fname_buf == NULL) {
        return false;
    }
    memset(fname_buf, 0 ,size);
	path = root_dir_path;
    len = strlen(root_dir_path);
    res = fs_mkdir(path);
    if ((res == FR_OK) || (res == FR_EXIST)) {
        memcpy(fname_buf, path, len);
        fname_buf[len++] = '/';
        return true;
    }
    return false;

}

static bool record_file_sfn_inc(u8 *fn)
{
    u8 *ptr = fn + SFN_NUM_LEN - 1;

    *ptr += 1;
    for (u8 i = 0; i < (SFN_NUM_LEN - 1); i++) {
        if(*ptr > '9') {
            *ptr-- = '0';
            *ptr += 1;
        } else {
            return true;
        }
    }

    return (*ptr > '9')? false : true;
}

//打开录音文件
AT(.text.record.file)
bool record_file_open(u8 *fname_buf, u32 size, u32 rec_type)
{
    FRESULT res;

    if (fname_buf == NULL) {
        return false;
    }
    tm_t rtc_tm = time_to_tm(RTCCNT);
    u8 pos = strlen((char *)fname_buf);
    strcpy((char *)&fname_buf[pos], file_name_mic);
    if (rec_type == REC_WAV || rec_type == REC_ADPCM) {
        fname_buf[pos+8] = 'w';
        fname_buf[pos+9] = 'a';
        fname_buf[pos+10] = 'v';
    } else if (rec_type == REC_SBC) {
        fname_buf[pos+8] = 's';
        fname_buf[pos+9] = 'b';
        fname_buf[pos+10] = 'c';
    }
    pos += MIC_SFN_NUM_OFS;

    //fs_create_time_inc();
    fs_create_time_modi(rtc_tm.year, rtc_tm.mon, rtc_tm.day, rtc_tm.hour, rtc_tm.min, rtc_tm.sec);

    while (1) {
        res = fs_open((const char *)fname_buf, FA_WRITE|FA_CREATE_NEW);
        if (res == FR_OK) {
            break;
        } else if (res == FR_EXIST) {
            if (!record_file_sfn_inc(&fname_buf[pos])) {
                return false;
            }
        } else {
            return false;
        }
    }
    printf("%s: %s\n", __func__, fname_buf);
    return true;
}

bool record_file_write(u8 *buf, u32 rec_type)
{
    FRESULT res;
    res = fs_write(buf, 512);
    if (res != FR_OK) {
        if (res == FR_NOT_ENOUGH_CORE) {
            printf("record disk full\n");
        } else {
            printf("record disk failed: %d\n", res);
        }
        return false;
    }
    return true;
}

bool record_file_close(u32 rec_type)
{
#if (REC_WAV_SUPPORT || REC_ADPCM_SUPPORT)
    if (!record_wav_header_sync(rec_type)) {
        return false;
    }
#endif
    if (fs_close() != FR_OK) {
        return false;
    }
    return true;
}

//同步更新目录项
bool record_file_sync(u32 rec_type)
{
#if (REC_WAV_SUPPORT || REC_ADPCM_SUPPORT)
    if (!record_wav_header_sync(rec_type)) {
        return false;
    }
#endif
    if (fs_sync() != FR_OK) {
        return false;
    }
    return true;
}

//判断是否录音文件夹
AT(.text.fs.scan)
bool is_record_dir(char *sfn)
{
    if (!memcmp(sfn, root_dir_path, strlen(root_dir_path))) {
        return true;
    }
    return false;
}

#if (REC_WAV_SUPPORT || REC_ADPCM_SUPPORT)

static u8 wav_header_buf[512] AT(.rec_buf.wav);
static u32 wav_header_fssect AT(.rec_buf.wav);

AT(.rodata.record.file)
const u8 wav_header_tbl[52] = {
    0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
	0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
	0x04, 0x00, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x66, 0x61, 0x63, 0x74, 0xC8, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

bool record_wav_init(u32 nch, u32 spr, u32 rec_type)
{
    u8 *wbuf = avio_buf;
    wav_header_t *wavhead = (wav_header_t *)wav_header_buf;

    memset(wav_header_buf, 0, 512);
    memcpy(wav_header_buf, wav_header_tbl, sizeof(wav_header_tbl));
    wavhead->wFormatTag = WAVE_FORMAT_PCM;
    wavhead->nChannels  = nch & 0x03;
    wavhead->nSamplesPerSec  = tbl_sample_rate[spr];
    wavhead->nAvgBytesPerSec = (wavhead->nSamplesPerSec * wavhead->nChannels * PCM_BITS) / 8;
    wavhead->nBlockAlign = (wavhead->nChannels * PCM_BITS) / 8;
    wavhead->wBitsPerSample = PCM_BITS;
    wavhead->data_id = DATA_CKID;
#if REC_ADPCM_SUPPORT
    if (rec_type == REC_ADPCM) {
        wavhead->wFormatTag = WAVE_FORMAT_DVI_ADPCM;
        wavhead->nAvgBytesPerSec = (wavhead->nSamplesPerSec * wavhead->nChannels * 4) / 8;
        wavhead->wValidBitsPerSample = (128 - 4 * wavhead->nChannels) * 2 / wavhead->nChannels + 1;		//adpcm nSamplesPerBlock
        wavhead->wBitsPerSample = 4;
        wavhead->nBlockAlign = 128;
    }
#endif

    memcpy(wbuf, wav_header_buf, 512);
    if (fs_write(wbuf, 512) != FR_OK) {
        return false;
    }
    wav_header_fssect = fs_get_file_ssect();                    //录音文件起始sector地址
    return true;
}

bool record_wav_header_sync(u32 rec_type)
{
    if (rec_type == REC_WAV || rec_type == REC_ADPCM) {
        u8 *wbuf = avio_buf;
        wav_header_t *wavhead = (wav_header_t *)wav_header_buf;
        u32 fsize = fs_get_file_size();
        wavhead->riff_size = fsize - 8;
        wavhead->data_size = fsize - 512;
        wavhead->dwSampleLength = (fsize - 512)/wavhead->nBlockAlign;  //Number of samples
        memcpy(wbuf, wav_header_buf, 512);
        if (disk_writep(wbuf, wav_header_fssect)) {
            return false;
        }
    }
    return true;
}
#endif


