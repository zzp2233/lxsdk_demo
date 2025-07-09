#ifndef __BSP_PBAP_H
#define __BSP_PBAP_H

#define PBAP_MAX_NAME_LEN       20
#define PBAP_MAX_NUM_LEN        16
#define PBAP_MAX_DATE_LEN       16

typedef void (*pbap_sync_finish_cb_t)(void *info, u16 count);

enum
{
    PBAP_SYNC_LOCAL,    //ͬ����������
    PBAP_SYNC_SIM,      //ͬ��sim������
};

enum
{
    PBAP_OBJECT_PB,     //ͨѶ¼
    PBAP_OBJECT_FAV,    //�ղص���ϵ��
    PBAP_OBJECT_ICH,    //�����¼
    PBAP_OBJECT_OCH,    //ȥ���¼
    PBAP_OBJECT_MCH,    //δ������
    PBAP_OBJECT_CCH,    //���е�ͨ����¼
};

//btstack��appͨѶ�ã��û������޸�
typedef struct
{
    char name[160];
    char anum[20];
    char bnum[20];
} pbap_data_cb_t;

typedef struct
{
    char name[PBAP_MAX_NAME_LEN];
    char num[PBAP_MAX_NUM_LEN];
    char date[PBAP_MAX_DATE_LEN];
} pbap_pb_buf_t;

typedef struct
{
    u8 pb_size;
    u8 pb_idx;
    u16 pb_count;
    bool is_pb_syncing;
    bool is_pbap_connected;
    pbap_pb_buf_t *pb_buf;
    pbap_sync_finish_cb_t cb;
} pbap_param_t;

void bt_pbap_data_callback(u8 type, void *item);
void bt_pbap_event_handle(uint evt, u8 *params);
void bt_pbap_param_reset(void);

/** �жϵ�ǰ�Ƿ��ڶ�ȡpbap���� **/
bool bt_pbap_is_syncing(void);

/**
 * @brief ��ʼͬ����ȡpbap������
 * @param[in] object : PBAP_OBJECT_ICH, �����¼
                       PBAP_OBJECT_OCH, ȥ���¼
                       ...
 * @param[in] buf :    �������ݵ�buf
 * @param[in] count :  Ҫ��ȡ����������
 * @param[in] finish_cb : pbap����ͬ�����Ļص�
 **/
void bt_pbap_sync_start(u8 object, void *buf, u16 count, pbap_sync_finish_cb_t finish_cb);

void bt_pbap_sync_stop(void);
#endif // __BSP_PBAP_H
