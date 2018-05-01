//=========================================================================
// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

/*
 * Copyright (C) 2005 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __CCM_CPARCEL_H__
#define __CCM_CPARCEL_H__

#include "util/ccmobject.h"

namespace ccm {

extern const CoclassID CID_CParcel;

COCLASS_ID(28208469-8814-49af-80f7-8b147391fb94)
class CParcel
    : public Object
    , public IParcel
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode ReadChar(
        /* [out] */ Char* value);

    ECode WriteChar(
        /* [in] */ Char value);

    ECode ReadByte(
        /* [out] */ Byte* value);

    ECode WriteByte(
        /* [in] */ Byte value);

    ECode ReadShort(
        /* [out] */ Short* value);

    ECode WriteShort(
        /* [in] */ Short value);

    ECode ReadInteger(
        /* [out] */ Integer* value);

    ECode WriteInteger(
        /* [in] */ Integer value);

    ECode ReadLong(
        /* [out] */ Long* value);

    ECode WriteLong(
        /* [in] */ Long value);

    ECode ReadFloat(
        /* [out] */ Float* value);

    ECode WriteFloat(
        /* [in] */ Float value);

    ECode ReadDouble(
        /* [out] */ Double* value);

    ECode WriteDouble(
        /* [in] */ Double value);

    ECode ReadBoolean(
        /* [out] */ Boolean* value);

    ECode WriteBoolean(
        /* [in] */ Boolean value);

    ECode ReadString(
        /* [out] */ String* value);

    ECode WriteString(
        /* [in] */ String value);

    ECode ReadCoclassID(
        /* [out] */ CoclassID* value);

    ECode WriteCoclassID(
        /* [in] */ CoclassID value);

    ECode ReadComponentID(
        /* [out] */ ComponentID* value);

    ECode WriteComponentID(
        /* [in] */ ComponentID value);

    ECode ReadInterfaceID(
        /* [out] */ InterfaceID* value);

    ECode WriteInterfaceID(
        /* [in] */ InterfaceID value);

    ECode ReadHANDLE(
        /* [out] */ HANDLE* value);

    ECode WriteHANDLE(
        /* [in] */ HANDLE value);

    ECode ReadECode(
        /* [out] */ ECode* value);

    ECode WriteECode(
        /* [in] */ ECode value);

    ECode ReadEnumeration(
        /* [out] */ Integer* value);

    ECode WriteEnumeration(
        /* [in] */ Integer value);

    ECode ReadArray(
        /* [out] */ HANDLE* array);

    ECode WriteArray(
        /* [in] */ HANDLE array);

    ECode ReadInterface(
        /* [out] */ IInterface** value);

    ECode WriteInterface(
        /* [in] */ IInterface* value);
};

}

#endif //__CCM_CPARCEL_H__
