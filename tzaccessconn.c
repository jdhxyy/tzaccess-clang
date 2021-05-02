// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ���Ӹ�·��
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

// dealAckConnectParentRouter ����Ӧ������֡
// ��ѭ˭����˭�ͷ�ԭ��,resp��Ҫ�ɻص�����ʹ��TZMalloc���ٿռ�,���÷������ͷ�
// respΪNULL����respLenΪ0��ʾ����ҪӦ��
// argp�ǿɱ����,������ʽ�������ɵ���˫��Լ��
static void dealAckConnectParentRouter(uint8_t* req, int reqLen, uint8_t** resp, int* respLen, va_list argp);

static int connTask(void);
static void sendConnFrame(void);

// TZAccessConnInit ��ʼ��
void TZAccessConnInit(void) {
    KnockRegister(UTZ_HEADER_CMP, UTZ_CMP_MSG_TYPE_ACK_CONNECT_PARENT_ROUTER, dealAckConnectParentRouter);
    AsyncStart(connTask, ASYNC_SECOND);
}

// dealAckConnectParentRouter ����Ӧ������֡
// ��ѭ˭����˭�ͷ�ԭ��,resp��Ҫ�ɻص�����ʹ��TZMalloc���ٿռ�,���÷������ͷ�
// respΪNULL����respLenΪ0��ʾ����ҪӦ��
// argp�ǿɱ����,������ʽ�������ɵ���˫��Լ��
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
    // ǰ׺����
    body[j++] = 64;
    // ��Ĥ�ӻ��̶�������ַ
    j += UTZ_IA_LEN;
    // ����ֵ
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

// TZAccessIsConn �Ƿ����Ӻ�����
bool TZAccessIsConn(void) {
    return Parent.IA != UTZ_IA_INVALID && Parent.IsConn;
}

// TZAccessGetParentAddr ��ȡ���ڵ�ĵ�ַ
// ip��ַ�����ֽ�����.������ڵ㲻����,��ip��port��Ϊ0
void TZAccessGetParentAddr(char* ip, uint16_t* port) {
    if (TZAccessIsConn() == false) {
        memset(ip, 0, 4);
        *port = 0;
    } else {
        memcpy(ip, Parent.IP, 4);
        *port = Parent.Port;
    }
}
