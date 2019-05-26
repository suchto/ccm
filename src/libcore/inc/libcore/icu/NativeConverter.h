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

#ifndef __LIBCORE_ICU_NATIVECONVERTER_H__
#define __LIBCORE_ICU_NATIVECONVERTER_H__

#include "ccm.io.charset.ICharset.h"

using ccm::io::charset::ICharset;

namespace libcore {
namespace icu {

class NativeConverter
{
public:
    static ECode CharsetForName(
        /* [in] */ const String& charsetName,
        /* [out] */ ICharset** cs)
    {
        return NOERROR;
    }
};

}
}

#endif // __LIBCORE_ICU_NATIVECONVERTER_H__
