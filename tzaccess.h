// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 接入海萤物联网
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_LOAD_H
#define TZACCESS_LOAD_H

#include "tztype.h"

// TZAccessSendFunc 发送函数类型
// dstIP是目标IP地址,4字节数组.dstPort是目标端口号
typedef void (*TZAccessSendFunc)(uint8_t* data, int size, uint8_t* dstIP, uint16_t dstPort);

// TZAccessLoad 模块载入
// mid是内存ID.ia是本节点IA地址,pwd是本节点密码
void TZAccessLoad(uint64_t ia, char* pwd, TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend);

// TZAccessReceive 用户接收到数据时需回调本函数
// dstIP是源IP地址,4字节数组.dstPort是源端口号
void TZAccessReceive(uint8_t* data, int size, uint8_t* srcIP, uint16_t srcPort);

// TZAccessIsConn 是否连接核心网
bool TZAccessIsConn(void);

// TZAcessGetParentAddr 读取父节点的地址
// ip地址是四字节数组.如果父节点不存在,则ip和port都为0
void TZAcessGetParentAddr(char* ip, uint16_t* port);

// TZAccessConfigCoreParam 配置核心网参数
void TZAccessConfigCoreParam(uint64_t ia, char* ip, uint16_t port);

#endif
