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

#ifndef __CCM_TEXT_PARSEPOSITION_H__
#define __CCM_TEXT_PARSEPOSITION_H__

#include "ccm/core/SyncObject.h"
#include "ccm.text.IParsePosition.h"

using ccm::core::SyncObject;

namespace ccm {
namespace text {

class ParsePosition
    : public SyncObject
    , public IParsePosition
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer index);

    ECode GetIndex(
        /* [out] */ Integer* index) override;

    ECode SetIndex(
        /* [in] */ Integer index) override;

    ECode SetErrorIndex(
        /* [in] */ Integer index) override;

    ECode GetErrorIndex(
        /* [out] */ Integer* index) override;

    ECode Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* same) override;

    ECode GetHashCode(
        /* [out] */ Integer* hash) override;

    ECode ToString(
        /* [out] */ String* desc) override;

private:
    Integer mIndex = 0;
    Integer mErrorIndex = -1;
};

}
}


#endif // __CCM_TEXT_PARSEPOSITION_H__
