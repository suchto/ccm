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

#include "ccm/core/Character.h"
#include "ccm/core/CoreUtils.h"
#include "ccm/core/CStringBuilder.h"
#include "ccm/core/Math.h"
#include "ccm/core/StringUtils.h"
#include "ccm/core/System.h"
#include "ccm/io/CBufferedWriter.h"
#include "ccm/io/CFile.h"
#include "ccm/io/CFileOutputStream.h"
#include "ccm/io/COutputStreamWriter.h"
#include "ccm/io/charset/Charset.h"
#include "ccm/util/CArrayList.h"
#include "ccm/util/CFormatter.h"
#include "ccm/util/Formatter.h"
#include "ccm/util/Locale.h"
#include "ccm.core.IBoolean.h"
#include "ccm.core.IByte.h"
#include "ccm.core.IChar.h"
#include "ccm.core.IDouble.h"
#include "ccm.core.IFloat.h"
#include "ccm.core.IInteger.h"
#include "ccm.core.ILong.h"
#include "ccm.core.IShort.h"
#include "ccm.io.IFile.h"
#include "ccm.io.IOutputStream.h"
#include "ccm.io.IWriter.h"
#include "ccm.util.IFormattable.h"
#include <ccmlogger.h>

using ccm::core::Character;
using ccm::core::CoreUtils;
using ccm::core::CStringBuilder;
using ccm::core::IID_IAppendable;
using ccm::core::IID_IAutoCloseable;
using ccm::core::IID_IStringBuilder;
using ccm::core::IBoolean;
using ccm::core::IByte;
using ccm::core::IChar;
using ccm::core::IDouble;
using ccm::core::IFloat;
using ccm::core::IInteger;
using ccm::core::ILong;
using ccm::core::IShort;
using ccm::core::Math;
using ccm::core::StringUtils;
using ccm::core::System;
using ccm::io::CBufferedWriter;
using ccm::io::CFile;
using ccm::io::CFileOutputStream;
using ccm::io::COutputStreamWriter;
using ccm::io::IFile;
using ccm::io::IID_ICloseable;
using ccm::io::IID_IFile;
using ccm::io::IID_IFlushable;
using ccm::io::IID_IOutputStream;
using ccm::io::IID_IWriter;
using ccm::io::IOutputStream;
using ccm::io::IWriter;
using ccm::io::charset::Charset;

