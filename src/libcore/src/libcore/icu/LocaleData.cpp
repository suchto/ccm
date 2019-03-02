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

#include "ccm/core/AutoLock.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/StringUtils.h"
#include "ccm/text/DateFormat.h"
#include "ccm/util/CHashMap.h"
#include "ccm/util/CLocale.h"
#include "ccm.core.ICharSequence.h"
#include "ccm.text.IDateFormat.h"
#include "libcore/icu/ICU.h"
#include "libcore/icu/LocaleData.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::E_ASSERTION_ERROR;
using ccm::core::E_NULL_POINTER_EXCEPTION;
using ccm::core::ICharSequence;
using ccm::core::IID_IStringBuilder;
using ccm::core::IStringBuilder;
using ccm::core::StringUtils;
using ccm::text::DateFormat;
using ccm::text::IDateFormat;
using ccm::util::CHashMap;
using ccm::util::CLocale;
using ccm::util::IID_IHashMap;

namespace libcore {
namespace icu {

static AutoPtr<IHashMap> CreateHashMap()
{
    AutoPtr<IHashMap> map;
    CHashMap::New(IID_IHashMap, (IInterface**)&map);
    return map;
}

AutoPtr<IHashMap> LocaleData::GetLocaleDataCache()
{
    static AutoPtr<IHashMap> sLocaleDataCache = CreateHashMap();
    return sLocaleDataCache;
}

Boolean LocaleData::StaticInitialize()
{
    GetInner(CLocale::GetROOT(), nullptr);
    GetInner(CLocale::GetUS(), nullptr);
    GetInner(CLocale::GetDefault(), nullptr);
    return true;
}

CCM_INTERFACE_IMPL_1(LocaleData, SyncObject, ILocaleData);

AutoPtr<ILocale> LocaleData::MapInvalidAndNullLocales(
    /* [in] */ ILocale* locale)
{
    if (locale == nullptr) {
        return CLocale::GetDefault();
    }

    String language;
    locale->ToLanguageTag(&language);
    if (language.Equals("und")) {
        return CLocale::GetROOT();
    }

    return locale;
}

ECode LocaleData::Get(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocaleData** data)
{
    VALIDATE_NOT_NULL(data);

    static Boolean initialize = StaticInitialize();

    return GetInner(locale, data);
}

ECode LocaleData::GetInner(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocaleData** data)
{
    if (locale == nullptr) {
        Logger::E("LocaleData", "locale == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    String languageTag;
    locale->ToLanguageTag(&languageTag);
    AutoPtr<IHashMap> localeDataCache = GetLocaleDataCache();
    {
        AutoLock lock(ISynchronize::Probe(localeDataCache));

        AutoPtr<ILocaleData> localeData;
        localeDataCache->Get(CoreUtils::Box(languageTag), (IInterface**)&localeData);
        if (localeData != nullptr && data != nullptr) {
            localeData.MoveTo(data);
            return NOERROR;
        }
    }
    AutoPtr<ILocaleData> newLocaleData;
    FAIL_RETURN(InitLocaleData(locale, &newLocaleData));
    {
        AutoLock lock(ISynchronize::Probe(localeDataCache));

        AutoPtr<ICharSequence> key = CoreUtils::Box(languageTag);
        AutoPtr<ILocaleData> localeData;
        localeDataCache->Get(key, (IInterface**)&localeData);
        if (localeData != nullptr && data != nullptr) {
            localeData.MoveTo(data);
            return NOERROR;
        }
        localeDataCache->Put(key, newLocaleData);
        if (data != nullptr) {
            newLocaleData.MoveTo(data);
        }
        return NOERROR;
    }
}

ECode LocaleData::ToString(
    /* [out] */ String* desc)
{
    VALIDATE_NOT_NULL(desc);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);

    sb->Append(String("LocaleData["));

    sb->Append(String("mFirstDayOfWeek="));
    sb->Append(Object::ToString(mFirstDayOfWeek));
    sb->Append(String(",mMinimalDaysInFirstWeek="));
    sb->Append(Object::ToString(mMinimalDaysInFirstWeek));

    sb->Append(String(",mAmPm=("));
    for (Long i = 0; i < mAmPm.GetLength(); i++) {
        sb->Append(mAmPm[i]);
        if (i != mAmPm.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mEras=("));
    for (Long i = 0; i < mEras.GetLength(); i++) {
        sb->Append(mEras[i]);
        if (i != mEras.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mLongMonthNames=("));
    for (Long i = 0; i < mLongMonthNames.GetLength(); i++) {
        sb->Append(mLongMonthNames[i]);
        if (i != mLongMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mShortMonthNames=("));
    for (Long i = 0; i < mShortMonthNames.GetLength(); i++) {
        sb->Append(mShortMonthNames[i]);
        if (i != mShortMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mTinyMonthNames=("));
    for (Long i = 0; i < mTinyMonthNames.GetLength(); i++) {
        sb->Append(mTinyMonthNames[i]);
        if (i != mTinyMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mLongStandAloneMonthNames=("));
    for (Long i = 0; i < mLongStandAloneMonthNames.GetLength(); i++) {
        sb->Append(mLongStandAloneMonthNames[i]);
        if (i != mLongStandAloneMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mShortStandAloneMonthNames=("));
    for (Long i = 0; i < mShortStandAloneMonthNames.GetLength(); i++) {
        sb->Append(mShortStandAloneMonthNames[i]);
        if (i != mShortStandAloneMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mTinyStandAloneMonthNames=("));
    for (Long i = 0; i < mTinyStandAloneMonthNames.GetLength(); i++) {
        sb->Append(mTinyStandAloneMonthNames[i]);
        if (i != mTinyStandAloneMonthNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mLongWeekdayNames=("));
    for (Long i = 0; i < mLongWeekdayNames.GetLength(); i++) {
        sb->Append(mLongWeekdayNames[i]);
        if (i != mLongWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mShortWeekdayNames=("));
    for (Long i = 0; i < mShortWeekdayNames.GetLength(); i++) {
        sb->Append(mShortWeekdayNames[i]);
        if (i != mShortWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mTinyWeekdayNames=("));
    for (Long i = 0; i < mTinyWeekdayNames.GetLength(); i++) {
        sb->Append(mTinyWeekdayNames[i]);
        if (i != mTinyWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mLongStandAloneWeekdayNames=("));
    for (Long i = 0; i < mLongStandAloneWeekdayNames.GetLength(); i++) {
        sb->Append(mLongStandAloneWeekdayNames[i]);
        if (i != mLongStandAloneWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mShortStandAloneWeekdayNames=("));
    for (Long i = 0; i < mShortStandAloneWeekdayNames.GetLength(); i++) {
        sb->Append(mShortStandAloneWeekdayNames[i]);
        if (i != mShortStandAloneWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mTinyStandAloneWeekdayNames=("));
    for (Long i = 0; i < mTinyStandAloneWeekdayNames.GetLength(); i++) {
        sb->Append(mTinyStandAloneWeekdayNames[i]);
        if (i != mTinyStandAloneWeekdayNames.GetLength() - 1) {
            sb->Append(U',');
        }
    }
    sb->Append(U')');

    sb->Append(String(",mYesterday="));
    sb->Append(mYesterday);
    sb->Append(String(",mToday="));
    sb->Append(mToday);
    sb->Append(String(",mTomorrow="));
    sb->Append(mTomorrow);

    sb->Append(String(",mFullTimeFormat="));
    sb->Append(mFullTimeFormat);
    sb->Append(String(",mLongTimeFormat="));
    sb->Append(mLongTimeFormat);
    sb->Append(String(",mMediumTimeFormat="));
    sb->Append(mMediumTimeFormat);
    sb->Append(String(",mShortTimeFormat="));
    sb->Append(mShortTimeFormat);

    sb->Append(String(",mFullDateFormat="));
    sb->Append(mFullDateFormat);
    sb->Append(String(",mLongDateFormat="));
    sb->Append(mLongDateFormat);
    sb->Append(String(",mMediumDateFormat="));
    sb->Append(mMediumDateFormat);
    sb->Append(String(",mShortDateFormat="));
    sb->Append(mShortDateFormat);

    sb->Append(String(",mNarrowAm="));
    sb->Append(mNarrowAm);
    sb->Append(String(",mNarrowPm="));
    sb->Append(mNarrowPm);

    sb->Append(String(",mTimeFormat_hm="));
    sb->Append(mTimeFormat_hm);
    sb->Append(String(",mTimeFormat_Hm="));
    sb->Append(mTimeFormat_Hm);
    sb->Append(String(",mTimeFormat_hms="));
    sb->Append(mTimeFormat_hms);
    sb->Append(String(",mTimeFormat_Hms="));
    sb->Append(mTimeFormat_Hms);

    sb->Append(String(",mZeroDigit="));
    sb->Append(mZeroDigit);
    sb->Append(String(",mDecimalSeparator="));
    sb->Append(mDecimalSeparator);
    sb->Append(String(",mGroupingSeparator="));
    sb->Append(mGroupingSeparator);
    sb->Append(String(",mPatternSeparator="));
    sb->Append(mPatternSeparator);
    sb->Append(String(",mPercent="));
    sb->Append(mPercent);
    sb->Append(String(",mPerMill="));
    sb->Append(mPerMill);
    sb->Append(String(",mMonetarySeparator="));
    sb->Append(mMonetarySeparator);
    sb->Append(String(",mMinusSign="));
    sb->Append(mMinusSign);
    sb->Append(String(",mExponentSeparator="));
    sb->Append(mExponentSeparator);
    sb->Append(String(",mInfinity="));
    sb->Append(mInfinity);
    sb->Append(String(",mNaN="));
    sb->Append(mNaN);

    sb->Append(String(",mCurrencySymbol="));
    sb->Append(mCurrencySymbol);
    sb->Append(String(",mInternationalCurrencySymbol="));
    sb->Append(mInternationalCurrencySymbol);

    sb->Append(String(",mNumberPattern="));
    sb->Append(mNumberPattern);
    sb->Append(String(",mIntegerPattern="));
    sb->Append(mIntegerPattern);
    sb->Append(String(",mCurrencyPattern="));
    sb->Append(mCurrencyPattern);
    sb->Append(String(",mPercentPattern="));
    sb->Append(mPercentPattern);

    sb->Append(U']');

    return sb->ToString(desc);
}

ECode LocaleData::GetDateFormat(
    /* [in] */ Integer style,
    /* [out] */ String* dateFormat)
{
    VALIDATE_NOT_NULL(dateFormat);

    switch(style) {
        case IDateFormat::SHORT:
            *dateFormat = mShortDateFormat;
            return NOERROR;
        case IDateFormat::MEDIUM:
            *dateFormat = mMediumDateFormat;
            return NOERROR;
        case IDateFormat::LONG:
            *dateFormat = mLongDateFormat;
            return NOERROR;
        case IDateFormat::FULL:
            *dateFormat = mFullDateFormat;
            return NOERROR;
    }
    return E_ASSERTION_ERROR;
}

ECode LocaleData::GetTimeFormat(
    /* [in] */ Integer style,
    /* [out] */ String* timeFormat)
{
    VALIDATE_NOT_NULL(timeFormat);

    switch(style) {
        case IDateFormat::SHORT:
            if (DateFormat::sIs24Hour == nullptr) {
                *timeFormat = mShortTimeFormat;
                return NOERROR;
            }
            else {
                *timeFormat = CoreUtils::Unbox(DateFormat::sIs24Hour) ?
                        mTimeFormat_Hm : mTimeFormat_hm;
                return NOERROR;
            }
        case IDateFormat::MEDIUM:
            if (DateFormat::sIs24Hour == nullptr) {
                *timeFormat = mMediumTimeFormat;
                return NOERROR;
            }
            else {
                *timeFormat = CoreUtils::Unbox(DateFormat::sIs24Hour) ?
                        mTimeFormat_Hms : mTimeFormat_hms;
                return NOERROR;
            }
        case IDateFormat::LONG:
            *timeFormat = mLongTimeFormat;
            return NOERROR;
        case IDateFormat::FULL:
            *timeFormat = mFullTimeFormat;
            return NOERROR;
    }
    return E_ASSERTION_ERROR;
}

ECode LocaleData::InitLocaleData(
    /* [in] */ ILocale* locale,
    /* [out] */ ILocaleData** localeData)
{
    AutoPtr<LocaleData> localeDataObj = new LocaleData();
    String languageTag;
    locale->ToLanguageTag(&languageTag);
    if (!ICU::InitLocaleData(languageTag, localeDataObj)) {
        Logger::E("LocaleData", "couldn't initialize LocaleData for locale %s", Object::ToString(locale).string());
        return E_ASSERTION_ERROR;
    }

    localeDataObj->mTimeFormat_hm = ICU::GetBestDateTimePattern(String("hm"), locale);
    localeDataObj->mTimeFormat_Hm = ICU::GetBestDateTimePattern(String("Hm"), locale);
    localeDataObj->mTimeFormat_hms = ICU::GetBestDateTimePattern(String("hms"), locale);
    localeDataObj->mTimeFormat_Hms = ICU::GetBestDateTimePattern(String("Hms"), locale);

    if (!localeDataObj->mFullTimeFormat.IsNull()) {
        // There are some full time format patterns in ICU that use the pattern character 'v'.
        // ccm doesn't accept this, so we replace it with 'z' which has about the same result
        // as 'v', the timezone name.
        // 'v' -> "PT", 'z' -> "PST", v is the generic timezone and z the standard tz
        // "vvvv" -> "Pacific Time", "zzzz" -> "Pacific Standard Time"
        localeDataObj->mFullTimeFormat = localeDataObj->mFullTimeFormat.Replace(U'v', U'z');
    }
    if (!localeDataObj->mNumberPattern.IsNull()) {
        // The number pattern might contain positive and negative subpatterns. Arabic, for
        // example, might look like "#,##0.###;#,##0.###-" because the minus sign should be
        // written last. Macedonian supposedly looks something like "#,##0.###;(#,##0.###)".
        // (The negative subpattern is optional, though, and not present in most locales.)
        // By only swallowing '#'es and ','s after the '.', we ensure that we don't
        // accidentally eat too much.
        StringUtils::ReplaceAll(localeDataObj->mNumberPattern, String("\\.[#,]*"),
                String(""), &localeDataObj->mIntegerPattern);
    }
    *localeData = (ILocaleData*)localeDataObj.Get();
    REFCOUNT_ADD(*localeData);
    return NOERROR;
}

ECode LocaleData::GetCurrencySymbol(
    /* [out] */ String* currencySymbol)
{
    VALIDATE_NOT_NULL(currencySymbol);

    *currencySymbol = mCurrencySymbol;
    return NOERROR;
}

ECode LocaleData::GetDecimalSeparator(
    /* [out] */ Char* decSeparator)
{
    VALIDATE_NOT_NULL(decSeparator);

    *decSeparator = mDecimalSeparator;
    return NOERROR;
}

ECode LocaleData::GetExponentSeparator(
    /* [out] */ String* expSeparator)
{
    VALIDATE_NOT_NULL(expSeparator);

    *expSeparator = mExponentSeparator;
    return NOERROR;
}

ECode LocaleData::GetGroupingSeparator(
    /* [out] */ Char* grpSeparator)
{
    VALIDATE_NOT_NULL(grpSeparator);

    *grpSeparator = mGroupingSeparator;
    return NOERROR;
}

ECode LocaleData::GetFirstDayOfWeek(
    /* [out] */ IInteger** day)
{
    VALIDATE_NOT_NULL(day);

    *day = mFirstDayOfWeek;
    REFCOUNT_ADD(*day);
    return NOERROR;
}

ECode LocaleData::GetInfinity(
    /* [out] */ String* infinity)
{
    VALIDATE_NOT_NULL(infinity);

    *infinity = mInfinity;
    return NOERROR;
}

ECode LocaleData::GetInternationalCurrencySymbol(
    /* [out] */ String* intlCurrencySymbol)
{
    VALIDATE_NOT_NULL(intlCurrencySymbol);

    *intlCurrencySymbol = mInternationalCurrencySymbol;
    return NOERROR;
}

ECode LocaleData::GetMinimalDaysInFirstWeek(
    /* [out] */ IInteger** days)
{
    VALIDATE_NOT_NULL(days);

    *days = mMinimalDaysInFirstWeek;
    REFCOUNT_ADD(*days);
    return NOERROR;
}

ECode LocaleData::GetMinusSign(
    /* [out] */ String* sign)
{
    VALIDATE_NOT_NULL(sign);

    *sign = mMinusSign;
    return NOERROR;
}

ECode LocaleData::GetNaN(
    /* [out] */ String* nan)
{
    VALIDATE_NOT_NULL(nan);

    *nan = mNaN;
    return NOERROR;
}

ECode LocaleData::GetPatternSeparator(
    /* [out] */ Char* patSeparator)
{
    VALIDATE_NOT_NULL(patSeparator);

    *patSeparator = mPatternSeparator;
    return NOERROR;
}

ECode LocaleData::GetPercent(
    /* [out] */ String* percent)
{
    VALIDATE_NOT_NULL(percent);

    *percent = mPercent;
    return NOERROR;
}

ECode LocaleData::GetPerMill(
    /* [out] */ Char* perMill)
{
    VALIDATE_NOT_NULL(perMill);

    *perMill = mPerMill;
    return NOERROR;
}

ECode LocaleData::GetZeroDigit(
    /* [out] */ Char* zeroDigit)
{
    VALIDATE_NOT_NULL(zeroDigit);

    *zeroDigit = mZeroDigit;
    return NOERROR;
}

}
}