// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 申请父路由
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessapply.h"
#include "tzaccessconfig.h"
#include "tzaccessstandardlayer.h"

#include "knock.h"
#include "utz.h"
#include "lagan.h"
#include "async.h"
#include "tzmalloc.h"
#include "pt.h"

#include <string.h>

// dealAssignSlaveRouter 处理分配从机帧
// 遵循谁调用谁释放原则,resp需要由回调函数使用TZMalloc开辟空间,调用方负责释放
// resp为NULL或者respLen为0表示不需要应答
// argp是可变参数,参数格式和数量由调用双方约定
static void dealAssignSlaveRouter(uint8_t* req, int reqLen, uint8_t** resp, int* respLen, va_list argp);

static int applyTask(void);
static void sendApplyFrame(void);

// TZAccessApplyInit 初始化
void TZAccessApplyInit(void) {
    KnockRegister(UTZ_HEADER_CMP, UTZ_CMP_MSG_TYPE_ASSIGN_SLAVE_ROUTER, dealAssignSlaveRouter);
    AsyncStart(applyTask, ASYNC_SECOND);
}

// dealAssignSlaveRouter 处理分配从机帧
// 遵循谁调用谁释放原则,resp需要由回调函数使用TZMalloc开辟空间,调用方负责释放
// resp为NULL或者respLen为0表示不需要应答
// argp是可变参数,参数格式和数量由调用双方约定
static void dealAssignSlaveRouter(uint8_t* req, int reqLen, uint8_t** resp, int* respLen, va_list argp) {
    TZ_UNUSED(resp);
    TZ_UNUSED(respLen);

    uint8_t*ip = va_arg(argp, uint8_t*);
    uint16_t port = (uint16_t)va_arg(argp, int);

    if (memcmp(ip, TZAccessGetCoreIP(), 4) != 0 || port != TZAccessGetCorePort()) {
        LW(TZACCESS_TAG, "deal apply failed.ip is not match.ip:%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], port);
        return;
    }

    if (req == NULL || reqLen == 0) {
        LW(TZACCESS_TAG, "deal apply failed.payload len is 0");
        return;
    }

    int j = 0;
    if (req[j] != 0) {
        LW(TZACCESS_TAG, "deal apply failed.error code:%d", req[j]);
        return;
    }
    j++;

    if (reqLen != 16) {
        LW(TZACCESS_TAG, "deal apply failed.payload len is wrong:%d", reqLen);
        return;
    }

    Parent.IA = UtzBytesToIA(req + j);
    j += UTZ_IA_LEN;

    memcpy(Parent.IP, req + j, 4);
    j += 4;
    Parent.Port = (uint16_t)((req[j] << 8) + req[j + 1]);
    j += 2;

    LI(TZACCESS_TAG, "apply success.parent ia:0x%llx ip:%d.%d.%d.%d port:%d cost:%d", Parent.IA, Parent.IP[0], 
        Parent.IP[1], Parent.IP[2], Parent.IP[3], Parent.Port, req[j]);
    return;
}

static int applyTask(void) {
    static struct pt pt;

    PT_BEGIN(&pt);

    if (TZAccessStandardLayerIsAllowSend() == false || Parent.IA != UTZ_IA_INVALID) {
        PT_EXIT(&pt);
    }

    LI(TZACCESS_TAG, "send apply frame");
    sendApplyFrame();

    ASYNC_WAIT(&pt, TZACCESS_APPLY_INTERVAL * ASYNC_SECOND);

    PT_END(&pt);
}

static void sendApplyFrame(void) {
    TZBufferDynamic* securityHeaderBytes = NULL;
    TZBufferDynamic* flpFrame = NULL;
    uint8_t* payload = NULL;
    uint8_t body[9] = {0};
    int payloadSize = 0;

    securityHeaderBytes = UtzSimpleSecurityHeaderDataToBytes(UTZ_HEADER_CMP, TZAccessGetLocalPwd());
    if (securityHeaderBytes == NULL) {
        LE(TZACCESS_TAG, "send apply frame failed!simple security header to bytes failed!");
        goto EXIT;
    }

    body[0] = UTZ_CMP_MSG_TYPE_REQUEST_SLAVE_ROUTER;
    UtzIAToBytes(Parent.IA, body + 1);

    flpFrame = UtzBytesToFlpFrame(body, 9, true, 0);
    if (flpFrame == NULL) {
        LE(TZACCESS_TAG, "send apply frame failed!bytes to flp frame failed!");
        goto EXIT;
    }

    payloadSize = securityHeaderBytes->len + flpFrame->len;
    payload = TZMalloc(TZAccessGetMid(), payloadSize);
    if (payload == NULL) {
        LE(TZACCESS_TAG, "send apply frame failed!malloc failed!");
        goto EXIT;
    }
    memcpy(payload, securityHeaderBytes->buf, (size_t)securityHeaderBytes->len);
    memcpy(payload + securityHeaderBytes->len, flpFrame->buf, (size_t)flpFrame->len);

    UtzStandardHeader header;
    header.Version = UTZ_NLV1_PROTOCOL_VERSION;
    header.FrameIndex = UtzGenerateFrameIndex();
    header.PayloadLen = (uint16_t)payloadSize;
    header.NextHead = UTZ_HEADER_SIMPLE_SECURITY;
    header.HopsLimit = 0xff;
    header.SrcIA = TZAccessGetLocalIA();
    header.DstIA = TZAccessGetCoreIA();

    TZAccessStandardLayerSend(payload, payloadSize, &header, TZAccessGetCoreIP(), TZAccessGetCorePort());

EXIT:
    TZFree(securityHeaderBytes);
    TZFree(flpFrame);
    TZFree(payload);
}
