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

#include "ccm/core/NativeLockWord.h"
#include "ccm/core/NativeMonitor.h"
#include "ccm/core/NativeMonitorPool.h"
#include <ccmdef.h>

namespace ccm {
namespace core {

NativeLockWord::NativeLockWord(
    /* [in] */ NativeMonitor* mon)
    : mValue(mon->GetMonitorId() | (kStateFat << kStateShift))
{
    CHECK(FatLockMonitor() == mon);
    CHECK(mon->GetMonitorId() <= static_cast<uint32_t>(kMaxMonitorId));
}

NativeLockWord::LockState NativeLockWord::GetState() const
{
    if (UNLIKELY(mValue == 0)) {
        return kUnlocked;
    }
    else {
        uint32_t internalState = (mValue >> kStateShift) & kStateMask;
        switch (internalState) {
            case kStateThinOrUnlocked:
                return kThinLocked;
            default:
                CHECK(internalState == kStateFat);
               return kFatLocked;
        }
    }
}

NativeMonitor* NativeLockWord::FatLockMonitor() const
{
    CHECK(GetState() == kFatLocked);
    MonitorId monId = (mValue >> kMonitorIdShift) & kMonitorIdMask;
    return NativeMonitorPool::MonitorFromMonitorId(monId);
}

}
}
