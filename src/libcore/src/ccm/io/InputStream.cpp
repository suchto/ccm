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
#include "ccm/core/Math.h"
#include "ccm/io/InputStream.h"
#include <ccmlogger.h>

using ccm::core::AutoLock;
using ccm::core::IID_IAutoCloseable;
using ccm::core::Math;

namespace ccm {
namespace io {

CCM_INTERFACE_IMPL_3(InputStream, SyncObject, IInputStream, ICloseable, IAutoCloseable);

ECode InputStream::Read(
    /* [out] */ Array<Byte>& buffer,
    /* [out] */ Integer* number)
{
    return Read(buffer, 0, buffer.GetLength(), number);
}

ECode InputStream::Read(
    /* [out] */ Array<Byte>& buffer,
    /* [in] */ Integer offset,
    /* [in] */ Integer size,
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    if (offset < 0 || size < 0 || size > buffer.GetLength() - offset) {
        return ccm::core::E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    else if (size == 0) {
        *number = 0;
        return NOERROR;
    }

    Integer c;
    FAIL_RETURN(Read(&c));
    if (c == -1) {
        *number = -1;
        return NOERROR;
    }
    buffer[offset] = (Byte)c;

    Integer i = 1;
    for (; i < size; i++) {
        ECode ec = Read(&c);
        if (FAILED(ec) || c == -1) {
            break;
        }
        buffer[offset + i] = (Byte)c;
    }
    *number = i;
    return NOERROR;
}

ECode InputStream::Skip(
    /* [in] */ Long byteCount,
    /* [out] */ Long* number)
{
    VALIDATE_NOT_NULL(number);

    Long remaining = byteCount;
    Integer nr;

    if (byteCount <= 0) {
        *number = 0;
        return NOERROR;
    }

    Integer size = Math::Min((Long)MAX_SKIP_BUFFER_SIZE, remaining);
    Array<Byte> skipBuffer(size);
    while (remaining > 0) {
        FAIL_RETURN(Read(skipBuffer, 0, Math::Min((Long)size, remaining), &nr));
        if (nr < 0) {
            break;
        }
        remaining -= nr;
    }

    *number = byteCount - remaining;
    return NOERROR;
}

ECode InputStream::Available(
    /* [out] */ Integer* number)
{
    VALIDATE_NOT_NULL(number);

    *number = 0;
    return NOERROR;
}

ECode InputStream::Close()
{}

ECode InputStream::Mark(
    /* [in] */ Integer readLimit)
{
    AutoLock lock(this);
    return NOERROR;
}

ECode InputStream::Reset()
{
    AutoLock lock(this);
    Logger::E("InputStream", "reset not supported");
    return E_IO_EXCEPTION;
}

ECode InputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = false;
    return NOERROR;
}

}
}
