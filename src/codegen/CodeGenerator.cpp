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

#include "CodeGenerator.h"
#include "../ccdl/Parameter.h"
#include "../util/File.h"
#include "../util/Logger.h"
#include "../util/StringBuilder.h"

#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

using ccm::ccdl::Parameter;
using ccm::metadata::CcdlType;
using ccm::metadata::MetaNamespace;

namespace ccm {
namespace codegen {

const String CodeGenerator::TAG("CodeGenerator");

CodeGenerator::CodeGenerator()
{
    mLicense = String("//=========================================================================\n"
           "// Copyright (C) 2018 The C++ Component Model(CCM) Open Source Project\n"
           "//\n"
           "// Licensed under the Apache License, Version 2.0 (the \"License\");\n"
           "// you may not use this file except in compliance with the License.\n"
           "// You may obtain a copy of the License at\n"
           "//\n"
           "//      http://www.apache.org/licenses/LICENSE-2.0\n"
           "//\n"
           "// Unless required by applicable law or agreed to in writing, software\n"
           "// distributed under the License is distributed on an \"AS IS\" BASIS,\n"
           "// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
           "// See the License for the specific language governing permissions and\n"
           "// limitations under the License.\n"
           "//=========================================================================\n");
}

void CodeGenerator::Generate()
{
    if (!ResolveDirectory()) return;

    GenerateTypeDeclarations();

    // GenerateCoclasses();
    // GenerateModule();
}

bool CodeGenerator::ResolveDirectory()
{
    if (!mDirectory.StartsWith("/")) {
        char* cwd = getcwd(nullptr, 0);
        mDirectory = String::Format("%s/%s", cwd, mDirectory.string());
        free(cwd);
    }

    if (!access(mDirectory.string(), R_OK | W_OK)) {
        return true;
    }

    if (mkdir(mDirectory.string(), S_IRWXU | S_IRWXG | S_IRWXO) != 0) {
        Logger::E(TAG, "Create code directory failed.");
        return false;
    }

    return true;
}

void CodeGenerator::GenerateTypeDeclarations()
{
    String filePath = String::Format("%s/%s.h", mDirectory.string(), mMetaComponent->mName);
    File file(filePath, File::WRITE);

    StringBuilder builder;

    builder.Append(mLicense);
    builder.Append("\n");

    MetaComponent* mc = mMetaComponent;
    for (int i = 0; i < mc->mNamespaceNumber; i++) {
        MetaNamespace* mn = mc->mNamespaces[i];
        if (mn->mEnumerationNumber + mn->mInterfaceNumber == 0) {
            continue;
        }
        builder.Append(GenerateNamespaceBegin(String(mn->mName)));

        builder.Append(GenerateInterfaces(mn));

        builder.Append(GenerateNamespaceEnd(String(mn->mName)));
        if (i != mc->mNamespaceNumber - 1) builder.Append("\n");
    }

    String data = builder.ToString();
    file.Write(data.string(), data.GetLength());
    file.Flush();
    file.Close();
}

String CodeGenerator::GenerateInterfaces(
    /* [in] */ MetaNamespace* mn)
{
    StringBuilder builder;

    if (mn->mInterfaceNumber > 0) {
        for (int j = 0; j < mn->mInterfaceNumber; j++) {
            builder.Append(GenerateInterfaceDeclaration(
                    mMetaComponent->mInterfaces[mn->mInterfaceIndexes[j]]));
            if (j != mn->mInterfaceNumber - 1) builder.Append("\n");
        }
    }

    return builder.ToString();
}

String CodeGenerator::GenerateInterfaceDeclaration(
    /* [in] */ MetaInterface* mi)
{
    StringBuilder builder;

    String defMacro = GenerateDefineMacro(
            String::Format("%s%s", mi->mNamespace, mi->mName));
    builder.Append("#ifndef ").Append(defMacro).Append("\n");
    builder.Append("#define ").Append(defMacro).Append("\n\n");

    builder.Append("interface ").Append(mi->mName).Append(" : public ");
    if (mi->mBaseInterfaceIndex != -1) {
        builder.Append(
                mMetaComponent->mInterfaces[mi->mBaseInterfaceIndex]->mName).Append("\n");
    }
    else builder.Append("IInterface\n");
    builder.Append("{\n");

    for (int i = 0; i < mi->mConstantNumber; i++) {
        builder.Append(GenerateConstantDeclaration(mi->mConstants[i]));
        if (i == mi->mConstantNumber - 1) builder.Append("\n");
    }

    for (int i = 0; i < mi->mMethodNumber; i++) {
        builder.Append(GenerateMethodDeclaration(mi->mMethods[i]));
        if (i != mi->mMethodNumber - 1) builder.Append("\n");
    }

    builder.Append("}\n");

    builder.Append("\n#endif //").Append(defMacro).Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateConstantDeclaration(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];

    if (mt->mKind == CcdlType::String) {
        builder.Append("    static const ");
    }
    else {
        builder.Append("    static constexpr ");
    }

    builder.Append(GenerateType(mt)).Append(" ");
    builder.Append(mc->mName).Append(" = ").Append(GenerateValue(mc)).Append(";\n");

    return builder.ToString();
}

String CodeGenerator::GenerateMethodDeclaration(
    /* [in] */ MetaMethod* mm)
{
    StringBuilder builder;

    builder.Append("    virtual ECode ").Append(mm->mName).Append("(");
    for (int i = 0; i < mm->mParameterNumber; i++) {
        builder.Append("\n        ").Append(GenerateParameter(mm->mParameters[i]));
        if (i != mm->mParameterNumber - 1) builder.Append(",");
    }
    builder.Append(") = 0;\n");

    return builder.ToString();
}

String CodeGenerator::GenerateDefineMacro(
    /* [in] */ const String& fullName)
{
    String defMacro = fullName.Replace("::", "_");
    defMacro = defMacro.ToUpperCase();
    defMacro = String("__") + defMacro + String("__");
    return defMacro;
}

String CodeGenerator::GenerateNamespaceBegin(
    /* [in] */ const String& ns)
{
    StringBuilder builder;

    if (ns.IsNullOrEmpty()) return String();

    int begin = 0;
    int index = ns.IndexOf("::");
    while (index != -1) {
        builder.Append("namespace ").Append(ns.Substring(begin, index - 1)).Append(" {\n");
        begin = index + 2;
        index = ns.IndexOf("::", begin);
    }
    builder.Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateNamespaceEnd(
    /* [in] */ const String& ns)
{
    StringBuilder builder;

    if (ns.IsNullOrEmpty()) return String();

    builder.Append("\n");

    int end = ns.LastIndexOf("::") - 1;
    int fromIndex = ns.LastIndexOf("::", end);
    while (fromIndex != -1) {
        builder.Append("} // namespace ").Append(ns.Substring(fromIndex + 2, end)).Append("\n");
        end = fromIndex - 1;
        fromIndex = ns.LastIndexOf("::", end);
    }
    builder.Append("} // namespace ").Append(ns.Substring(0, end)).Append("\n");

    return builder.ToString();
}

String CodeGenerator::GenerateParameter(
    /* [in] */ MetaParameter* mp)
{
    StringBuilder builder;

    if (mp->mAttribute & Parameter::IN) {
        if (mp->mAttribute & Parameter::OUT) {
            builder.Append("/* [in, out] */ ");
        }
        else {
            builder.Append("/* [in] */ ");
        }
    }
    else if (mp->mAttribute & Parameter::OUT) {
        if (mp->mAttribute & Parameter::CALLEE) {
            builder.Append("/* [out, callee] */ ");
        }
        else {
            builder.Append("/* [out] */ ");
        }
    }

    builder.Append(GenerateType(mMetaComponent->mTypes[mp->mTypeIndex])).Append(" ");
    builder.Append(mp->mName);

    return builder.ToString();
}

String CodeGenerator::GenerateType(
    /* [in] */ MetaType* mt)
{
    StringBuilder builder;

    MetaComponent* mc = mMetaComponent;
    switch(mt->mKind) {
        case CcdlType::Char:
            builder.Append("Char");
            break;
        case CcdlType::Byte:
            builder.Append("Byte");
            break;
        case CcdlType::Short:
            builder.Append("Short");
            break;
        case CcdlType::Integer:
            builder.Append("Integer");
            break;
        case CcdlType::Long:
            builder.Append("Long");
            break;
        case CcdlType::Float:
            builder.Append("Float");
            break;
        case CcdlType::Double:
            builder.Append("Double");
            break;
        case CcdlType::Boolean:
            builder.Append("Boolean");
            break;
        case CcdlType::String:
            builder.Append("String");
            break;
        case CcdlType::HANDLE:
            builder.Append("HANDLE");
            break;
        case CcdlType::Enum:
            builder.Append(mc->mEnumerations[mt->mIndex]->mName);
            break;
        case CcdlType::Array:
            builder.Append("Array<").Append(
                    GenerateType(mc->mTypes[mt->mNestedTypeIndex])).Append(">");
            break;
        case CcdlType::Interface:
            builder.Append(mc->mInterfaces[mt->mIndex]->mName);
            break;
        default:
            break;
    }

    for (int i = 0; i < mt->mPointerNumber; i++) {
        builder.Append("*");
    }

    return builder.ToString();
}

String CodeGenerator::GenerateValue(
    /* [in] */ MetaConstant* mc)
{
    StringBuilder builder;

    MetaType* mt = mMetaComponent->mTypes[mc->mTypeIndex];
    switch(mt->mKind) {
        case CcdlType::Char:
            return String::Format("\'%c\'", mc->mValue.mInteger);
        case CcdlType::Byte: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, (unsigned char)mc->mValue.mInteger);
        }
        case CcdlType::Short: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, (short)mc->mValue.mInteger);
        }
        case CcdlType::Integer: {
            const char* format = mc->mRadix == 8 ? "%o" :
                    mc->mRadix == 10 ? "%d" : "%x";
            return String::Format(format, mc->mValue.mInteger);
        }
        case CcdlType::Long: {
            const char* format = mc->mRadix == 8 ? "%llo" :
                    mc->mRadix == 10 ? "%lld" : "%llx";
            return String::Format(format, mc->mValue.mLong);
        }
        case CcdlType::Float:
            return String::Format("%f", mc->mValue.mFloat);
        case CcdlType::Double:
            return String::Format("%e", mc->mValue.mDouble);
        case CcdlType::Boolean:
            return mc->mValue.mBoolean ? String("true") : String("false");
        case CcdlType::String:
            return String::Format("\"%s\"", mc->mValue.mString);
        case CcdlType::Enum:
            return String(mc->mValue.mString);
        case CcdlType::Array:
        case CcdlType::HANDLE:
        case CcdlType::Interface:
        default:
            break;
    }

    return builder.ToString();
}

}
}