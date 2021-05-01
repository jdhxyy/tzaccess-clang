// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 配置文件
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_CONFIG_H
#define TZACCESS_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#define TZACCESS_TAG "tzaccess"

// 最大帧字节数
#define TZACCESS_FRAME_MAX_LEN 512

// 申请父路由间隔.单位:s
#define TZACCESS_APPLY_INTERVAL 10

// 连接间隔.单位:s
#define TZACCESS_CONN_INTERVAL 30
// 最大连接次数.超过连接次数这回清除父路由IA地址,重连父路由
#define TZACCESS_CONN_NUM_MAX 5

// 默认核心网地址
#define TZACCESS_CORE_IA 0x2141000000000002
#define TZACCESS_CORE_IP {115, 28, 86, 171}
#define TZACCESS_CORE_PORT 12914

#pragma pack(1)

// 父节点信息
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

// 内存id
extern int TZAccessMid;

// TZAccessSetLocalIA 设置本地IA地址
void TZAccessSetLocalIA(uint64_t ia);

// TZAccessGetLocalIA 读取本地IA地址
uint64_t TZAccessGetLocalIA(void);

// TZAccessSetLocalPwd 设置密码
void TZAccessSetLocalPwd(char* pwd);

// TZAccessGetLocalPwd 读取密码
char* TZAccessGetLocalPwd(void);

// TZAccessGetCoreIA 读取核心网IA地址
uint64_t TZAccessGetCoreIA(void);

// TZAccessGetCoreIP 读取核心网IP地址
uint8_t* TZAccessGetCoreIP(void);

// TZAccessGetCorePort 读取核心网端口号
uint16_t TZAccessGetCorePort(void);

#endif
