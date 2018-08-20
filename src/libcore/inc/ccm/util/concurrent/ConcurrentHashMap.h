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

#ifndef __CCM_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__
#define __CCM_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__

#include "coredef.h"
#include "ccm/core/SyncObject.h"
#include "ccm.io.ISerializable.h"
#include "ccm.util.ICollection.h"
#include "ccm.util.IMap.h"
#include "ccm.util.IMapEntry.h"
#include "ccm.util.ISet.h"
#include "ccm.util.concurrent.IConcurrentHashMap.h"
#include <ccmrefbase.h>

using ccm::core::SyncObject;
using ccm::io::ISerializable;

namespace ccm {
namespace util {
namespace concurrent {

class ConcurrentHashMap
    : public SyncObject
    , public IConcurrentHashMap
    , public IMap
    , public ISerializable
{
public:
    class Node
        : public SyncObject
        , public IMapEntry
    {
    public:
        Node(
            /* [in] */ Integer hash,
            /* [in] */ IInterface* key,
            /* [in] */ IInterface* val,
            /* [in] */ Node* next);

        CCM_INTERFACE_DECL();

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        AutoPtr<Node> Find(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k);



        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode GetKey(
            /* [out] */ IInterface** key) override;

        ECode GetValue(
            /* [out] */ IInterface** value) override;

        ECode SetValue(
            /* [in] */ IInterface* value,
            /* [out] */ IInterface** prevValue = nullptr) override;

    public:
        Integer mHash;
        AutoPtr<IInterface> mKey;
        VOLATILE AutoPtr<IInterface> mVal;
        VOLATILE AutoPtr<Node> mNext;
    };

    class ReservationNode
        : public Node
    {

    };

    class TreeNode
        : public Node
    {
    public:
        AutoPtr<TreeNode> FindTreeNode(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k,
            /* [in] */ Boolean compare);
    };

    class TreeBin
        : public Node
    {
    public:
        inline static TreeBin* From(
            /* [in] */ Node* node)
        {
            return (TreeBin*)node;
        }

        AutoPtr<TreeNode> PutTreeVal(
            /* [in] */ Integer h,
            /* [in] */ IInterface* k,
            /* [in] */ IInterface* v);

        Boolean RemoveTreeNode(
            /* [in] */ TreeNode* p);

    public:
        AutoPtr<TreeNode> mRoot;
        VOLATILE AutoPtr<TreeNode> mFirst;
    };

    class Traverser
        : public LightRefBase
    {
    public:
        Traverser(
            /* [in] */ Array<Node*>& tab,
            /* [in] */ Integer size,
            /* [in] */ Integer index,
            /* [in] */ Integer limit);

        AutoPtr<Node> Advance();
    };

    class CollectionView
        : public SyncObject
        , public ICollection
        , public ISerializable
    {
    public:
        CCM_INTERFACE_DECL();

        ECode Clear() override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* empty) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToString(
            /* [out] */ String* desc) override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;
    };

    class KeySetView
        : public CollectionView
        , public ISet
    {
    public:
        KeySetView(
            /* [in] */ ConcurrentHashMap* map,
            /* [in] */ IInterface* value);

        CCM_INTERFACE_DECL();

        ECode Contains(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Remove(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetIterator(
            /* [out] */ IIterator** it) override;

        ECode Add(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode AddAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode GetHashCode(
            /* [out] */ Integer* hash) override;

        ECode Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* result) override;

        ECode Clear() override;

        ECode ContainsAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* result) override;

        ECode GetSize(
            /* [out] */ Integer* size) override;

        ECode IsEmpty(
            /* [out] */ Boolean* empty) override;

        ECode RemoveAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode RetainAll(
            /* [in] */ ICollection* c,
            /* [out] */ Boolean* changed = nullptr) override;

        ECode ToArray(
            /* [out, callee] */ Array<IInterface*>* objs) override;

        ECode ToArray(
            /* [in] */ const InterfaceID& iid,
            /* [out, callee] */ Array<IInterface*>* objs) override;
    };


public:
    CCM_INTERFACE_DECL();

    static Integer Spread(
        /* [in] */ Integer h);

    static AutoPtr<Node> TabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i);

    static Boolean CasTabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i,
        /* [in] */ Node* c,
        /* [in] */ Node* v);

    static void SetTabAt(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer i,
        /* [in] */ Node* v);

