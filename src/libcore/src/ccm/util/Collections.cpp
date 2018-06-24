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

#include "ccm/util/Collections.h"

namespace ccm {
namespace util {

AutoPtr<ICollection> Collections::CreateSynchronizedCollection(
    /* [in] */ ICollection* c,
    /* [in] */ ISynchronize* mutex)
{
    return nullptr;
}

AutoPtr<ISet> Collections::CreateSynchronizedSet(
    /* [in] */ ISet* s,
    /* [in] */ ISynchronize* mutex)
{
    return nullptr;
}

AutoPtr<IIterator> Collections::GetEmptyIterator()
{
    return nullptr;
}

AutoPtr<IEnumeration> Collections::GetEmptyEnumeration()
{
    return nullptr;
}

}
}
