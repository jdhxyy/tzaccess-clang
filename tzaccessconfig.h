// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// �����ļ�
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_CONFIG_H
#define TZACCESS_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define TZACCESS_TAG "tzaccess"

// ���֡�ֽ���
#define TZACCESS_FRAME_MAX_LEN 512

// ���븸·�ɼ��.��λ:s
#define TZACCESS_APPLY_INTERVAL 10

// ���Ӽ��.��λ:s
#define TZACCESS_CONN_INTERVAL 30
// ������Ӵ���.�������Ӵ�����������·��IA��ַ,������·��
#define TZACCESS_CONN_NUM_MAX 5

// Ĭ�Ϻ�������ַ
#define TZACCESS_CORE_IA 0x2141000000000002
#define TZACCESS_CORE_IP {115, 28, 86, 171}
#define TZACCESS_CORE_PORT 12914

#pragma pack(1)

// ���ڵ���Ϣ
typedef struct {
    uint64_t IA;
    uint8_t IP[4];
    uint16_t Port;
    uint8_t Cost;
    bool IsConn;
    uint64_t Timestamp;
} ParentInfo;

#pragma pack()

extern ParentInfo Parent;

// �ڴ�id
extern int TZAccessMid;

// TZAccessSetLocalIA ���ñ���IA��ַ
void TZAccessSetLocalIA(uint64_t ia);

// TZAccessGetLocalIA ��ȡ����IA��ַ
uint64_t TZAccessGetLocalIA(void);

// TZAccessSetLocalPwd ��������
void TZAccessSetLocalPwd(char* pwd);

// TZAccessGetLocalPwd ��ȡ����
char* TZAccessGetLocalPwd(void);

// TZAccessGetCoreIA ��ȡ������IA��ַ
uint64_t TZAccessGetCoreIA(void);

// TZAccessGetCoreIP ��ȡ������IP��ַ
uint8_t* TZAccessGetCoreIP(void);

// TZAccessGetCorePort ��ȡ�������˿ں�
uint16_t TZAccessGetCorePort(void);

#endif
