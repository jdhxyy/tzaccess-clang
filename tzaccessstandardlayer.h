// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ��׼�㴦��ģ��
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_STANDARD_LAYER_H
#define TZACCESS_STANDARD_LAYER_H

#include <stdint.h>
#include <stdbool.h>

#include "tzaccess.h"

#include "tztype.h"
#include "utz.h"

// TZAccessStandardLayerRxCallback ���ջص�����
typedef void (*TZAccessStandardLayerRxCallback)(uint8_t* data, int size, UtzStandardHeader* standardHeader, 
    uint8_t* srcIP, uint16_t srcPort);

// TZAccessStandardLayerInit ��ʼ��
void TZAccessStandardLayerInit(TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend);

// TZAccessStandardLayerRegisterRxObserver ע��۲���
void TZAccessStandardLayerRegisterRxObserver(TZAccessStandardLayerRxCallback callback);

// TZAccessStandardLayerSend ���ڱ�׼ͷ������
void TZAccessStandardLayerSend(uint8_t* data, int dataLen, UtzStandardHeader* standardHeader, uint8_t* dstIP, 
    uint16_t dstPort);

// TZAccessStandardLayerIsAllowSend �Ƿ�������
bool TZAccessStandardLayerIsAllowSend(void);

#endif
