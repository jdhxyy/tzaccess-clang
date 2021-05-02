// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 配置文件
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessconfig.h"

#include "lagan.h"
#include "tzmalloc.h"

#include <stdio.h>
#include <string.h>

// 内存id
static int mid = -1;

// 父节点信息
ParentInfo Parent;

// 本机单播地址
static uint64_t localIA = 0;
static char* localPwd = NULL;

// 核心网参数
static uint64_t coreIA = TZACCESS_CORE_IA;
static uint8_t coreIP[4] = TZACCESS_CORE_IP;
static uint16_t corePort = TZACCESS_CORE_PORT;

// TZAccessGetMid 读取内存id
int TZAccessGetMid(void) {
    if (mid == -1) {
        mid = TZMallocRegister(0, TZACCESS_TAG, TZACEESS_MALLOC_SIZE);
        if (mid == -1) {
            LE(TZACCESS_TAG, "malloc register failed!");
        }
    }
    return mid;
}

// TZAccessSetLocalIA 设置本地IA地址
void TZAccessSetLocalIA(uint64_t ia) {
    localIA = ia;
}

// TZAccessGetLocalIA 读取本地IA地址
uint64_t TZAccessGetLocalIA(void) {
    return localIA;
}

// TZIotSetLocalPwd 设置密码
void TZAccessSetLocalPwd(char* pwd) {
    localPwd = TZMalloc(TZAccessGetMid(), (int)strlen(pwd) + 1);
    if (localPwd == NULL) {
        LE(TZACCESS_TAG, "set local pwd failed!");
        return;
    }
    strcpy(localPwd, pwd);
}

// TZAccessGetLocalPwd 读取密码
char* TZAccessGetLocalPwd(void) {
    return localPwd;
}

// TZAccessConfigCoreParam 配置核心网参数
// ip是4字节数组
void TZAccessConfigCoreParam(uint64_t ia, uint8_t* ip, uint16_t port) {
    coreIA = ia;
    memcpy(coreIP, ip, 4);
    corePort = port;
}

// TZAccessGetCoreIA 读取核心网IA地址
uint64_t TZAccessGetCoreIA(void) {
    return coreIA;
}

// TZAccessGetCoreIP 读取核心网IP地址
uint8_t* TZAccessGetCoreIP(void) {
    return coreIP;
}

// TZAccessGetCorePort 读取核心网端口号
uint16_t TZAccessGetCorePort(void) {
    return corePort;
}
