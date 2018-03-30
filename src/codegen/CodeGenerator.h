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

#ifndef __CCM_CODEGEN_CODEGENERATOR_H__
#define __CCM_CODEGEN_CODEGENERATOR_H__

#include "../metadata/Component.h"
#include "../util/String.h"

using ccm::metadata::MetaComponent;
using ccm::metadata::MetaConstant;
using ccm::metadata::MetaInterface;
using ccm::metadata::MetaMethod;
using ccm::metadata::MetaNamespace;
using ccm::metadata::MetaParameter;
using ccm::metadata::MetaType;

namespace ccm {
namespace codegen {

class CodeGenerator
{
public:
    CodeGenerator();

    void Generate();

    inline void SetDirectory(
        /* [in] */ const String& dir)
    { mDirectory = dir; }

    inline void SetMetadata(
        /* [in] */ MetaComponent* component)
    { mMetaComponent = component; }

    inline void SetLicense(
        /* [in] */ const String& license);

private:
    bool ResolveDirectory();

    void GenerateTypeDeclarations();

    String GenerateInterfaces(
        /* [in] */ MetaNamespace* mn);

    void GenerateCoclasses();

    void GenerateModule();

    String GenerateInterfaceDeclaration(
        /* [in] */ MetaInterface* mi);

    String GenerateConstantDeclaration(
        /* [in] */ MetaConstant* mc);

    String GenerateMethodDeclaration(
        /* [in] */ MetaMethod* mm);

    String GenerateDefineMacro(
        /* [in] */ const String& fullName);

    String GenerateNamespaceBegin(
        /* [in] */ const String& ns);

    String GenerateNamespaceEnd(
        /* [in] */ const String& ns);

    String GenerateParameter(
        /* [in] */ MetaParameter* mp);

    String GenerateType(
        /* [in] */ MetaType* mt);

    String GenerateValue(
        /* [in] */ MetaConstant* mc);

private:
    static const String TAG;
    String mDirectory;
    MetaComponent* mMetaComponent;
    String mLicense;
};

}
}

#endif // __CCM_CODEGEN_CODEGENERATOR_H__
