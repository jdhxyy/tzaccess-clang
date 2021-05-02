// Copyright 2021-2021 The jdh99 Authors. All rights reserved.
// 接入海萤物联网
// Authors: jdh99 <jdh821@163.com>

#include "tzaccess.h"
#include "tzaccessconfig.h"
#include "tzaccessstandardlayer.h"
#include "tzaccessparsecmp.h"
#include "tzaccessapply.h"
#include "tzaccessconn.h"

// TZAccessLoad 模块载入
// mid是内存ID.ia是本节点IA地址,pwd是本节点密码
void TZAccessLoad(uint64_t ia, char* pwd, TZAccessSendFunc send, TZIsAllowSendFunc isAllowSend) {
    TZAccessSetLocalIA(ia);
    TZAccessSetLocalPwd(pwd);

    TZAccessStandardLayerInit(send, isAllowSend);
    TZAccessParseCmpInit();
    TZAccessApplyInit();
    TZAccessConnInit();
}
