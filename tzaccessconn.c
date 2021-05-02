// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 连接父路由
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessconn.h"
#include "tzaccessconfig.h"
#include "tzaccessstandardlayer.h"

#include "knock.h"
#include "utz.h"
#include "async.h"
#include "lagan.h"
#include "tztime.h"
#include "pt.h"
#include "tzmalloc.h"

#include <string.h>

static int connNum = 0;

// dealAckConnectParentRouter 处理应答连接帧
// 遵循谁调用谁释放原则,resp需要由回调函数使用TZMalloc开辟空间,调用方负责释放
// resp为NULL或者respLen为0表示不需要应答
// argp是可变参数,参数格式和数量由调用双方约定
static void dealAckConnectParentRouter(uint8_t* req, int reqLen, uint8_t** resp, int* respLen, va_list argp);

static int connTask(void);
static void sendConnFrame(void);

// TZAccessConnInit 初始化
void TZAccessConnInit(void) {
    KnockRegister(UTZ_HEADER_CMP, UTZ_CMP_MSG_TYPE_ACK_CONNECT_PARENT_ROUTER, dealAckConnectParentRouter);
    AsyncStart(connTask, ASYNC_SECOND);
}

// dealAckConnectParentRouter 处理应答连接帧
// 遵循谁调用谁释放原则,resp需要由回调函数使用TZMalloc开辟空间,调用方负责释放
// resp为NULL或者respLen为0表示不需要应答
// argp是可变参数,参数格式和数量由调用双方约定
static void dealAckConnectParentRouter(uint8_t* req, int reqLen, uint8_t** resp, int* respLen, va_list argp) {
    TZ_UNUSED(resp);
    TZ_UNUSED(respLen);

    uint8_t*ip = va_arg(argp, uint8_t*);
    uint16_t port = (uint16_t)va_arg(argp, int);

    if (memcmp(ip, Parent.IP, 4) != 0 || port != Parent.Port) {
        LW(TZACCESS_TAG, "deal conn failed.ip is not match.ip:%d.%d.%d.%d:%d", ip[0], ip[1], ip[2], ip[3], port);
        return;
    }

    if (req == NULL || reqLen == 0) {
        LW(TZACCESS_TAG, "deal conn failed.payload len is 0");
        return;
    }

    int j = 0;
    if (req[j] != 0) {
        LW(TZACCESS_TAG, "deal conn failed.error code:%d", req[j]);
        return;
    }
    j++;

    if (reqLen != 2) {
        LW(TZACCESS_TAG, "deal conn failed.payload len is wrong:%d", reqLen);
        return;
    }

    connNum = 0;
    Parent.IsConn = true;
    Parent.Cost = req[j];
    Parent.Timestamp = TZTimeGet();
    LI(TZACCESS_TAG, "conn success.parent ia:0x%llx cost:%d", Parent.IA, Parent.Cost);
    return;
}

static int connTask(void) {
    static struct pt pt;

    PT_BEGIN(&pt);

    if (TZAccessStandardLayerIsAllowSend() == false || Parent.IA == UTZ_IA_INVALID) {
        PT_EXIT(&pt);
    }

    connNum++;
    if (connNum > TZACCESS_CONN_NUM_MAX) {
        connNum = 0;
        Parent.IA = UTZ_IA_INVALID;
        LW(TZACCESS_TAG, "conn num is too many!");
        PT_EXIT(&pt);
    }

    LI(TZACCESS_TAG, "send conn frame");
    sendConnFrame();

    ASYNC_WAIT(&pt, TZACCESS_CONN_INTERVAL * ASYNC_SECOND);

    PT_END(&pt);
}

static void sendConnFrame(void) {
    TZBufferDynamic* securityHeaderBytes = NULL;
    TZBufferDynamic* flpFrame = NULL;
    uint8_t* payload = NULL;

    securityHeaderBytes = UtzSimpleSecurityHeaderDataToBytes(UTZ_HEADER_CMP, TZAccessGetLocalPwd());
    if (securityHeaderBytes == NULL) {
        LE(TZACCESS_TAG, "send conn frame failed!simple security header to bytes failed!");
        goto EXIT;
    }

    uint8_t body[TZ_BUFFER_TINY_LEN] = {0};
    int j = 0;
    body[j++] = UTZ_CMP_MSG_TYPE_CONNECT_PARENT_ROUTER;
    // 前缀长度
    body[j++] = 64;
    // 子膜从机固定单播地址
    j += UTZ_IA_LEN;
    // 开销值
    body[j++] = 0;

    flpFrame = UtzBytesToFlpFrame(body, j, true, 0);
    if (flpFrame == NULL) {
        LE(TZACCESS_TAG, "send conn frame failed!bytes to flp frame failed!");
        goto EXIT;
    }

    int payloadSize = securityHeaderBytes->len + flpFrame->len;
    payload = TZMalloc(TZAccessGetMid(), payloadSize);
    if (payload == NULL) {
        LE(TZACCESS_TAG, "send conn frame failed!malloc failed!");
        goto EXIT;
    }
    memcpy(payload, securityHeaderBytes->buf, (uint64_t)securityHeaderBytes->len);
    memcpy(payload + securityHeaderBytes->len, flpFrame->buf, (uint64_t)flpFrame->len);

    UtzStandardHeader header;
    header.Version = UTZ_NLV1_PROTOCOL_VERSION;
    header.FrameIndex = UtzGenerateFrameIndex();
    header.PayloadLen = (uint16_t)payloadSize;
    header.NextHead = UTZ_HEADER_SIMPLE_SECURITY;
    header.HopsLimit = 0xff;
    header.SrcIA = TZAccessGetLocalIA();
    header.DstIA = TZAccessGetCoreIA();

    TZAccessStandardLayerSend(payload, payloadSize, &header, Parent.IP, Parent.Port);

EXIT:
    TZFree(securityHeaderBytes);
    TZFree(flpFrame);
    TZFree(payload);
}

// TZAccessIsConn 是否连接核心网
bool TZAccessIsConn(void) {
    return Parent.IA != UTZ_IA_INVALID && Parent.IsConn;
}

// TZAccessGetParentAddr 读取父节点的地址
// ip地址是四字节数组.如果父节点不存在,则ip和port都为0
void TZAccessGetParentAddr(char* ip, uint16_t* port) {
    if (TZAccessIsConn() == false) {
        memset(ip, 0, 4);
        *port = 0;
    } else {
        memcpy(ip, Parent.IP, 4);
        *port = Parent.Port;
    }
}
