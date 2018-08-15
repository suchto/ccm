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

#include "coredef.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/util/Collections.h"
#include "ccm/util/CTreeMap.h"
#include "ccm/util/CTreeSet.h"
#include "ccm/util/locale/InternalLocaleBuilder.h"
#include "ccm/util/locale/LanguageTag.h"
#include "ccm/util/locale/LocaleExtensions.h"
#include "ccm/util/locale/LocaleUtils.h"
#include "ccm/util/locale/UnicodeLocaleExtension.h"
#include "ccm.core.IChar.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.core.IStringBuilder.h"
#include "ccm.util.IMapEntry.h"

using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IChar;
using ccm::core::ICharSequence;
using ccm::core::IStringBuilder;
using ccm::core::IID_IStringBuilder;

namespace ccm {
namespace util {
namespace locale {

static const InterfaceID IID_LocaleExtensions =
        {{0xd0928ef6,0x5d64,0x40af,0xb577,{0xc,0x6,0x2,0xe,0xb,0xd,0x1,0xc,0xe,0x8,0x6,0xb}}, &CID_libcore};

AutoPtr<LocaleExtensions> LocaleExtensions::GetCALENDAR_JAPANESE()
{
    static AutoPtr<LocaleExtensions> CALENDAR_JAPANESE =
            new LocaleExtensions(String("u-ca-japanese"),
                                 UnicodeLocaleExtension::SINGLETON,
                                 UnicodeLocaleExtension::GetCA_JAPANESE());
    return CALENDAR_JAPANESE;
}

AutoPtr<LocaleExtensions> LocaleExtensions::GetNUMBER_THAI()
{
    static AutoPtr<LocaleExtensions> NUMBER_THAI =
            new LocaleExtensions(String("u-nu-thai"),
                                 UnicodeLocaleExtension::SINGLETON,
                                 UnicodeLocaleExtension::GetNU_THAI());
    return NUMBER_THAI;
}

LocaleExtensions::LocaleExtensions(
    /* [in] */ const String& id,
    /* [in] */ Char key,
    /* [in] */ Extension* value)
    : mId(id)
{
    mExtensionMap = Collections::CreateSingletonMap(CoreUtils::Box(key), (IObject*)value);
}

LocaleExtensions::LocaleExtensions(
    /* [in] */ IMap* extensions,
    /* [in] */ ISet* uattributes,
    /* [in] */ IMap* ukeywords)
{
    Boolean hasExtension = !LocaleUtils::IsEmpty(extensions);
    Boolean hasUAttributes = !LocaleUtils::IsEmpty(uattributes);
    Boolean hasUKeywords = !LocaleUtils::IsEmpty(ukeywords);

    if (!hasExtension && !hasUAttributes && !hasUKeywords) {
        mId = "";
        mExtensionMap = Collections::GetEmptyMap();
        return;
    }

    AutoPtr<ISortedMap> map;
    CTreeMap::New(IID_ISortedMap, (IInterface**)&map);
    if (hasExtension) {
        AutoPtr<ISet> entryset;
        extensions->GetEntrySet((ISet**)&entryset);
        FOR_EACH(IMapEntry*, ext, IMapEntry::Probe, entryset) {
            AutoPtr<IInterface> keyObj, valueObj;
            ext->GetKey((IInterface**)&keyObj);
            ext->GetValue((IInterface**)&valueObj);
            Char key = LocaleUtils::ToLower(
                    ((InternalLocaleBuilder::CaseInsensitiveChar*)IObject::Probe(keyObj))->Value());
            String value = CoreUtils::Unbox(ICharSequence::Probe(valueObj));

            if (LanguageTag::IsPrivateusePrefixChar(key)) {
                // we need to exclude special variant in privuateuse, e.g. "x-abc-lvariant-DEF"
                value = InternalLocaleBuilder::RemovePrivateuseVariant(value);
                if (value.IsNull()) {
                    continue;
                }
            }

            IMap::Probe(map)->Put(CoreUtils::Box(key), (IObject*)new Extension(key, LocaleUtils::ToLowerString(value)));
        } END_FOR_EACH()
    }

    if (hasUAttributes || hasUKeywords) {
        AutoPtr<ISortedSet> uaset;
        AutoPtr<ISortedMap> ukmap;

        if (hasUAttributes) {
            CTreeSet::New(IID_ISortedSet, (IInterface**)&uaset);
            FOR_EACH(InternalLocaleBuilder::CaseInsensitiveString*, cis, IObject::Probe, uattributes) {
                ISet::Probe(uaset)->Add(CoreUtils::Box(LocaleUtils::ToLowerString(cis->Value())));
            } END_FOR_EACH()
        }

        if (hasUKeywords) {
            CTreeMap::New(IID_ISortedMap, (IInterface**)&ukmap);
            AutoPtr<ISet> entryset;
            ukeywords->GetEntrySet((ISet**)&entryset);
            FOR_EACH(IMapEntry*, kwd, IMapEntry::Probe, entryset) {
                AutoPtr<IInterface> keyObj, valueObj;
                kwd->GetKey((IInterface**)&keyObj);
                kwd->GetValue((IInterface**)&valueObj);
                String key = LocaleUtils::ToLowerString(
                        ((InternalLocaleBuilder::CaseInsensitiveString*)IObject::Probe(keyObj))->Value());
                String type = LocaleUtils::ToLowerString(CoreUtils::Unbox(ICharSequence::Probe(valueObj)));
                IMap::Probe(ukmap)->Put(CoreUtils::Box(key), CoreUtils::Box(type));
            } END_FOR_EACH()
        }

        AutoPtr<UnicodeLocaleExtension> ule = new UnicodeLocaleExtension(uaset, ukmap);
        IMap::Probe(map)->Put(CoreUtils::Box(UnicodeLocaleExtension::SINGLETON), (IObject*)ule.Get());
    }

    Boolean empty;
    if (IMap::Probe(map)->IsEmpty(&empty), empty) {
        // this could happen when only privuateuse with special variant
        mId = "";
        mExtensionMap = Collections::GetEmptyMap();
    }
    else {
        mId = ToID(map);
        mExtensionMap = IMap::Probe(map);
    }
}

IInterface* LocaleExtensions::Probe(
    /* [in] */ const InterfaceID& iid)
{
    if (iid == IID_LocaleExtensions) {
        return (IInterface*)(IObject*)this;
    }
    return Object::Probe(iid);
}

AutoPtr<ISet> LocaleExtensions::GetKeys()
{
    Boolean empty;
    if (mExtensionMap->IsEmpty(&empty), empty) {
        return Collections::GetEmptySet();
    }
    AutoPtr<ISet> keyset;
    mExtensionMap->GetKeySet((ISet**)&keyset);
    return Collections::CreateUnmodifiableSet(keyset);
}

AutoPtr<Extension> LocaleExtensions::GetExtension(
    /* [in] */ Char key)
{
    AutoPtr<IInterface> value;
    mExtensionMap->Get(CoreUtils::Box(LocaleUtils::ToLower(key)), (IInterface**)&value);
    return (Extension*)IObject::Probe(value);
}

String LocaleExtensions::GetExtensionValue(
    /* [in] */ Char key)
{
    AutoPtr<IInterface> value;
    mExtensionMap->Get(CoreUtils::Box(LocaleUtils::ToLower(key)), (IInterface**)&value);
    if (value == nullptr) {
        return String();
    }
    return ((Extension*)IObject::Probe(value))->GetValue();
}

AutoPtr<ISet> LocaleExtensions::GetUnicodeLocaleAttributes()
{
    AutoPtr<IInterface> value;
    mExtensionMap->Get(CoreUtils::Box(UnicodeLocaleExtension::SINGLETON),
            (IInterface**)&value);
    if (value == nullptr) {
        return Collections::GetEmptySet();
    }
    return ((UnicodeLocaleExtension*)IObject::Probe(value))->GetUnicodeLocaleAttributes();
}

AutoPtr<ISet> LocaleExtensions::GetUnicodeLocaleKeys()
{
    AutoPtr<IInterface> value;
    mExtensionMap->Get(CoreUtils::Box(UnicodeLocaleExtension::SINGLETON),
            (IInterface**)&value);
    if (value == nullptr) {
        return Collections::GetEmptySet();
    }
    return ((UnicodeLocaleExtension*)IObject::Probe(value))->GetUnicodeLocaleKeys();
}

String LocaleExtensions::GetUnicodeLocaleType(
    /* [in] */ const String& unicodeLocaleKey)
{
    AutoPtr<IInterface> value;
    mExtensionMap->Get(CoreUtils::Box(UnicodeLocaleExtension::SINGLETON),
            (IInterface**)&value);
    if (value == nullptr) {
        return String();
    }
    return ((UnicodeLocaleExtension*)IObject::Probe(value))->GetUnicodeLocaleType(
            LocaleUtils::ToLowerString(unicodeLocaleKey));
}

Boolean LocaleExtensions::IsEmpty()
{
    Boolean empty;
    mExtensionMap->IsEmpty(&empty);
    return empty;
}

Boolean LocaleExtensions::IsValidKey(
    /* [in] */ Char c)
{
    return LanguageTag::IsExtensionSingletonChar(c) ||
           LanguageTag::IsPrivateusePrefixChar(c);
}

Boolean LocaleExtensions::IsValidUnicodeLocaleKey(
    /* [in] */ const String& ukey)
{
    return UnicodeLocaleExtension::IsKey(ukey);
}

String LocaleExtensions::ToID(
    /* [in] */ ISortedMap* map)
{
    AutoPtr<IStringBuilder> buf;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&buf);
    AutoPtr<Extension> privuse;
    AutoPtr<ISet> entryset;
    IMap::Probe(map)->GetEntrySet((ISet**)&entryset);
    FOR_EACH(IMapEntry*, entry, IMapEntry::Probe, entryset) {
        AutoPtr<IInterface> singleton, extension;
        entry->GetKey((IInterface**)&singleton);
        entry->GetValue((IInterface**)&extension);
        if (LanguageTag::IsPrivateusePrefixChar(CoreUtils::Unbox(IChar::Probe(singleton)))) {
            privuse = (Extension*)IObject::Probe(extension);
        }
        else {
            Integer len;
            if (buf->GetLength(&len), len > 0) {
                buf->Append(ILanguageTag::SEP);
            }
            buf->Append(extension);
        }
    } END_FOR_EACH()
    if (privuse != nullptr) {
        Integer len;
        if (buf->GetLength(&len), len > 0) {
            buf->Append(ILanguageTag::SEP);
        }
        buf->Append((IObject*)privuse.Get());
    }
    String str;
    buf->ToString(&str);
    return str;
}

ECode LocaleExtensions::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    *desc = mId;
    return NOERROR;
}

ECode LocaleExtensions::GetHashCode(
    /* [out] */ Integer* hash)
{
    VALIDATE_NOT_NULL(hash);

    *hash = mId.GetHashCode();
    return NOERROR;
}

ECode LocaleExtensions::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* same)
{
    VALIDATE_NOT_NULL(same);

    if (IInterface::Equals((IObject*)this, obj)) {
        *same = true;
        return NOERROR;
    }
    if (obj->Probe(IID_LocaleExtensions) == nullptr) {
        *same = false;
        return NOERROR;
    }
    *same = mId.Equals(((LocaleExtensions*)IObject::Probe(obj))->mId);
    return NOERROR;
}

}
}
}
