// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 标准层处理模块
// Authors: jdh99 <jdh821@163.com>

#include "tzaccessstandardlayer.h"
#include "tzaccessconfig.h"

#include "lagan.h"
#include "tzlist.h"
#include "tzmalloc.h"

#include <string.h>

#pragma pack(1)

typedef struct {
    TZAccessStandardLayerRxCallback callback;
} tItem;

#pragma pack()

static TZAccessSendFunc sendFunc = NULL;
static TZIsAllowSendFunc isAllowSendFunc = NULL;

// 观察者列表
static intptr_t list = 0;

static int getStandardHeader(uint8_t* data, int size, UtzStandardHeader* header);
static void notifyObserver(uint8_t* data, int size, UtzStandardHeader* header, uint8_t* srcIP, uint16_t srcPort);
static TZListNode* createNode(void);

// TZAccessStandardLayerInit 初始化
void TZAccessStandardLayerInit(TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend) {
    list = TZListCreateList(TZAccessGetMid());
    if (list == 0) {
        LE(TZACCESS_TAG, "standard layer init failed!create list failed!");
        return;
    }
    sendFunc = send;
    isAllowSendFunc = isAllowSend;
}

// TZAccessReceive 用户接收到数据时需回调本函数
// dstIP是源IP地址,4字节数组.dstPort是源端口号
void TZAccessReceive(uint8_t* data, int size, uint8_t* srcIP, uint16_t srcPort) {
    UtzStandardHeader header;
    int offset = getStandardHeader(data, size, &header);
    if (offset == 0) {
        return;
    }
    notifyObserver(data + offset, size - offset, &header, srcIP, srcPort);
}

static int getStandardHeader(uint8_t* data, int size, UtzStandardHeader* header) {
    int offset = UtzBytesToStandardHeader(data, size, header);
    if (offset == 0) {
        LD(TZACCESS_TAG, "get standard header failed:bytes to standard header failed");
        return 0;
    }
    if (header->Version != UTZ_NLV1_PROTOCOL_VERSION) {
        LD(TZACCESS_TAG, "get standard header failed:protocol version is not match:%d", header->Version);
        return 0;
    }
    if (header->PayloadLen + offset != size) {
        LD(TZACCESS_TAG, "get standard header failed:payload len is not match:%d", header->PayloadLen);
        return 0;
    }
    return offset;
}

static void notifyObserver(uint8_t* data, int size, UtzStandardHeader* header, uint8_t* srcIP, uint16_t srcPort) {
    TZListNode* node = TZListGetHeader(list);
    tItem* item = NULL;
    for (;;) {
        if (node == NULL) {
            break;
        }

        item = (tItem*)node->Data;
        if (item->callback) {
            item->callback(data, size, header, srcIP, srcPort);
        }

        node = node->Next;
    }
}

// TZAccessStandardLayerRegisterRxObserver 注册观察者
void TZAccessStandardLayerRegisterRxObserver(TZAccessStandardLayerRxCallback callback) {
    TZListNode* node = createNode();
    if (node == NULL) {
        LE(TZACCESS_TAG, "standard layer register observer failed!create node failed!");
        return;
    }
    tItem* item = (tItem*)node->Data;
    item->callback = callback;
    TZListAppend(list, node);
}

static TZListNode* createNode(void) {
    TZListNode* node = TZListCreateNode(list);
    if (node == NULL) {
        return NULL;
    }
    node->Data = TZMalloc(TZAccessGetMid(), sizeof(tItem));
    if (node->Data == NULL) {
        TZFree(node);
        return NULL;
    }
    return node;
}

// TZAccessStandardLayerSend 基于标准头部发送
void TZAccessStandardLayerSend(uint8_t* data, int dataLen, UtzStandardHeader* standardHeader, uint8_t* dstIP, 
    uint16_t dstPort) {
    if (dataLen > TZACCESS_FRAME_MAX_LEN) {
        LE(TZACCESS_TAG, "standard layer send failed!data len is too long:%d src ia:0x%llx dst ia:0x%llx", dataLen, 
            standardHeader->SrcIA, standardHeader->DstIA);
        return;
    }
    if (standardHeader->PayloadLen != (uint16_t)dataLen) {
        standardHeader->PayloadLen = (uint16_t)dataLen;
    }

    int frameSize = UTZ_NLV1_HEAD_LEN + dataLen;
    uint8_t* frame = TZMalloc(TZAccessGetMid(), frameSize);
    if (frame == NULL) {
        LE(TZACCESS_TAG, "standard layer send failed!malloc failed!");
        return;
    }
    (void)UtzStandardHeaderToBytes(standardHeader, frame, UTZ_NLV1_HEAD_LEN);
    memcpy(frame + UTZ_NLV1_HEAD_LEN, data, (uint64_t)dataLen);
    sendFunc(frame, frameSize, dstIP, dstPort);
    TZFree(frame);
}

// TZAccessStandardLayerIsAllowSend 是否允许发送
bool TZAccessStandardLayerIsAllowSend(void) {
    return isAllowSendFunc();
}
