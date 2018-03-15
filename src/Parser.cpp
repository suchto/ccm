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

#include "Parser.h"
#include "ccdl/ArrayType.h"
#include "ccdl/PointerType.h"
#include "util/Logger.h"

using ccm::ccdl::ArrayType;
using ccm::ccdl::PointerType;

namespace ccm {

const String Parser::TAG("Parser");

bool Parser::Parse(
    /* [in] */ const std::shared_ptr<File>& file)
{
    mFile = file;

    if (!mFile->IsValid()) {
        Logger::E(TAG, "File %s is invalid.", mFile->GetPath().string());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    mTokenizer.SetInputFile(file);

    mComponent = new Component(mFile->GetPath());
    mCurrNamespace = new Namespace(String("__global__"));
    mComponent->AddNamespace(mCurrNamespace);

    bool ret = ParseFile();

    if (ret) {
        String out = mComponent->Dump();
        printf("%s", out.string());
    }
    else DumpError();

    return ret;
}

Parser::~Parser()
{
    if (mComponent != nullptr) {
        delete mComponent;
        mComponent = nullptr;
    }
    mCurrNamespace = nullptr;

    mCurrError = mErrorHeader;
    while (mCurrError != nullptr) {
        mErrorHeader = mCurrError->mNext;
        delete mCurrError;
        mCurrError = mErrorHeader;
    }
    mErrorHeader = mCurrError = nullptr;
}

bool Parser::ParseFile()
{
    bool parseResult = true;

    Tokenizer::Token token;
    while ((token = mTokenizer.GetToken()) != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::BRACKETS_OPEN:
                parseResult = ParseDeclarationWithAttribute() && parseResult;
                continue;
            case Tokenizer::Token::COCLASS: {
                LogError(token, String("coclass should have attributes"));
                Attribute attr;
                parseResult = ParseCoclass(attr) && parseResult;
                continue;
            }
            case Tokenizer::Token::ENUM:
                parseResult = ParseEnumeration() && parseResult;
                continue;
            case Tokenizer::Token::INCLUDE:
                ParseInclude();
                continue;
            case Tokenizer::Token::INTERFACE: {
                LogError(token, String("interface should have attributes"));
                Attribute attr;
                ParseInterface(attr);
                continue;
            }
            case Tokenizer::Token::NAMESPACE:
                ParseNamespace();
                continue;
            default: {
                String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
                LogError(token, message);
                continue;
            }
        }
    }
    return parseResult;
}

bool Parser::ParseDeclarationWithAttribute()
{
    Attribute attr;
    bool parseResult = ParseAttribute(attr);

    Tokenizer::Token token = mTokenizer.GetToken();
    switch (token) {
        case Tokenizer::Token::COCLASS: {
            parseResult = ParseCoclass(attr) && parseResult;
            break;
        }
        case Tokenizer::Token::INTERFACE: {
            parseResult = ParseInterface(attr) && parseResult;
            break;
        }
        case Tokenizer::Token::MODULE: {
            parseResult = ParseModule(attr) && parseResult;
            break;
        }
        default: {
            String message = String::Format("%s is not expected.", mTokenizer.DumpToken(token));
            LogError(token, message);
            mTokenizer.UngetToken(token);
            parseResult = false;
            break;
        }
    }

    return parseResult;
}

bool Parser::ParseAttribute(
    /* [out] */ Attribute& attr)
{
    bool parseResult = true;
    Tokenizer::Token token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::UUID: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                token = mTokenizer.GetUuidNumberToken();
                if (token != Tokenizer::Token::UUID_NUMBER) {
                    LogError(token, String("uuid number is expected."));
                    while (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                        token = mTokenizer.GetToken();
                    }
                    parseResult = false;
                    break;
                }
                else {
                    attr.mUuid = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                break;
            }
            case Tokenizer::Token::VERSION: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                token = mTokenizer.GetVersionNumberToken();
                if (token != Tokenizer::Token::VERSION_NUMBER) {
                    LogError(token, String("version number is expected."));
                    while (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                        token = mTokenizer.GetToken();
                    }
                    parseResult = false;
                    break;
                }
                else {
                    attr.mVersion = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                break;
            }
            case Tokenizer::Token::DESCRIPTION: {
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_OPEN) {
                    LogError(token, String("\"(\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                token = mTokenizer.GetStringLiteralToken();
                if (token != Tokenizer::Token::STRING_LITERAL) {
                    LogError(token, String("version number is expected."));
                    while (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                        token = mTokenizer.GetToken();
                    }
                    parseResult = false;
                    break;
                }
                else {
                    attr.mDescription = mTokenizer.GetString();
                }
                token = mTokenizer.GetToken();
                if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
                    LogError(token, String("\")\" is expected."));
                    mTokenizer.UngetToken(token);
                    parseResult = false;
                }
                break;
            }
            default:
                break;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\",\" is expected."));
            mTokenizer.UngetToken(token);
            parseResult = false;
            break;
        }
    }
    return parseResult;
}

