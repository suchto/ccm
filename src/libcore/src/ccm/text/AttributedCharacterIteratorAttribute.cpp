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

#include "ccm/core/CoreUtils.h"
#include "ccm/text/AttributedCharacterIteratorAttribute.h"
#include "ccm/text/CAttributedCharacterIteratorAttribute.h"
#include "ccm/util/CHashMap.h"
#include <ccmlogger.h>

using ccm::core::CoreUtils;
using ccm::io::E_INVALID_OBJECT_EXCEPTION;
using ccm::util::CHashMap;
using ccm::util::IID_IMap;

namespace ccm {
namespace text {

CCM_INTERFACE_IMPL_1(AttributedCharacterIteratorAttribute, SyncObject, IAttributedCharacterIteratorAttribute);

static AutoPtr<IMap> CreateHashMap()
{
    AutoPtr<IMap> map;
    CHashMap::New(7, IID_IMap, (IInterface**)&map);
    return map;
}

AutoPtr<IMap> AttributedCharacterIteratorAttribute::GetInstanceMap()
{
    static const AutoPtr<IMap> sInstanceMap = CreateHashMap();
    return sInstanceMap;
}

ECode AttributedCharacterIteratorAttribute::Constructor(
    /* [in] */ const String& name)
{
    mName = name;
    CoclassID cid;
    GetCoclassID(&cid);
    if (cid == CID_CAttributedCharacterIteratorAttribute) {
        GetInstanceMap()->Put(CoreUtils::Box(name), (IAttributedCharacterIteratorAttribute*)this);
    }
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = String::Format("%s(%s)",
            Object::GetCoclassName((IAttributedCharacterIteratorAttribute*)this).string(), mName.string());
    return NOERROR;
}

String AttributedCharacterIteratorAttribute::GetName()
{
    return mName;
}

ECode AttributedCharacterIteratorAttribute::ReadResolve(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj);

    CoclassID cid;
    GetCoclassID(&cid);
    if (cid != CID_CAttributedCharacterIteratorAttribute) {
        Logger::E("AttributedCharacterIteratorAttribute", "subclass didn't correctly implement readResolve");
        return E_INVALID_OBJECT_EXCEPTION;
    }

    GetInstanceMap()->Get(CoreUtils::Box(GetName()), obj);
    if (*obj == nullptr) {
        Logger::E("AttributedCharacterIteratorAttribute", "unknown attribute name");
        return E_INVALID_OBJECT_EXCEPTION;
    }
    return NOERROR;
}

static AutoPtr<IAttributedCharacterIteratorAttribute> CreateAttributedCharacterIteratorAttribute(
    /* [in] */ const String& name)
{
    AutoPtr<IAttributedCharacterIteratorAttribute> attribute;
    CAttributedCharacterIteratorAttribute::New(name, IID_IAttributedCharacterIteratorAttribute, (IInterface**)&attribute);
    return attribute;
}

ECode AttributedCharacterIteratorAttribute::GetLANGUAGE(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIteratorAttribute> sLANGUAGE =
            CreateAttributedCharacterIteratorAttribute(String("language"));

    *attribute = sLANGUAGE;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetREADING(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIteratorAttribute> sREADING =
            CreateAttributedCharacterIteratorAttribute(String("reading"));

    *attribute = sREADING;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

ECode AttributedCharacterIteratorAttribute::GetINPUT_METHOD_SEGMENT(
    /* [out] */ IAttributedCharacterIteratorAttribute** attribute)
{
    VALIDATE_NOT_NULL(attribute);

    static const AutoPtr<IAttributedCharacterIteratorAttribute> sINPUT_METHOD_SEGMENT =
            CreateAttributedCharacterIteratorAttribute(String("input_method_segment"));

    *attribute = sINPUT_METHOD_SEGMENT;
    REFCOUNT_ADD(*attribute);
    return NOERROR;
}

}
}
