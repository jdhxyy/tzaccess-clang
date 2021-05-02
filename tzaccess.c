// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// ���뺣ө������
// Authors: jdh99 <jdh821@163.com>

#include "tzaccess.h"
#include "tzaccessconfig.h"
#include "tzaccessstandardlayer.h"
#include "tzaccessparsecmp.h"
#include "tzaccessapply.h"
#include "tzaccessconn.h"

// TZAccessLoad ģ������
// mid���ڴ�ID.ia�Ǳ��ڵ�IA��ַ,pwd�Ǳ��ڵ�����
void TZAccessLoad(uint64_t ia, char* pwd, TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend) {
    TZAccessSetLocalIA(ia);
    TZAccessSetLocalPwd(pwd);

    TZAccessStandardLayerInit(send, isAllowSend);
    TZAccessParseCmpInit();
    TZAccessApplyInit();
    TZAccessConnInit();
}
