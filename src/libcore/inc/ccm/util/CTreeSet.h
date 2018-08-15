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

#ifndef __CCM_UTIL_CTREESET_H__
#define __CCM_UTIL_CTREESET_H__

#include "ccm/core/SyncObject.h"
#include "ccm.core.ICloneable.h"
#include "ccm.core.IIterable.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IComparator.h"
#include "ccm.util.INavigableMap.h"
#include "ccm.util.INavigableSet.h"
#include "ccm.util.ISortedSet.h"
#include "ccm.util.ISet.h"
#include "ccm.util.ITreeSet.h"
#include "_ccm_util_CTreeSet.h"

using ccm::core::ICloneable;
using ccm::core::IIterable;
using ccm::io::ISerializable;
using ccm::core::SyncObject;

namespace ccm {
namespace util {

Coclass(CTreeSet)
    , public SyncObject
    , public ITreeSet
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode Constructor()
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ IComparator* comparator)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ INavigableMap* m)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ ICollection* c)
    {
        return NOERROR;
    }

    ECode Constructor(
        /* [in] */ ISortedSet* s)
    {
        return NOERROR;
    }
};

}
}

#endif // __CCM_UTIL_CTREESET_H__
