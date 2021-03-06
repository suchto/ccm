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

#ifndef __CCM_UTIL_LinkedHashSet_H__
#define __CCM_UTIL_LinkedHashSet_H__

#include "ccm/util/HashSet.h"
#include "ccm.util.ILinkedHashSet.h"

namespace ccm {
namespace util {

class LinkedHashSet
    : public HashSet
    , public ILinkedHashSet
{
public:
    CCM_INTERFACE_DECL();

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ ICollection* c);
};

}
}

#endif // __CCM_UTIL_LinkedHashSet_H__
