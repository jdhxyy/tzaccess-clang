// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 标准层处理模块
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_STANDARD_LAYER_H
#define TZACCESS_STANDARD_LAYER_H

#include <stdint.h>
#include <stdbool.h>

#include "tzaccess.h"

#include "tztype.h"
#include "utz.h"

// TZAccessStandardLayerRxCallback 接收回调函数
typedef void (*TZAccessStandardLayerRxCallback)(uint8_t* data, int size, UtzStandardHeader* standardHeader, 
    uint8_t* srcIP, uint16_t srcPort);

// TZAccessStandardLayerInit 初始化
void TZAccessStandardLayerInit(TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend);

// TZAccessStandardLayerRegisterRxObserver 注册观察者
void TZAccessStandardLayerRegisterRxObserver(TZAccessStandardLayerRxCallback callback);

// TZAccessStandardLayerSend 基于标准头部发送
void TZAccessStandardLayerSend(uint8_t* data, int dataLen, UtzStandardHeader* standardHeader, uint8_t* dstIP, 
    uint16_t dstPort);

// TZAccessStandardLayerIsAllowSend 是否允许发送
bool TZAccessStandardLayerIsAllowSend(void);

#endif