bool Parser::ParseInterface(
    /* [in] */ Attribute& attr)
{
    bool parseResult = true;

    Interface* interface = new Interface();

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        interface->SetName(mTokenizer.GetIdentifier());
    }
    else {
        LogError(token, String("Interface name is expected."));
        while (token != Tokenizer::Token::BRACES_OPEN &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        mTokenizer.UngetToken(token);
        parseResult = false;
    }

    parseResult = ParseInterfaceBody(interface) && parseResult;

    if (parseResult) {
        interface->SetNamespace(mCurrNamespace);
        interface->SetAttribute(attr);
        mComponent->AddInterface(interface);
    }
    else {
        delete interface;
    }

    return parseResult;
}

bool Parser::ParseInterfaceBody(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\"{\" is expected."));
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        mTokenizer.UngetToken(token);
        parseResult = false;
    }

    token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
        if (token == Tokenizer::Token::CONST) {
            parseResult = ParseConstDataMember(interface) && parseResult;
        }
        else if (token == Tokenizer::Token::IDENTIFIER) {
            mTokenizer.UngetToken(token);
            parseResult = ParseMethod(interface) && parseResult;
        }
        token = mTokenizer.GetToken();
    }

    return parseResult;
}

bool Parser::ParseMethod(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::IDENTIFIER) {
        LogError(token, String("Method name is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        return false;
    }

    Method* method = new Method();
    method->SetName(mTokenizer.GetIdentifier());

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::PARENTHESES_OPEN) {
        LogError(token, String("\"(\" is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete method;
        return false;
    }

    while (token != Tokenizer::Token::PARENTHESES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        parseResult = ParseParameter(method) && parseResult;
        if (!parseResult) break;
        token = mTokenizer.GetToken();
    }

    if (token != Tokenizer::Token::PARENTHESES_CLOSE) {
        LogError(token, String("\")\" is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete method;
        return false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::SEMICOLON) {
        LogError(token, String("\";\" is expected."));
        while (token != Tokenizer::Token::SEMICOLON &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete method;
        return false;
    }

    if (parseResult) {
        interface->AddMethod(method);
    }
    else {
        delete method;
    }

    return parseResult;
}

bool Parser::ParseParameter(
    /* [in] */ Method* method)
{
    Parameter* parameter = new Parameter();

    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACKETS_OPEN) {
        LogError(token, String("\"[\" is expected."));
        delete parameter;
        return false;
    }

    token = mTokenizer.GetToken();
    while (token != Tokenizer::Token::BRACKETS_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE) {
        switch (token) {
            case Tokenizer::Token::IN:
                parameter->SetAttribute(Parameter::IN);
                break;
            case Tokenizer::Token::OUT:
                parameter->SetAttribute(Parameter::OUT);
                break;
            case Tokenizer::Token::CALLEE:
                parameter->SetAttribute(Parameter::CALLEE);
                break;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACKETS_CLOSE) {
            LogError(token, String("\"]\" is expected."));
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::END_OF_FILE) {
                token = mTokenizer.GetToken();
            }
            delete parameter;
            return false;
        }
    }

    Type* type = ParseType();
    if (type != nullptr) {
        parameter->SetType(type);
    }
    else {
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete parameter;
        return false;
    }

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::IDENTIFIER) {
        LogError(token, String("Parameter name is expected."));
        while (token != Tokenizer::Token::BRACES_CLOSE &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        delete parameter;
        return false;
    }

    parameter->SetName(mTokenizer.GetIdentifier());
    method->AddParameter(parameter);
    return true;
}

Type* Parser::ParseType()
{
    Type* type = nullptr;
    Tokenizer::Token token = mTokenizer.GetToken();
    if (Tokenizer::IsPrimitiveType(token)) {
        type = mComponent->FindType(String(mTokenizer.DumpToken(token)));
    }
    else if (token == Tokenizer::Token::IDENTIFIER) {
        type = mComponent->FindType(mTokenizer.GetIdentifier());
    }
    else if (token == Tokenizer::Token::ARRAY) {
        type = ParseArrayType();
    }

    if (type == nullptr) {
        String message = String::Format("Type \"%s\" was not declared in this scope.",
                mTokenizer.DumpToken(token));
        LogError(token, message);
        return nullptr;
    }

    int ptrNumber = 0;
    token = mTokenizer.GetToken();
    while (token == Tokenizer::Token::ASTERISK) {
        ptrNumber++;
        token = mTokenizer.GetToken();
    }
    mTokenizer.UngetToken(token);

    if (ptrNumber != 0) {
        String ptrTypeStr = type->ToString();
        for (int i = 0; i < ptrNumber; i++) {
            ptrTypeStr = ptrTypeStr + "*";
        }
        Type* ptrType = mComponent->FindType(ptrTypeStr);
        if (ptrType == nullptr) {
            PointerType* ptrType = new PointerType();
            ptrType->SetBaseType(type);
            ptrType->SetPointerNumber(ptrNumber);
            mComponent->AddTemporaryType(ptrType);
        }
    }

    return type;
}

Type* Parser::ParseArrayType()
{
    Tokenizer::Token token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ANGLE_BRACKETS_OPEN) {
        LogError(token, String("\"<\" is expected."));
        return nullptr;
    }

    Type* elemType = ParseType();
    if (elemType == nullptr) return nullptr;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::ANGLE_BRACKETS_CLOSE) {
        LogError(token, String("\">\" is expected."));
        return nullptr;
    }

    String arrayTypeStr =
            String("Array<") + elemType->ToString() + String(">");
    Type* arrayType = mComponent->FindType(arrayTypeStr);
    if (arrayType == nullptr) {
        ArrayType* arrayType = new ArrayType();
        arrayType->SetElementType(elemType);
        mComponent->AddTemporaryType(arrayType);
    }

    return arrayType;
}

bool Parser::ParseConstDataMember(
    /* [in] */ Interface* interface)
{
    bool parseResult = true;

    return parseResult;
}

bool Parser::ParseExpression(
    /* [out] */ int* value)
{
    // todo:
    return false;
}

bool Parser::ParseCoclass(
    /* [in] */ Attribute& attr)
{
    // todo:
    return 0;
}

bool Parser::ParseEnumeration()
{
    Tokenizer::Token token;
    bool parseResult = true;

    Enumeration* enumeration = new Enumeration();

    token = mTokenizer.GetToken();
    if (token == Tokenizer::Token::IDENTIFIER) {
        enumeration->SetName(mTokenizer.GetIdentifier());
        enumeration->SetNamespace(mCurrNamespace);
    }
    else {
        LogError(token, String("Identifier as enumeration name is expected."));
        while (token != Tokenizer::Token::BRACES_OPEN &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        mTokenizer.UngetToken(token);
        parseResult = false;
    }

    parseResult = ParseEnumerationBody(enumeration) && parseResult;

    if (parseResult) {
        mComponent->AddEnumeration(enumeration);
    }
    else {
        delete enumeration;
    }

    return parseResult;
}

bool Parser::ParseEnumerationBody(
    /* [in] */ Enumeration* enumeration)
{
    Tokenizer::Token token;
    bool parseResult = true;

    token = mTokenizer.GetToken();
    if (token != Tokenizer::Token::BRACES_OPEN) {
        LogError(token, String("\" { \" is expected."));
        while (token != Tokenizer::Token::BRACES_OPEN &&
                token != Tokenizer::Token::END_OF_FILE) {
            token = mTokenizer.GetToken();
        }
        parseResult = false;
    }

    token = mTokenizer.GetToken();
    int value = 0;
    while (token != Tokenizer::Token::BRACES_CLOSE &&
            token != Tokenizer::Token::END_OF_FILE ) {
        String name;
        if (token == Tokenizer::Token::IDENTIFIER) {
            name = mTokenizer.GetIdentifier();
        }
        else {
            LogError(token, String("Identifier as enumeration name is expected."));
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::COMMA) {
                token = mTokenizer.GetToken();
            }
            if (token == Tokenizer::Token::COMMA) {
                token = mTokenizer.GetToken();
            }
            parseResult = false;
            continue;
        }
        token = mTokenizer.GetToken();
        if (token == Tokenizer::Token::ASSIGNMENT) {
            parseResult = ParseExpression(&value) && parseResult;
            token = mTokenizer.GetToken();
        }
        if (token == Tokenizer::Token::COMMA) {
            token = mTokenizer.GetToken();
        }
        else if (token != Tokenizer::Token::BRACES_CLOSE) {
            LogError(token, String("\")\" is expected."));
            while (token != Tokenizer::Token::BRACES_CLOSE &&
                    token != Tokenizer::Token::COMMA) {
                token = mTokenizer.GetToken();
            }
            parseResult = false;
            break;
        }
        enumeration->AddEnumerator(name, value++);
    }
    return parseResult;
}

int Parser::ParseInclude()
{
    // todo:
    return 0;
}

bool Parser::ParseModule(
    /* [in] */ Attribute& attr)
{
    bool parseResult = true;
    // todo:
    return parseResult;
}

int Parser::ParseNamespace()
{
    // todo:
    return 0;
}

void Parser::LogError(
    /* [in] */ Tokenizer::Token token,
    /* [in] */ const String& message)
{
    int lineNo = mTokenizer.GetTokenLineNo();
    int columeNo = mTokenizer.GetTokenColumnNo();

    if (mErrorHeader == nullptr) {
        mErrorHeader = new Error();
        mErrorHeader->mErrorToken = token;
        mErrorHeader->mLineNo = lineNo;
        mErrorHeader->mColumnNo = columeNo;
        mErrorHeader->mMessage = message;
        mCurrError = mErrorHeader;
        return;
    }

    Error* error = new Error();
    error->mErrorToken = token;
    error->mLineNo = lineNo;
    error->mColumnNo = columeNo;
    error->mMessage = message;
    mCurrError->mNext = error;
    mCurrError = error;
}

void Parser::DumpError()
{
    Error* error = mErrorHeader;
    while (error != nullptr) {
        Logger::E(TAG, "[Line %d, Column %d] %s",
                error->mLineNo, error->mColumnNo, error->mMessage.string());
        error = error->mNext;
    }
}

}
