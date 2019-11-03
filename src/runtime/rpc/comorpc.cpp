//=========================================================================
// Copyright (C) 2018 The C++ Component Model(COMO) Open Source Project
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

#include "comorpc.h"
#include "CProxy.h"
#include "CStub.h"
#include "dbus/CDBusChannelFactory.h"

namespace como {

static AutoPtr<IRPCChannelFactory> sLocalFactory = new CDBusChannelFactory(RPCType::Local);
static AutoPtr<IRPCChannelFactory> sRemoteFactory;

ECode CoCreateParcel(
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IParcel>& parcel)
{
    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    return factory->CreateParcel(parcel);
}

ECode CoCreateInterfacePack(
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IInterfacePack>& ipack)
{
    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    return factory->CreateInterfacePack(ipack);
}

ECode CoCreateProxy(
    /* [in] */ const CoclassID& cid,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IProxy>& proxy)
{
    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Proxy, channel);
    if (FAILED(ec)) {
        proxy = nullptr;
        return ec;
    }
    return CProxy::CreateObject(cid, channel, proxy);
}

ECode CoCreateStub(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IStub>& stub)
{
    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    AutoPtr<IRPCChannel> channel;
    ECode ec = factory->CreateChannel(RPCPeer::Stub, channel);
    if (FAILED(ec)) {
        stub = nullptr;
        return ec;
    }
    return CStub::CreateObject(object, channel, stub);
}

ECode CoMarshalInterface(
    /* [in] */ IInterface* object,
    /* [in] */ RPCType type,
    /* [out] */ AutoPtr<IInterfacePack>& ipack)
{
    if (object == nullptr) {
        ipack = nullptr;
        return NOERROR;
    }

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    return factory->MarshalInterface(object, ipack);
}

ECode CoUnmarshalInterface(
    /* [in] */ RPCType type,
    /* [in] */ IInterfacePack* data,
    /* [out] */ AutoPtr<IInterface>& object)
{
    if (data == nullptr) {
        object = nullptr;
        return NOERROR;
    }

    AutoPtr<IRPCChannelFactory> factory =
            type == RPCType::Local ? sLocalFactory : sRemoteFactory;
    return factory->UnmarshalInterface(data, object);
}

} // namespace como