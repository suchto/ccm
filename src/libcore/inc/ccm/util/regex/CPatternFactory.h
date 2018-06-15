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

#ifndef __CCM_UTIL_REGEX_CPATTERNFACTORY_H__
#define __CCM_UTIL_REGEX_CPATTERNFACTORY_H__

#include "ccm.core.ICharSequence.h"
#include "ccm.util.regex.IPatternFactory.h"
#include "_ccm_util_regex_CPatternFactory.h"
#include <ccmobject.h>
#include <ccmdef.h>

using ccm::core::ICharSequence;

namespace ccm {
namespace util {
namespace regex {

class CPatternFactory
    : public _CPatternFactory
    , public Object
    , public IPatternFactory
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Compile(
        /* [in] */ const String& regex,
        /* [out] */ IPattern** pattern);

    ECode Compile(
        /* [in] */ const String& regex,
        /* [in] */ Integer flags,
        /* [out] */ IPattern** pattern);

    ECode FastSplit(
        /* [in] */ const String& re,
        /* [in] */ const String& input,
        /* [in] */ Integer limit,
        /* [out, callee] */ Array<String>* strArray);

    ECode Matches(
        /* [in] */ const String& regex,
        /* [in] */ ICharSequence* input,
        /* [out] */ Boolean* matched);

    ECode Quote(
        /* [in] */ const String& s,
        /* [out] */ String* pattStr);
};

}
}
}

#endif // __CCM_UTIL_REGEX_CPATTERNFACTORY_H__
