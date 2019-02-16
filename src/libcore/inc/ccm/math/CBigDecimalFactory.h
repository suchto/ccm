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

#ifndef __CCM_MATH_CBIGDECIMALFACTORY_H__
#define __CCM_MATH_CBIGDECIMALFACTORY_H__

#include "ccm/math/CBigDecimal.h"
#include "ccm.math.IBigDecimalFactory.h"
#include "_ccm_math_CBigDecimalFactory.h"

namespace ccm {
namespace math {

class CBigDecimalFactory
    : public Object
    , public IBigDecimalFactory
{
public:
    CCM_INTERFACE_DECL();

    CCM_OBJECT_DECL();

    ECode GetONE(
        /* [out] */ IBigDecimal** value) override;

    ECode GetTEN(
        /* [out] */ IBigDecimal** value) override;

    ECode GetZERO(
        /* [out] */ IBigDecimal** value) override;

    ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [out] */ IBigDecimal** result) override;

    ECode ValueOf(
        /* [in] */ Long unscaledValue,
        /* [in] */ Integer scale,
        /* [out] */ IBigDecimal** result) override;

    ECode ValueOf(
        /* [in] */ Double value,
        /* [out] */ IBigDecimal** result) override;
};

}
}

#endif // __CCM_MATH_CBIGDECIMALFACTORY_H__
