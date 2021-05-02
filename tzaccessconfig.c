// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// �����ļ�
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessconfig.h"

#include "lagan.h"
#include "tzmalloc.h"

#include <stdio.h>
#include <string.h>

// �ڴ�id
static int mid = -1;

// ���ڵ���Ϣ
ParentInfo Parent;

// ����������ַ
static uint64_t localIA = 0;
static char* localPwd = NULL;

// ����������
static uint64_t coreIA = TZACCESS_CORE_IA;
static uint8_t coreIP[4] = TZACCESS_CORE_IP;
static uint16_t corePort = TZACCESS_CORE_PORT;

// TZAccessGetMid ��ȡ�ڴ�id
int TZAccessGetMid(void) {
    if (mid == -1) {
        mid = TZMallocRegister(0, TZACCESS_TAG, TZACEESS_MALLOC_SIZE);
        if (mid == -1) {
            LE(TZACCESS_TAG, "malloc register failed!");
        }
    }
    return mid;
}

// TZAccessSetLocalIA ���ñ���IA��ַ
void TZAccessSetLocalIA(uint64_t ia) {
    localIA = ia;
}

// TZAccessGetLocalIA ��ȡ����IA��ַ
uint64_t TZAccessGetLocalIA(void) {
    return localIA;
}

// TZIotSetLocalPwd ��������
void TZAccessSetLocalPwd(char* pwd) {
    localPwd = TZMalloc(TZAccessGetMid(), (int)strlen(pwd) + 1);
    if (localPwd == NULL) {
        LE(TZACCESS_TAG, "set local pwd failed!");
        return;
    }
    strcpy(localPwd, pwd);
}

// TZAccessGetLocalPwd ��ȡ����
char* TZAccessGetLocalPwd(void) {
    return localPwd;
}

// TZAccessConfigCoreParam ���ú���������
// ip��4�ֽ�����
void TZAccessConfigCoreParam(uint64_t ia, uint8_t* ip, uint16_t port) {
    coreIA = ia;
    memcpy(coreIP, ip, 4);
    corePort = port;
}

// TZAccessGetCoreIA ��ȡ������IA��ַ
uint64_t TZAccessGetCoreIA(void) {
    return coreIA;
}

// TZAccessGetCoreIP ��ȡ������IP��ַ
uint8_t* TZAccessGetCoreIP(void) {
    return coreIP;
}

// TZAccessGetCorePort ��ȡ�������˿ں�
uint16_t TZAccessGetCorePort(void) {
    return corePort;
}
