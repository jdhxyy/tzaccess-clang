// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// CMP协议解析处理
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessparsecmp.h"
#include "tzaccessstandardlayer.h"
#include "tzaccessconfig.h"

#include "utz.h"
#include "tzmalloc.h"
#include "knock.h"
#include "lagan.h"

#include <string.h>

static void dealStandardLayerRx(uint8_t* data, int size, UtzStandardHeader* standardHeader, uint8_t* srcIP, 
    uint16_t srcPort);

// TZAccessParseCmpInit 初始化
void TZAccessParseCmpInit(void) {
    TZAccessStandardLayerRegisterRxObserver(dealStandardLayerRx);
}

static void dealStandardLayerRx(uint8_t* data, int size, UtzStandardHeader* standardHeader, uint8_t* srcIP, 
    uint16_t srcPort) {
    if (standardHeader->DstIA != TZAccessGetLocalIA() || standardHeader->NextHead != UTZ_HEADER_CMP) {
        return;
    }

    TZBufferDynamic* payload = UtzFlpFrameToBytes(data, size);
    if (payload == NULL) {
        LW(TZACCESS_TAG, "parse cmp failed.flp frame to bytes failed");
        return;
    }

    KnockCall(UTZ_HEADER_CMP, payload->buf[0], payload->buf + 1, payload->len - 1, NULL, 0, srcIP, (int)srcPort);
    TZFree(payload);
}
