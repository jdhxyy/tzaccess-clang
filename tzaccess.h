// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ���뺣ө������
// Authors: jdh99 <jdh821@163.com>

#ifndef TZACCESS_LOAD_H
#define TZACCESS_LOAD_H

#include "tztype.h"

// TZAccessSendFunc ���ͺ�������
// dstIP��Ŀ��IP��ַ,4�ֽ�����.dstPort��Ŀ��˿ں�
typedef void (*TZAccessSendFunc)(uint8_t* data, int size, uint8_t* dstIP, uint16_t dstPort);

// TZAccessLoad ģ������
// mid���ڴ�ID.ia�Ǳ��ڵ�IA��ַ,pwd�Ǳ��ڵ�����
void TZAccessLoad(uint64_t ia, char* pwd, TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend);

// TZAccessReceive �û����յ�����ʱ��ص�������
// dstIP��ԴIP��ַ,4�ֽ�����.dstPort��Դ�˿ں�
void TZAccessReceive(uint8_t* data, int size, uint8_t* srcIP, uint16_t srcPort);

// TZAccessIsConn �Ƿ����Ӻ�����
bool TZAccessIsConn(void);

// TZAcessGetParentAddr ��ȡ���ڵ�ĵ�ַ
// ip��ַ�����ֽ�����.������ڵ㲻����,��ip��port��Ϊ0
void TZAcessGetParentAddr(char* ip, uint16_t* port);

// TZAccessConfigCoreParam ���ú���������
void TZAccessConfigCoreParam(uint64_t ia, char* ip, uint16_t port);

#endif
