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

#ifndef __CDLC_NAMESPACE_H__
#define __CDLC_NAMESPACE_H__

#include "ast/Constant.h"
#include "ast/EnumerationType.h"
#include "ast/InterfaceType.h"
#include "ast/Node.h"
#include "ast/Type.h"
#include "util/AutoPtr.h"
#include "util/String.h"
#include <vector>

namespace cdlc {

class Module;

class Namespace
    : public Node
{
public:
    inline Namespace(
        /* [in] */ const String& name,
        /* [in] */ Module* module);

    inline Namespace(
        /* [in] */ InterfaceType* interfaceWrapped,
        /* [in] */ Module* module);

    inline AutoPtr<Namespace> GetParent();

    inline void SetParent(
        /* [in] */ Namespace* parent);

    void AddNamespace(
        /* [in] */ Namespace* ns);

    AutoPtr<Namespace> FindNamespace(
        /* [in] */ const String& nsString);

    void AddConstant(
        /* [in] */ Constant* constant);

    void AddEnumerationType(
        /* [in] */ EnumerationType* enumeration);

    void AddInterfaceType(
        /* [in] */ InterfaceType* interface);

    void AddType(
        /* [in] */ Type* type);

    inline bool IsGlobal();

    inline String ToShortString();

    String ToString() override;

    String Dump(
        /* [in] */ const String& prefix) override;

public:
    static const String GLOBAL_NAME;

private:
    // mName does not include "::"
    String mName;
    Namespace* mParent = nullptr;
    std::vector<AutoPtr<Namespace>> mChildren;
    std::vector<AutoPtr<Constant>> mConstants;
    std::vector<AutoPtr<Type>> mTypes;
    bool mIsWrapper = false;
    InterfaceType* mInterfaceWrapped = nullptr;

    Module* mModule = nullptr;
};

Namespace::Namespace(
    /* [in] */ const String& name,
    /* [in] */ Module* module)
    : mName(name)
    , mModule(module)
{}

Namespace::Namespace(
    /* [in] */ InterfaceType* interfaceWrapped,
    /* [in] */ Module* module)
    : mName(interfaceWrapped->GetName())
    , mIsWrapper(true)
    , mInterfaceWrapped(interfaceWrapped)
    , mModule(module)
{}

AutoPtr<Namespace> Namespace::GetParent()
{
    return mParent;
}

void Namespace::SetParent(
    /* [in] */ Namespace* parent)
{
    mParent = parent;
}

bool Namespace::IsGlobal()
{
    return mName.Equals(GLOBAL_NAME);
}

String Namespace::ToShortString()
{
    return mName;
}

}

#endif // __CDLC_NAMESPACE_H__
