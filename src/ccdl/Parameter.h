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

#ifndef __CCM_CCDL_PARAMETER_H__
#define __CCM_CCDL_PARAMETER_H__

#include "ASTElement.h"
#include "Type.h"

namespace ccm {
namespace ccdl {

class Parameter : public ASTElement
{
public:
    Parameter()
        : mAttribute(0)
        , mType(nullptr)
        , mPointer(0)
    {}

    inline Parameter& SetAttribute(
        /* [in] */ int attr) { mAttribute = mAttribute | attr; return *this; }

public:
    static constexpr int IN = 0x1;
    static constexpr int OUT = 0x2;
    static constexpr int CALLEE = 0x4;

    int mAttribute;
    Type* mType;
    int mPointer;
};

}
}

#endif // __CCM_CCDL_PARAMETER_H__
