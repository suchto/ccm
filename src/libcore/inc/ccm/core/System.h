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

#ifndef __CCM_CORE_SYSTEM_H__
#define __CCM_CORE_SYSTEM_H__

#include "ccm/util/Properties.h"
#include "ccm.core.ISecurityManager.h"
#include "ccm.io.IPrintStream.h"
#include "ccm.util.IProperties.h"
#include <ccmautoptr.h>
#include <ccmtypes.h>

using ccm::io::IPrintStream;
using ccm::util::IProperties;
using ccm::util::Properties;

namespace ccm {
namespace core {

static void StaticInitializeSystem();

class System
{
protected:
    class PropertiesWithNonOverrideableDefaults
        : public Properties
    {
    public:
        PropertiesWithNonOverrideableDefaults(
            /* [in] */ IProperties* defaults)
        {}

        ECode Clone(
            /* [out] */ IInterface** obj) override;

        ECode ToString(
            /* [in] */ String* str) override;
    };

public:
    static AutoPtr<IPrintStream> GetOut();

    static AutoPtr<ISecurityManager> GetSecurityManager();

    static Long GetCurrentTimeMillis();

    static Long GetNanoTime();

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    static ECode GetProperty(
        /* [in] */ const String& key,
        /* [in] */ const String& def,
        /* [out] */ String* value);

private:
    System();

    static AutoPtr<IProperties> InitProperties();

    static AutoPtr<IProperties> SetDefaultChangeableProperties(
        /* [in] */ IProperties* p);

    static void StaticInitialize();

    static ECode CheckKey(
        /* [in] */ const String& key);

private:
    static AutoPtr<IProperties> sProps;

    static AutoPtr<IProperties> sUnchangeableProps;

    friend void StaticInitializeSystem();
};

}
}

#endif // __CCM_CORE_SYSTEM_H__
