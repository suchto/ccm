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

#ifndef __CCDL_AST_INTERFACE_H__
#define __CCDL_AST_INTERFACE_H__

#include "Type.h"
#include "Attribute.h"
#include "Constant.h"
#include "Method.h"
#include "../util/ArrayList.h"
#include "../util/Uuid.h"

namespace ccdl {
class Parser;
}

namespace ccdl {
namespace ast {

class Pool;

class Interface : public Type
{
public:
    Interface();

    void SetNamespace(
        /* [in] */ Namespace* ns) override;

    bool IsInterfaceType() override;

    inline Interface* GetBaseInterface();

    void SetBaseInterface(
        /* [in] */ Interface* baseItf);

    inline bool IsDeclared();

    inline void SetDeclared(
        /* [in] */ bool declared);

    inline Uuid& GetUuid();

    void SetAttribute(
        /* [in] */ const Attribute& attr);

    bool AddConstant(
        /* [in] */ Constant* constant);

    inline int GetConstantNumber();

    inline Constant* GetConstant(
        /* [in] */ int index);

    bool AddMethod(
        /* [in] */ Method* method);

    inline int GetMethodNumber();

    inline Method* GetMethod(
        /* [in] */ int index);

    Method* FindMethod(
        /* [in] */ const String& name,
        /* [in] */ const String& signature);

    String Signature() override;

    void DeepCopy(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

    void ShallowCopy(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

    void Specialize();

    String Dump(
        /* [in] */ const String& prefix) override;

private:
    void SpecializeInternal(
        /* [in] */ Interface* source,
        /* [in] */ Pool* pool);

private:
    bool mDeclared;
    Interface* mBaseInterface;
    Uuid mUuid;
    String mVersion;
    String mDescription;
    ArrayList<Constant*> mConstants;
    ArrayList<Method*> mMethods;
};

Interface* Interface::GetBaseInterface()
{
    return mBaseInterface;
}

bool Interface::IsDeclared()
{
    return mDeclared;
}

void Interface::SetDeclared(
    /* [in] */ bool declared)
{
    mDeclared = declared;
}

Uuid& Interface::GetUuid()
{
    return mUuid;
}

int Interface::GetConstantNumber()
{
    return mConstants.GetSize();
}

Constant* Interface::GetConstant(
    /* [in] */ int index)
{
    return mConstants.Get(index);
}

int Interface::GetMethodNumber()
{
    return mMethods.GetSize();
}

Method* Interface::GetMethod(
    /* [in] */ int index)
{
    return mMethods.Get(index);
}

}
}

#endif // __CCDL_AST_INTERFACE_H__