    ECode Constructor();

    ECode Constructor(
        /* [in] */ Integer initialCapacity);

    ECode Constructor(
        /* [in] */ IMap* m);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor);

    ECode Constructor(
        /* [in] */ Integer initialCapacity,
        /* [in] */ Float loadFactor,
        /* [in] */ Integer concurrencyLevel);

    ECode GetSize(
        /* [out] */ Integer* size) override;

    ECode IsEmpty(
        /* [out] */ Boolean* result) override;

    ECode Get(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** value) override;

    ECode ContainsKey(
        /* [in] */ IInterface* key,
        /* [out] */ Boolean* result) override;

    ECode ContainsValue(
        /* [in] */ IInterface* value,
        /* [out] */ Boolean* result) override;

    ECode Put(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode PutVal(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ Boolean onlyIfAbsent,
        /* [out] */ IInterface** prevValue = nullptr);

    ECode PutAll(
        /* [in] */ IMap* m) override;

    ECode Remove(
        /* [in] */ IInterface* key,
        /* [out] */ IInterface** prevValue = nullptr) override;

    ECode ReplaceNode(
        /* [in] */ IInterface* key,
        /* [in] */ IInterface* value,
        /* [in] */ IInterface* cv,
        /* [out] */ IInterface** prevValue = nullptr);

    ECode Clear() override;

    ECode GetKeySet(
        /* [out] */ ISet** keys) override;

    ECode GetValues(
        /* [out] */ ICollection** values) override;




    Long SumCount();

    Array<Node*> HelpTransfer(
        /* [in] */ Array<Node*> tab,
        /* [in] */ Node* f);

    AutoPtr<Node> Untreeify(
        /* [in] */ Node* b);

private:
    static Integer TableSizeFor(
        /* [in] */ Integer c);

    Array<Node*> InitTable();

    void TryPresize(
        /* [in] */ Integer size);

    void AddCount(
        /* [in] */ Long x,
        /* [in] */ Integer check);

    void TreeifyBin(
        /* [in] */ Array<Node*>& tab,
        /* [in] */ Integer index);

public:
    /**
     * The bin count threshold for using a tree rather than list for a
     * bin.  Bins are converted to trees when adding an element to a
     * bin with at least this many nodes. The value must be greater
     * than 2, and should be at least 8 to mesh with assumptions in
     * tree removal about conversion back to plain bins upon
     * shrinkage.
     */
    static constexpr Integer TREEIFY_THRESHOLD = 8;

    static constexpr Integer MOVED = -1; // hash for forwarding nodes
    static constexpr Integer HASH_BITS = 0x7fffffff; // usable bits of normal node hash

private:
    /**
     * The largest possible table capacity.  This value must be
     * exactly 1<<30, and is further required
     * because the top two bits of 32bit hash fields are used for
     * control purposes.
     */
    static constexpr Integer MAXIMUM_CAPACITY = 1 << 30;

    /**
     * The default initial table capacity.  Must be a power of 2
     * (i.e., at least 1) and at most MAXIMUM_CAPACITY.
     */
    static constexpr Integer DEFAULT_CAPACITY = 16;

    /**
     * The array of bins. Lazily initialized upon first insertion.
     * Size is always a power of two. Accessed directly by iterators.
     */
    VOLATILE Array<Node*> mTable;

    /**
     * Table initialization and resizing control.  When negative, the
     * table is being initialized or resized: -1 for initialization,
     * else -(1 + the number of active resizing threads).  Otherwise,
     * when table is null, holds the initial table size to use upon
     * creation, or 0 for default. After initialization, holds the
     * next element count value upon which to resize the table.
     */
    VOLATILE Integer mSizeCtl;

    AutoPtr<KeySetView> mKeySet;
};

inline Integer ConcurrentHashMap::Spread(
    /* [in] */ Integer h)
{
    return (h ^ (((unsigned Integer)h) >> 16)) & HASH_BITS;
}

inline ECode ConcurrentHashMap::Constructor()
{
    return NOERROR;
}

inline ECode ConcurrentHashMap::Constructor(
    /* [in] */ Integer initialCapacity,
    /* [in] */ Float loadFactor)
{
    return Constructor(initialCapacity, loadFactor, 1);
}

}
}
}

#endif // __CCM_UTIL_CONCURRENT_CONCURRENTHASHMAP_H__