namespace ccm {
namespace util {

CCM_INTERFACE_IMPL_4(Formatter, SyncObject, IFormatter, ICloseable, IFlushable, IAutoCloseable);

ECode Formatter::ToCharset(
    /* [in] */ const String& csn,
    /* [out] */ ICharset** cs)
{
    if (csn.IsNull()) {
        Logger::E("Formatter", "charsetName is null");
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    ECode ec = Charset::ForName(csn, cs);
    return SUCCEEDED(ec) ? NOERROR : ccm::io::E_UNSUPPORTED_ENCODING_EXCEPTION;
}

AutoPtr<IAppendable> Formatter::NonNullAppendable(
    /* [in] */ IAppendable* a)
{
    if (a == nullptr) {
        AutoPtr<IAppendable> sb;
        CStringBuilder::New(IID_IAppendable, (IInterface**)&sb);
        return sb;
    }
    return a;
}

ECode Formatter::Constructor(
    /* [in] */ ILocale* l,
    /* [in] */ IAppendable* a)
{
    mA = a;
    mL = l;
    mZero = GetZero(l);
    return NOERROR;
}

ECode Formatter::Constructor(
    /* [in] */ ICharset* charset,
    /* [in] */ ILocale* l,
    /* [in] */ IFile* file)
{
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(file, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, charset, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor()
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IAppendable> a;
    CStringBuilder::New(IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IAppendable* a)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IAppendable> aa = NonNullAppendable(a);
    return Constructor(l, aa);
}

ECode Formatter::Constructor(
    /* [in] */ ILocale* l)
{
    AutoPtr<IAppendable> a;
    CStringBuilder::New(IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IAppendable* a,
    /* [in] */ ILocale* l)
{
    AutoPtr<IAppendable> aa = NonNullAppendable(a);
    return Constructor(l, aa);
}

ECode Formatter::Constructor(
    /* [in] */  const String& fileName)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(fileName, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(fileName, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ const String& fileName,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, &cs));
    AutoPtr<IFile> f;
    FAIL_RETURN(CFile::New(fileName, IID_IFile, (IInterface**)&f));
    return Constructor(cs, l, f);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IOutputStream> fos;
    FAIL_RETURN(CFileOutputStream::New(file, IID_IOutputStream, (IInterface**)&fos));
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(fos, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(file, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ IFile* file,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<ICharset> cs;
    FAIL_RETURN(ToCharset(csn, &cs));
    return Constructor(cs, l, file);
}

ECode Formatter::Constructor(
    /* [in] */ IPrintStream* ps)
{
    if (ps == nullptr) {
        return ccm::core::E_NULL_POINTER_EXCEPTION;
    }
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(l, IAppendable::Probe(ps));
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn)
{
    AutoPtr<ILocale> l = Locale::GetDefault(Locale::Category::GetFORMAT());
    return Constructor(os, csn, l);
}

ECode Formatter::Constructor(
    /* [in] */ IOutputStream* os,
    /* [in] */ const String& csn,
    /* [in] */ ILocale* l)
{
    AutoPtr<IWriter> osw;
    FAIL_RETURN(COutputStreamWriter::New(os, csn, IID_IWriter, (IInterface**)&osw));
    AutoPtr<IAppendable> a;
    CBufferedWriter::New(osw, IID_IAppendable, (IInterface**)&a);
    return Constructor(l, a);
}

Char Formatter::GetZero(
    /* [in] */ ILocale* l)
{
    // not implemented;
    return '0';
}

ECode Formatter::GetLocale(
    /* [out] */ ILocale** locale)
{
    VALIDATE_NOT_NULL(locale);

    FAIL_RETURN(EnsureOpen());
    *locale = mL;
    REFCOUNT_ADD(*locale);
    return NOERROR;
}

ECode Formatter::GetOut(
    /* [out] */ IAppendable** output)
{
    VALIDATE_NOT_NULL(output);

    FAIL_RETURN(EnsureOpen());
    *output = mA;
    REFCOUNT_ADD(*output);
    return NOERROR;
}

ECode Formatter::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    FAIL_RETURN(EnsureOpen());
    *str = Object::ToString(mA);
    return NOERROR;
}

ECode Formatter::Flush()
{
    FAIL_RETURN(EnsureOpen());
    if (IFlushable::Probe(mA) != nullptr) {
        ECode ec = IFlushable::Probe(mA)->Flush();
        if (FAILED(ec)) {
            mLastException = ec;
        }
    }
    return NOERROR;
}

ECode Formatter::Close()
{
    if (mA == nullptr) {
        return NOERROR;
    }
    if (ICloseable::Probe(mA) != nullptr) {
        ECode ec = ICloseable::Probe(mA)->Close();
        if (FAILED(ec)) {
            mLastException = ec;
        }
    }
    mA = nullptr;
    return NOERROR;
}

ECode Formatter::EnsureOpen()
{
    if (mA == nullptr) {
        return E_FORMATTER_CLOSED_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::GetIoException(
    /* [out] */ ECode* ec)
{
    VALIDATE_NOT_NULL(ec);

    *ec = mLastException;
    return NOERROR;
}

ECode Formatter::Format(
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    return Format(mL, format, args);
}

ECode Formatter::Format(
    /* [in] */ ILocale* l,
    /* [in] */ const String& format,
    /* [in] */ const Array<IInterface*>* args)
{
    FAIL_RETURN(EnsureOpen());

    // index of last argument referenced
    Integer last = -1;
    // last ordinary index
    Integer lasto = -1;

    Array<IFormatString*> fsa;
    FAIL_RETURN(Parse(format, &fsa));
    for (Integer i = 0; i < fsa.GetLength(); i++) {
        IFormatString* fs = fsa[i];
        Integer index;
        fs->GetIndex(&index);
        switch (index) {
            case -2: { // fixed string, "%n", or "%%"
                ECode ec = fs->Print(nullptr, l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            case -1: { // relative index
                if (last < 0 || (args != nullptr && last > args->GetLength() - 1)) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[last]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            case 0: { // ordinary index
                lasto++;
                last = lasto;
                if (args != nullptr && lasto > args->GetLength() - 1) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[lasto]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
            default: { // explicit index
                last = index - 1;
                if (args != nullptr && last > args->GetLength() - 1) {
                    return E_MISSING_FORMAT_ARGUMENT_EXCEPTION;
                }
                ECode ec = fs->Print((args == nullptr ? nullptr : (*args)[last]), l);
                if (FAILED(ec)) {
                    mLastException = ec;
                }
                break;
            }
        }
    }
    return NOERROR;
}

ECode Formatter::Parse(
    /* [in] */ const String& s,
    /* [out, callee] */ Array<IFormatString*>* formats)
{
    AutoPtr<IArrayList> al;
    CArrayList::New(IID_IArrayList, (IInterface**)&al);
    for (Integer i = 0, len = s.GetLength(); i < len;) {
        Integer nextPercent = s.IndexOf('%', i);
        if (s.GetChar(i) != '%') {
            // This is plain-text part, find the maximal plain-text
            // sequence and store it.
            Integer plainTextStart = i;
            Integer plainTextEnd = (nextPercent == -1) ? len : nextPercent;
            al->Add(new FixedString(this, s.Substring(plainTextStart,
                    plainTextEnd)));
            i = plainTextEnd;
        }
        else {
            // We have a format specifier
            AutoPtr<FormatSpecifierParser> fsp = new FormatSpecifierParser();
            FAIL_RETURN(fsp->Constructor(this, s, i + 1));
            al->Add(fsp->GetFormatSpecifier());
            i = fsp->GetEndIdx();
        }
    }
    return al->ToArray(IID_IFormatString, (Array<IInterface*>*)formats);
}

//-------------------------------------------------------------------------

AutoPtr<Formatter::Flags> Formatter::Flags::GetNONE()
{
    static AutoPtr<Flags> NONE = new Flags(0); // ''
    return NONE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetLEFT_JUSTIFY()
{
    static AutoPtr<Flags> LEFT_JUSTIFY = new Flags(1 << 0); // '-'
    return LEFT_JUSTIFY;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetUPPERCASE()
{
    static AutoPtr<Flags> UPPERCASE = new Flags(1 << 1); // '^'
    return UPPERCASE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetALTERNATE()
{
    static AutoPtr<Flags> ALTERNATE = new Flags(1 << 2); // '#'
    return ALTERNATE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPLUS()
{
    static AutoPtr<Flags> PLUS = new Flags(1 << 3); // '+'
    return PLUS;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetLEADING_SPACE()
{
    static AutoPtr<Flags> LEADING_SPACE = new Flags(1 << 4); // ' '
    return LEADING_SPACE;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetZERO_PAD()
{
    static AutoPtr<Flags> ZERO_PAD = new Flags(1 << 5); // '0'
    return ZERO_PAD;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetGROUP()
{
    static AutoPtr<Flags> GROUP = new Flags(1 << 6); // ','
    return GROUP;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPARENTHESES()
{
    static AutoPtr<Flags> PARENTHESES = new Flags(1 << 7); // '('
    return PARENTHESES;
}

AutoPtr<Formatter::Flags> Formatter::Flags::GetPREVIOUS()
{
    static AutoPtr<Flags> PREVIOUS = new Flags(1 << 8); // '<'
    return PREVIOUS;
}

ECode Formatter::Flags::Parse(
    /* [in] */ const String& s,
    /* [out] */ Flags** retf)
{
    VALIDATE_NOT_NULL(retf);

    Array<Char> ca = s.GetChars();
    AutoPtr<Flags> f = new Flags(0);
    for (Integer i = 0; i < ca.GetLength(); i++) {
        AutoPtr<Flags> v;
        FAIL_RETURN(Parse(ca[i], &v));
        if (f->Contains(v)) {
            return E_DUPLICATE_FORMAT_FLAGS_EXCEPTION;
        }
        f->Add(v);
    }
    *retf = f;
    REFCOUNT_ADD(*retf);
    return NOERROR;
}

ECode Formatter::Flags::Parse(
    /* [in] */ Char c,
    /* [out] */ Flags** retf)
{
    AutoPtr<Flags> f;
    switch (c) {
        case '-': f = GetLEFT_JUSTIFY(); break;
        case '#': f = GetALTERNATE(); break;
        case '+': f = GetPLUS(); break;
        case ' ': f = GetLEADING_SPACE(); break;
        case '0': f = GetZERO_PAD(); break;
        case ',': f = GetGROUP(); break;
        case '(': f = GetPARENTHESES(); break;
        case '<': f = GetPREVIOUS(); break;
        default:
            return E_UNKNOWN_FORMAT_FLAGS_EXCEPTION;
    }
    *retf = f;
    REFCOUNT_ADD(*retf);
    return NOERROR;
}

String Formatter::Flags::ToString()
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    if (Contains(GetLEFT_JUSTIFY()))  sb->AppendChar('-');
    if (Contains(GetUPPERCASE()))     sb->AppendChar('^');
    if (Contains(GetALTERNATE()))     sb->AppendChar('#');
    if (Contains(GetPLUS()))          sb->AppendChar('+');
    if (Contains(GetLEADING_SPACE())) sb->AppendChar(' ');
    if (Contains(GetZERO_PAD()))      sb->AppendChar('0');
    if (Contains(GetGROUP()))         sb->AppendChar(',');
    if (Contains(GetPARENTHESES()))   sb->AppendChar('(');
    if (Contains(GetPREVIOUS()))      sb->AppendChar('<');
    String str;
    sb->ToString(&str);
    return str;
}

//-------------------------------------------------------------------------

extern const InterfaceID Formatter::IID_IFormatString =
    {{0xd6dc3cb6,0x3ca2,0x4066,0xa773,{0xe,0x8,0x9,0xe,0x6,0x9,0xb,0x0,0x0,0xa,0x6,0xd}}, &CID_libcore};

CCM_INTERFACE_IMPL_LIGHT_1(Formatter::FixedString, IFormatString);

ECode Formatter::FixedString::GetIndex(
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    *idx = -2;
    return NOERROR;
}

ECode Formatter::FixedString::Print(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    mOwner->mA->Append(CoreUtils::Box(mS));
    return NOERROR;
}

ECode Formatter::FixedString::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    *str = mS;
    return NOERROR;
}

//-------------------------------------------------------------------------

CCM_INTERFACE_IMPL_LIGHT_1(Formatter::FormatSpecifier, IFormatString);

ECode Formatter::FormatSpecifier::Constructor(
    /* [in] */ Formatter* owner,
    /* [in] */ const String& indexStr,
    /* [in] */ const String& flagsStr,
    /* [in] */ const String& widthStr,
    /* [in] */ const String& precisionStr,
    /* [in] */ const String& tTStr,
    /* [in] */ const String& convStr)
{
    mOwner = owner;
    Integer idx = 1;

    Index(indexStr);
    ToFlags(flagsStr);
    Width(widthStr);
    Precision(precisionStr);

    if (!tTStr.IsNull()) {
        mDt = true;
        if (tTStr.Equals("T")) {
            mF->Add(Flags::GetUPPERCASE());
        }
    }

    Conversion(convStr);

    if (mDt) {
        FAIL_RETURN(CheckDateTime());
    }
    else if (Conversion::IsGeneral(mC)) {
        FAIL_RETURN(CheckGeneral())
    }
    else if (Conversion::IsCharacter(mC)) {
        FAIL_RETURN(CheckCharacter());
    }
    else if (Conversion::IsInteger(mC)) {
        FAIL_RETURN(CheckInteger());
    }
    else if (Conversion::IsFloat(mC)) {
        FAIL_RETURN(CheckFloat());
    }
    else if (Conversion::IsText(mC)) {
        FAIL_RETURN(CheckText());
    }
    else {
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
}

Integer Formatter::FormatSpecifier::Index(
    /* [in] */ const String& s)
{
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInt(s, &mIndex);
        CHECK(SUCCEEDED(ec));
    }
    else {
        mIndex = 0;
    }
    return mIndex;
}

ECode Formatter::FormatSpecifier::GetIndex(
    /* [out] */ Integer* idx)
{
    VALIDATE_NOT_NULL(idx);

    *idx = mIndex;
    return NOERROR;
}

ECode Formatter::FormatSpecifier::ToFlags(
    /* [in] */ const String& s,
    /* [out] */ Flags** f)
{
    mF = nullptr;
    FAIL_RETURN(Flags::Parse(s, &mF));
    if (mF->Contains(Flags::GetPREVIOUS())) {
        mIndex = -1;
    }
    *f = mF;
    REFCOUNT_ADD(*f);
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Width(
    /* [in] */ const String& s,
    /* [out] */ Integer* w)
{
    mWidth = -1;
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInt(s, &mWidth);
        CHECK(SUCCEEDED(ec));
        if (mWidth < 0) {
            return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
        }
    }
    *w = mWidth;
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Precision(
    /* [in] */ const String& s,
    /* [out] */ Integer* p)
{
    mPrecision = -1;
    if (!s.IsNull()) {
        ECode ec = StringUtils::ParseInt(s, &mPrecision);
        CHECK(SUCCEEDED(ec));
        if (mPrecision < 0) {
            return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
        }
    }
    *p = mPrecision;
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Conversion(
    /* [in] */ const String& s,
    /* [out] */ Char* c)
{
    mC = s.GetChar(0);
    if (!mDt) {
        if (!Conversion::IsValid(mC)) {
            return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
        }
        if (Character::IsUpperCase(mC)) {
            mF->Add(Flags::GetUPPERCASE());
        }
        mC = Character::ToLowerCase(mC);
        if (Conversion::IsText(mC)) {
            mIndex = -2;
        }
    }
    *c = mC;
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (mDt) {
        return PrintDateTime(arg, l);
    }
    switch (mC) {
        case Conversion::DECIMAL_INTEGER:
        case Conversion::OCTAL_INTEGER:
        case Conversion::HEXADECIMAL_INTEGER:
            return PrintInteger(arg, l);
        case Conversion::SCIENTIFIC:
        case Conversion::GENERAL:
        case Conversion::DECIMAL_FLOAT:
        case Conversion::HEXADECIMAL_FLOAT:
            return PrintFloat(arg, l);
        case Conversion::CHARACTER:
        case Conversion::CHARACTER_UPPER:
            return PrintCharacter(arg);
        case Conversion::BOOLEAN:
            return PrintBoolean(arg);
        case Conversion::STRING:
            return PrintString(arg, l);
        case Conversion::HASHCODE:
            return PrintHashCode(arg);
        case Conversion::LINE_SEPARATOR:
            return mOwner->mA->Append(CoreUtils::Box(System::GetLineSeparator()));
        case Conversion::PERCENT_SIGN:
            return mOwner->mA->Append('%');
        default:
            CHECK(0);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::PrintInteger(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    else if (IByte::Probe(arg) != nullptr) {
        Byte value;
        IByte::Probe(arg)->ByteValue(&value);
        return Print(value, l);
    }
    else if (IShort::Probe(arg) != nullptr) {
        Short value;
        IShort::Probe(arg)->ShortValue(&value);
        return Print(value, l);
    }
    else if (IInteger::Probe(arg) != nullptr) {
        Integer value;
        IInteger::Probe(arg)->IntValue(&value);
        return Print(value, l);
    }
    else if (ILong::Probe(arg) != nullptr) {
        Long value;
        ILong::Probe(arg)->LongValue(&value);
        return Print(value, l);
    }
    else if (IBigInteger::Probe(arg) != nullptr) {
        return Print(IBigInteger::Probe(arg), l);
    }
    else {
        return FailConversion();
    }
}

ECode Formatter::FormatSpecifier::PrintFloat(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    else if (IFloat::Probe(arg) != nullptr) {
        Float value;
        IFloat::Probe(arg)->FloatValue(&value);
        return Print(value, l);
    }
    else if (IDouble::Probe(arg) != nullptr) {
        Double value;
        IDouble::Probe(arg)->DoubleValue(&value);
        return Print(value, l);
    }
    else if (IBigDecimal::Probe(arg) != nullptr) {
        return Print(IBigDecimal::Probe(arg), l);
    }
    else {
        return FailConversion();
    }
}

ECode Formatter::FormatSpecifier::PrintDateTime(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }

    return NOERROR;
}

ECode Formatter::FormatSpecifier::PrintCharacter(
    /* [in] */ IInterface* arg)
{
    if (arg == nullptr) {
        return Print(String("null"));
    }
    String s;
    if (IChar::Probe(arg) != nullptr) {
        Char c;
        IChar::Probe(arg)->GetValue(&c);
        s = String::ValueOfChar(c);
    }
    else if (IByte::Probe(arg) != nullptr) {
        Byte i;
        IByte::Probe(arg)->ByteValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOfChar((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else if (IShort::Probe(arg) != nullptr) {
        Short i;
        IShort::Probe(arg)->ShortValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOfChar((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else if (IInteger::Probe(arg) != nullptr) {
        Integer i;
        IInteger::Probe(arg)->IntValue(&i);
        if (Character::IsValidCodePoint(i)) {
            s = String::ValueOfChar((Char)i);
        }
        else {
            return E_ILLEGAL_FORMAT_CODE_POINT_EXCEPTION;
        }
    }
    else {
        return FailConversion();
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::PrintString(
    /* [in] */ IInterface* arg,
    /* [in] */ ILocale* l)
{
    if (IFormattable::Probe(arg) != nullptr) {
        AutoPtr<IFormatter> fmt = mOwner;
        AutoPtr<ILocale> ll;
        FAIL_RETURN(fmt->GetLocale(&ll));
        if (ll != l) {
            AutoPtr<IAppendable> out;
            fmt->GetOut(&out);
            CFormatter::New(out, l, IID_IFormatter, (IInterface**)&fmt);
        }
        IFormattable::Probe(arg)->FormatTo(fmt, mF->ValueOf(), mWidth, mPrecision);
    }
    else {
        if (mF->Contains(Flags::GetALTERNATE())) {
            return FailMismatch();
        }
        if (arg == nullptr) {
            return Print(String("null"));
        }
        else {
            return Print(Object::ToString(arg));
        }
    }
}

ECode Formatter::FormatSpecifier::PrintBoolean(
    /* [in] */ IInterface* arg)
{
    String s;
    if (arg != nullptr) {
        s = (IBoolean::Probe(arg) != nullptr) ?
                Object::ToString(arg) :
                StringUtils::ToString(true);
    }
    else {
        s = StringUtils::ToString(false);
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::PrintHashCode(
    /* [in] */ IInterface* arg)
{
    String s;
    if (arg == nullptr) {
        s = "null";
    }
    else {
        s = StringUtils::ToString(Object::GetHashCode(arg), 16);
    }
    return Print(s);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ const String& s_)
{
    String s = s_;
    if (mPrecision != -1 && mPrecision < s.GetLength()) {
        s = s.Substring(0, mPrecision);
    }
    if (mF->Contains(Flags::GetUPPERCASE())) {
        s = s.ToUpperCase();
    }
    return mOwner->mA->Append(CoreUtils::Box(Justify(s)));
}

String Formatter::FormatSpecifier::Justify(
    /* [in] */ const String& s)
{
    if (mWidth == -1) {
        return s;
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Boolean pad = mF->Contains(Flags::GetLEFT_JUSTIFY());
    Integer sp = mWidth - s.GetLength();
    if (!pad) {
        for (Integer i = 0; i < sp; i++) {
            sb->AppendChar(' ');
        }
    }
    sb->Append(s);
    if (pad) {
        for (Integer i = 0; i < sp; i++) {
            sb->AppendChar(' ');
        }
    }
    String str;
    sb->ToString(&str);
    return str;
}

ECode Formatter::FormatSpecifier::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(String("%"), IID_IStringBuilder, (IInterface**)&sb);
    AutoPtr<Flags> dupf = mF->Dup();
    dupf->Remove(Flags::GetUPPERCASE());
    sb->Append(dupf->ToString());
    if (mIndex > 0) {
        sb->Append(mIndex);
        sb->AppendChar('$');
    }
    if (mWidth != -1) {
        sb->Append(mWidth);
    }
    if (mPrecision != -1) {
        sb->AppendChar('.');
        sb->Append(mPrecision);
    }
    if (mDt) {
        sb->AppendChar(mF->Contains(Flags::GetUPPERCASE()) ? 'T' : 't');
    }
    sb->AppendChar(mF->Contains(Flags::GetUPPERCASE()) ?
            Character::ToUpperCase(mC) : mC);
    return sb->ToString(str);
}

ECode Formatter::FormatSpecifier::CheckGeneral()
{
    if ((mC == Conversion::BOOLEAN || mC == Conversion::HASHCODE) &&
            mF->Contains(Flags::GetALTERNATE())) {
        return FailMismatch();
    }
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    Array<Flags*> badFlags(5);
    badFlags.Set(0, Flags::GetPLUS());
    badFlags.Set(1, Flags::GetLEADING_SPACE());
    badFlags.Set(2, Flags::GetZERO_PAD());
    badFlags.Set(3, Flags::GetGROUP());
    badFlags.Set(4, Flags::GetPARENTHESES());
    return CheckBadFlags(badFlags);
}

ECode Formatter::FormatSpecifier::CheckDateTime()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    if (!DateTime::IsValid(mC)) {
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    Array<Flags*> badFlags(6);
    badFlags.Set(0, Flags::GetALTERNATE());
    badFlags.Set(1, Flags::GetPLUS());
    badFlags.Set(2, Flags::GetLEADING_SPACE());
    badFlags.Set(3, Flags::GetZERO_PAD());
    badFlags.Set(4, Flags::GetGROUP());
    badFlags.Set(5, Flags::GetPARENTHESES());
    FAIL_RETURN(CheckBadFlags(badFlags));
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckCharacter()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    Array<Flags*> badFlags(6);
    badFlags.Set(0, Flags::GetALTERNATE());
    badFlags.Set(1, Flags::GetPLUS());
    badFlags.Set(2, Flags::GetLEADING_SPACE());
    badFlags.Set(3, Flags::GetZERO_PAD());
    badFlags.Set(4, Flags::GetGROUP());
    badFlags.Set(5, Flags::GetPARENTHESES());
    FAIL_RETURN(CheckBadFlags(badFlags));
    // '-' requires a width
    if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckInteger()
{
    FAIL_RETURN(CheckNumeric());
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }

    if (mC == Conversion::DECIMAL_INTEGER) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetALTERNATE());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::OCTAL_INTEGER) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
}

ECode Formatter::FormatSpecifier::CheckBadFlags(
    /* [in] */ const Array<Flags*>& badFlags)
{
    for (Integer i = 0; i < badFlags.GetLength(); i++) {
        if (mF->Contains(badFlags[i])) {
            return FailMismatch();
        }
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckFloat()
{
    FAIL_RETURN(CheckNumeric());
    if (mC == Conversion::DECIMAL_FLOAT) {
    }
    else if (mC == Conversion::HEXADECIMAL_FLOAT) {
        Array<Flags*> badFlags(2);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::SCIENTIFIC) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetGROUP());
        return CheckBadFlags(badFlags);
    }
    else if (mC == Conversion::GENERAL) {
        Array<Flags*> badFlags(1);
        badFlags.Set(0, Flags::GetALTERNATE());
        return CheckBadFlags(badFlags);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckNumeric()
{
    if (mWidth != -1 && mWidth < 0) {
        return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
    }

    if (mPrecision != -1 && mPrecision < 0) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }

    // '-' and '0' require a width
    if (mWidth == -1 && (mF->Contains(Flags::GetLEFT_JUSTIFY()) ||
            mF->Contains(Flags::GetZERO_PAD()))) {
        return E_MISSING_FORMAT_WIDTH_EXCEPTION;
    }

    // bad combination
    if ((mF->Contains(Flags::GetPLUS()) && mF->Contains(Flags::GetLEADING_SPACE())) ||
            (mF->Contains(Flags::GetLEFT_JUSTIFY()) && mF->Contains(Flags::GetZERO_PAD()))) {
        return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::CheckText()
{
    if (mPrecision != -1) {
        return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
    }
    switch (mC) {
        case Conversion::PERCENT_SIGN:
            if (mF->ValueOf() != Flags::GetLEFT_JUSTIFY()->ValueOf() &&
                    mF->ValueOf() != Flags::GetNONE()->ValueOf()) {
                return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
            }
            // '-' requires a width
            if (mWidth == -1 && mF->Contains(Flags::GetLEFT_JUSTIFY())) {
                return E_MISSING_FORMAT_WIDTH_EXCEPTION;
            }
            break;
        case Conversion::LINE_SEPARATOR:
            if (mWidth != -1) {
                return E_ILLEGAL_FORMAT_WIDTH_EXCEPTION;
            }
            if (mF->ValueOf() != Flags::GetNONE()->ValueOf()) {
                return E_ILLEGAL_FORMAT_FLAGS_EXCEPTION;
            }
            break;
        default:
            CHECK(0);
    }
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Byte value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 8);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Short value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 16);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Integer value,
    /* [in] */ ILocale* l)
{
    Long v = value;
    if (value < 0 &&
            (mC == Conversion::OCTAL_INTEGER ||
             mC == Conversion::HEXADECIMAL_INTEGER)) {
        v += (1ll << 32);
    }
    return Print(v, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Long value,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);

    if (mC == Conversion::DECIMAL_INTEGER) {
        Boolean neg = value < 0;
        Array<Char> va;
        if (value < 0) {
            va = StringUtils::ToString(value, 10).Substring(1).GetChars();
        }
        else {
            va = StringUtils::ToString(value, 10).GetChars();
        }

        // leading sign indicator
        LeadingSign(sb, neg);

        // the value
        LocalizedMagnitude(sb, va, mF, AdjustWidth(mWidth, mF, neg), l);

        // trailing sign indicator
        TrailingSign(sb, neg);
    }
    else if (mC == Conversion::OCTAL_INTEGER) {
        Array<Flags*> badFlags(3);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetLEADING_SPACE());
        badFlags.Set(2, Flags::GetPLUS());
        FAIL_RETURN(CheckBadFlags(badFlags));
        String s = StringUtils::ToOctalString(value);
        Integer len = mF->Contains(Flags::GetALTERNATE()) ?
                s.GetLength() + 1 : s.GetLength();

        // apply ALTERNATE (radix indicator for octal) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            sb->AppendChar('0');
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->AppendChar('0');
            }
        }
        sb->Append(s);
    }
    else if (mC == Conversion::HEXADECIMAL_INTEGER) {
        Array<Flags*> badFlags(3);
        badFlags.Set(0, Flags::GetPARENTHESES());
        badFlags.Set(1, Flags::GetLEADING_SPACE());
        badFlags.Set(2, Flags::GetPLUS());
        FAIL_RETURN(CheckBadFlags(badFlags));
        String s = StringUtils::ToHexString(value);
        Integer len = mF->Contains(Flags::GetALTERNATE()) ?
                s.GetLength() + 2 : s.GetLength();

        // apply ALTERNATE (radix indicator for hex) before ZERO_PAD
        if (mF->Contains(Flags::GetALTERNATE())) {
            sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? String("0X") : String("0x"));
        }
        if (mF->Contains(Flags::GetZERO_PAD())) {
            for (Integer i = 0; i < mWidth - len; i++) {
                sb->AppendChar('0');
            }
        }
        if (mF->Contains(Flags::GetUPPERCASE())) {
            s = s.ToUpperCase();
        }
        sb->Append(s);
    }

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

void Formatter::FormatSpecifier::LeadingSign(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean neg)
{
    if (!neg) {
        if (mF->Contains(Flags::GetPLUS())) {
            sb->AppendChar('+');
        }
        else if (mF->Contains(Flags::GetLEADING_SPACE())) {
            sb->AppendChar(' ');
        }
    }
    else {
        if (mF->Contains(Flags::GetPARENTHESES())) {
            sb->AppendChar('(');
        }
        else {
            sb->AppendChar('-');
        }
    }
}

void Formatter::FormatSpecifier::TrailingSign(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Boolean neg)
{
    if (neg && mF->Contains(Flags::GetPARENTHESES())) {
        sb->AppendChar(')');
    }
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IBigInteger* value,
    /* [in] */ ILocale* l)
{
    return NOERROR;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Float value,
    /* [in] */ ILocale* l)
{
    return Print((Double)value, l);
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ Double value,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Boolean neg = Math::Compare(value, 0.0) == -1;

    if (!Math::IsNaN(value)) {
        Double v = Math::Abs(value);

        // leading sign indicator
        LeadingSign(sb, neg);

        // the value
        if (!Math::IsInfinite(v)) {
            FAIL_RETURN(Print(sb, v, l, mF, mC, mPrecision, neg));
        }
        else {
            sb->Append(mF->Contains(Flags::GetUPPERCASE()) ?
                    String("INFINITY") : String("Infinity"));
        }

        // trailing sign indicator
        TrailingSign(sb, neg);
    }
    else {
        sb->Append(mF->Contains(Flags::GetUPPERCASE()) ? String("NAN") : String("NaN"));
    }

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Double value,
    /* [in] */ ILocale* l,
    /* [in] */ Flags* f,
    /* [in] */ Char c,
    /* [in] */ Integer precision,
    /* [in] */ Boolean neg)
{
    return NOERROR;
}

Array<Char> Formatter::FormatSpecifier::AddZeros(
    /* [in] */ const Array<Char>& v,
    /* [in] */ Integer prec)
{
    // Look for the dot.  If we don't find one, the we'll need to add
    // it before we add the zeros.
    Integer i;
    for (i = 0; i < v.GetLength(); i++) {
        if (v[i] == '.') {
            break;
        }
    }
    Boolean needDot = false;
    if (i == v.GetLength()) {
        needDot = true;
    }

    // Determine existing precision.
    Integer outPrec = v.GetLength() - i - (needDot ? 0 : 1);
    CHECK(outPrec <= prec);
    if (outPrec == prec) {
        return v;
    }

    // Create new array with existing contents.
    Array<Char> tmp(v.GetLength() + prec - outPrec + (needDot ? 1 : 0));
    tmp.Copy(v, v.GetLength());

    // Add dot if previously determined to be necessary.
    Integer start = v.GetLength();
    if (needDot) {
        tmp[v.GetLength()] = '.';
        start++;
    }

    // Add zeros.
    for (Integer j = start; j < tmp.GetLength(); j++) {
        tmp[j] = '0';
    }

    return tmp;
}

String Formatter::FormatSpecifier::HexDouble(
    /* [in] */ Double d,
    /* [in] */ Integer prec)
{
    return String("");
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IBigDecimal* value,
    /* [in] */ ILocale* l)
{
    if (mC == Conversion::HEXADECIMAL_FLOAT) {
        return FailConversion();
    }
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    Integer sign;
    value->Signum(&sign);
    Boolean neg = sign == -1;
    AutoPtr<IBigDecimal> v;
    value->Abs(&v);
    // leading sign indicator
    LeadingSign(sb, neg);

    // the value
    FAIL_RETURN(Print(sb, v, l, mF, mC, mPrecision, neg));

    // trailing sign indicator
    TrailingSign(sb, neg);

    // justify based on width
    String str;
    sb->ToString(&str);
    return mOwner->mA->Append(CoreUtils::Box(Justify(str)));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ IBigDecimal* value,
    /* [in] */ ILocale* l,
    /* [in] */ Flags* f,
    /* [in] */ Char c,
    /* [in] */ Integer precision,
    /* [in] */ Boolean neg)
{
    return NOERROR;
}

Integer Formatter::FormatSpecifier::AdjustWidth(
    /* [in] */ Integer width,
    /* [in] */ Flags* f,
    /* [in] */ Boolean neg)
{
    Integer newW = mWidth;
    if (newW != -1 && neg && mF->Contains(Flags::GetPARENTHESES())) {
        newW--;
    }
    return newW;
}

Array<Char> Formatter::FormatSpecifier::AddDot(
    /* [in] */ const Array<Char>& mant)
{
    Array<Char> tmp(mant.GetLength() + 1);
    tmp.Copy(mant, mant.GetLength());
    tmp[tmp.GetLength() - 1] = '.';
    return tmp;
}

Array<Char> Formatter::FormatSpecifier::TrailingZeros(
    /* [in] */ const Array<Char>& mant,
    /* [in] */ Integer nzeros)
{
    Array<Char> tmp = mant;
    if (nzeros > 0) {
        tmp = Array<Char>(mant.GetLength() + nzeros);
        tmp.Copy(mant, mant.GetLength());
        for (Integer i = mant.GetLength(); i < tmp.GetLength(); i++) {
            tmp[i] = '0';
        }
    }
    return tmp;
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ ICalendar* t,
    /* [in] */ Char c,
    /* [in] */ ILocale* l)
{
    AutoPtr<IStringBuilder> sb;
    CStringBuilder::New(IID_IStringBuilder, (IInterface**)&sb);
    FAIL_RETURN(Print(sb, t, c, l));

    // justify based on width
    String str;
    sb->ToString(&str);
    String s = Justify(str);
    if (mF->Contains(Flags::GetUPPERCASE())) {
        s = s.ToUpperCase();
    }

    return mOwner->mA->Append(CoreUtils::Box(s));
}

ECode Formatter::FormatSpecifier::Print(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ ICalendar* t,
    /* [in] */ Char c,
    /* [in] */ ILocale* l)
{
    return NOERROR;
}

Char Formatter::FormatSpecifier::GetZero(
    /* [in] */ ILocale* l)
{
    return mOwner->mZero;
}

void Formatter::FormatSpecifier::LocalizedMagnitude(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ Long value,
    /* [in] */ Flags* f,
    /* [in] */ Integer width,
    /* [in] */ ILocale* l)
{
    Array<Char> va = StringUtils::ToString(value, 10).GetChars();
    LocalizedMagnitude(sb, va, f, width, l);
}

void Formatter::FormatSpecifier::LocalizedMagnitude(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const Array<Char>& value,
    /* [in] */ Flags* f,
    /* [in] */ Integer width,
    /* [in] */ ILocale* l)
{}

//-------------------------------------------------------------------------

String Formatter::FormatSpecifierParser::GetFLAGS()
{
    static const String FLAGS(",-(+# 0<");
    return FLAGS;
}

ECode Formatter::FormatSpecifierParser::Constructor(
    /* [in] */ Formatter* owner,
    /* [in] */ const String& format,
    /* [in] */ Integer startIdx)
{
    mFormat = format;
    mCursor = startIdx;
    // Index
    if (NextIsInt()) {
        String nint = NextInt();
        Char c;
        FAIL_RETURN(Peek(&c));
        if (c == '$') {
            mIndex = nint;
            Advance();
        }
        else if (nint.GetChar(0) == '0') {
            // This is a flag, skip to parsing flags.
            Back(nint.GetLength());
        }
        else {
            // This is the width, skip to parsing precision.
            mWidth = nint;
        }
    }
    // Flags
    mFlags = "";
    if (mWidth.IsNull()) {
        Char c;
        FAIL_RETURN(Peek(&c));
        while (GetFLAGS().IndexOf(c) >= 0) {
            Advance(&c);
            mFlags = mFlags + c;
            FAIL_RETURN(Peek(&c));
        }
    }
    // Width
    if (mWidth.IsNull() && NextIsInt()) {
        mWidth = NextInt();
    }
    // Precision
    Char c;
    FAIL_RETURN(Peek(&c));
    if (c == '.') {
        Advance();
        if (!NextIsInt()) {
            return E_ILLEGAL_FORMAT_PRECISION_EXCEPTION;
        }
        mPrecision = NextInt();
    }
    FAIL_RETURN(Peek(&c));
    if (c == 't' || c == 'T') {
        Advance(&c);
        mTT = String::Format("%c", c);
    }
    // Conversion
    FAIL_RETURN(Advance(&c));
    mConv = String::Format("%c", c);

    mFs = new FormatSpecifier();
    return mFs->Constructor(owner, mIndex, mFlags, mWidth, mPrecision, mTT, mConv);
}

String Formatter::FormatSpecifierParser::NextInt()
{
    Integer strBegin = mCursor;
    while (NextIsInt()) {
        Advance();
    }
    return mFormat.Substring(strBegin, mCursor);
}

Boolean Formatter::FormatSpecifierParser::NextIsInt()
{
    Char c;
    ECode ec = Peek(&c);
    if (FAILED(ec)) return false;
    return !IsEnd() && Character::IsDigit(c);
}

ECode Formatter::FormatSpecifierParser::Peek(
    /* [out] */ Char* c)
{
    if (IsEnd()) {
        Logger::E("Formatter", "End of String");
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    *c = mFormat.GetChar(mCursor);
    return NOERROR;
}

ECode Formatter::FormatSpecifierParser::Advance(
    /* [out] */ Char* c)
{
    if (IsEnd()) {
        Logger::E("Formatter", "End of String");
        return E_UNKNOWN_FORMAT_CONVERSION_EXCEPTION;
    }
    *c = mFormat.GetChar(mCursor++);
    return NOERROR;
}

//-------------------------------------------------------------------------

Boolean Formatter::Conversion::IsValid(
    /* [in] */ Char c)
{
    return (IsGeneral(c) || IsInteger(c) || IsFloat(c) || IsText(c)
            || c == 't' || IsCharacter(c));
}

Boolean Formatter::Conversion::IsGeneral(
    /* [in] */ Char c)
{
    switch (c) {
        case BOOLEAN:
        case BOOLEAN_UPPER:
        case STRING:
        case STRING_UPPER:
        case HASHCODE:
        case HASHCODE_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsCharacter(
    /* [in] */ Char c)
{
    switch (c) {
        case CHARACTER:
        case CHARACTER_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsInteger(
    /* [in] */ Char c)
{
    switch (c) {
        case DECIMAL_INTEGER:
        case OCTAL_INTEGER:
        case HEXADECIMAL_INTEGER:
        case HEXADECIMAL_INTEGER_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsFloat(
    /* [in] */ Char c)
{
    switch (c) {
        case SCIENTIFIC:
        case SCIENTIFIC_UPPER:
        case GENERAL:
        case GENERAL_UPPER:
        case DECIMAL_FLOAT:
        case HEXADECIMAL_FLOAT:
        case HEXADECIMAL_FLOAT_UPPER:
            return true;
        default:
            return false;
    }
}

Boolean Formatter::Conversion::IsText(
    /* [in] */ Char c)
{
    switch (c) {
        case LINE_SEPARATOR:
        case PERCENT_SIGN:
            return true;
        default:
            return false;
    }
}

//-------------------------------------------------------------------------

Boolean Formatter::DateTime::IsValid(
    /* [in] */ Char c)
{
    switch (c) {
        case HOUR_OF_DAY_0:
        case HOUR_0:
        case HOUR_OF_DAY:
        case HOUR:
        case MINUTE:
        case NANOSECOND:
        case MILLISECOND:
        case MILLISECOND_SINCE_EPOCH:
        case AM_PM:
        case SECONDS_SINCE_EPOCH:
        case SECOND:
        case TIME:
        case ZONE_NUMERIC:
        case ZONE:

        // Date
        case NAME_OF_DAY_ABBREV:
        case NAME_OF_DAY:
        case NAME_OF_MONTH_ABBREV:
        case NAME_OF_MONTH:
        case CENTURY:
        case DAY_OF_MONTH_0:
        case DAY_OF_MONTH:
        case NAME_OF_MONTH_ABBREV_X:
        case DAY_OF_YEAR:
        case MONTH:
        case YEAR_2:
        case YEAR_4:

        // Composites
        case TIME_12_HOUR:
        case TIME_24_HOUR:
        case DATE_TIME:
        case DATE:
        case ISO_STANDARD_DATE:
            return true;
        default:
            return false;
    }
}

}
}
