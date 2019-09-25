//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#ifndef __CDLC_REFERENCETYPE_H__
#define __CDLC_REFERENCETYPE_H__

#include "ast/Type.h"
#include "util/AutoPtr.h"

namespace cdlc {

class ReferenceType
    : public Type
{
public:
    bool IsReferenceType() override;

    inline AutoPtr<Type> GetBaseType();

    inline void SetBaseType(
        /* [in] */ Type* type);

    String GetSignature() override;

    String ToString() override;

    inline static AutoPtr<ReferenceType> CastFrom(
        /* [in] */ Type* type);

private:
    AutoPtr<Type> mBaseType;
};

AutoPtr<Type> ReferenceType::GetBaseType()
{
    return mBaseType;
}

void ReferenceType::SetBaseType(
    /* [in] */ Type* type)
{
    mBaseType = type;
}

AutoPtr<ReferenceType> ReferenceType::CastFrom(
    /* [in] */ Type* type)
{
    return static_cast<ReferenceType*>(type);
}

}

#endif // __CDLC_REFERENCETYPE_H__