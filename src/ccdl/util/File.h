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

#ifndef __CCDL_FILE_H__
#define __CCDL_FILE_H__

#include "String.h"

#include <stdio.h>

namespace ccdl {

class File
{
private:
    class BufferedReader
    {
    public:
        BufferedReader(
            /* [in] */ File* owner)
            : mOwner(owner)
            , mColumnNo(0)
            , mPrevColumnNo(0)
            , mLineNo(1)
            , mPosition(-1)
            , mCapacity(-1)
            , mEof(false)
            , mError(false)
        {}

        int Peek();

        int Read();

        void Unread(
            /* [in] */ int c);

        inline int GetColumnNo()
        { return mColumnNo; }

        inline int GetLineNo()
        { return mLineNo; }

    private:
        int FRead();

    private:
        static constexpr int BUFFER_SIZE = 512;

        File* mOwner;
        int mColumnNo;
        int mPrevColumnNo;
        int mLineNo;
        char mBuffer[BUFFER_SIZE];
        int mPosition;
        int mCapacity;
        bool mEof;
        bool mError;
    };

public:
    File(
        /* [in] */ const char* path,
        /* [in] */ int mode);

    File(
        /* [in] */ const String& path,
        /* [in] */ int mode);

    ~File();

    inline bool IsValid()
    { return mFd != nullptr; }

    inline String GetPath()
    { return mPath; }

    inline int Peek()
    { return mReader.Peek(); }

    inline int Read()
    { return mReader.Read(); }

    inline void Unread(
        /* [in] */ int c)
    { mReader.Unread(c); }

    void Write(
        /* [in] */ const void* data,
        /* [in] */ size_t size);

    void Flush();

    void Close();

    inline int GetColumnNo()
    { return mReader.GetColumnNo(); }

    inline int GetLineNo()
    { return mReader.GetLineNo(); }

public:
    static constexpr int READ = 0x1;
    static constexpr int WRITE = 0x2;
    static constexpr int APPEND = 0x4;

private:
    String mPath;
    FILE* mFd;
    int mMode;
    BufferedReader mReader;
};

}

#endif //__CCDL_FILE_H__