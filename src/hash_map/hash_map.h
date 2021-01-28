#ifndef HASH_MAP_H
#define HASH_MAP_H

#include "forward_list_storaged_size.h"
#include "extra_tools/extra_tools.h"

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <optional>
#include <iterator>
#include <type_traits>
#include <utility>
#include <cmath>

#define IS_GTEST_HASH_MAP_RUN

#ifdef IS_GTEST_HASH_MAP_RUN
#include <gtest/gtest.h>

namespace hash_map_tests {
class HashMapTest;
}

#endif

namespace hash_map_space {

using std::vector;
using std::forward_list;
using std::pair;

// PLAN TODO
// INFO: Wrong HashMap with Allocator logic
// TODO: tests
// TODO: performance measure mine and unordered_map
// TODO: test with non-default allocator

enum IteratorType {
    ITERATOR,
    CONST_ITERATOR
};

template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class TAllocator = std::allocator<T> // Can allocate objects of type T
          >
class HashMap {
#ifdef IS_GTEST_HASH_MAP_RUN
    friend hash_map_tests::HashMapTest;
#endif

    typedef HashMap<Key, T, Hash, KeyEqual, TAllocator> HashMapType;
public:
    typedef T MappedType;
    typedef Key key_type;
    typedef std::pair<const key_type, MappedType> value_type;    // it is our Node
    typedef ForwardListStoragedSize<value_type, TAllocator> ListType;
    typedef typename ListType::reference ValueTypeReference;
    using ContainerType = std::vector<ListType
		//, TAllocator	// is it normal?? 
	> ;
    typedef size_t size_type;
    using ThresholdType = float;

	using ValueTypeAllocator = typename //TAllocator::template rebind<value_type>::other ;
		std::allocator_traits<TAllocator>::template rebind_alloc<value_type>;
    typedef ValueTypeAllocator node_allocator_type;

    class node_type;

    template <IteratorType iterType>
    class IteratorCommon;
    typedef IteratorCommon<ITERATOR> iterator;
    typedef IteratorCommon<CONST_ITERATOR> const_iterator;

public:
    HashMap() : size_(0), container_(1) {}

    explicit HashMap(const node_allocator_type& alloc) : size_(0), container_(1, alloc) {}

    HashMap(std::initializer_list<value_type> il, const TAllocator& alloc = TAllocator())
        : size_(0), container_(1, alloc)
    {
        for (auto & elem : il) {
            insert(std::move(elem.first), std::move(elem.second));
        }
    }

    HashMap(const HashMap& other) : size_(other.size_), container_(other.container_),
        maxLoadFactor_(other.maxLoadFactor_) {}
    HashMap(HashMap<Key, T, TAllocator>&& other) { this->swap(other); }

    virtual ~HashMap() { container_.clear(); }

    const HashMap& operator=(HashMap right) {
        this->swap(right);
        return *this;
    }

    void swap(HashMap& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(container_, other.container_);
        std::swap(maxLoadFactor_, other.maxLoadFactor_);
    }

    template <class E, class P>
    iterator insert(E && key, P && mappedValue) {   //UR
        if (!bucket_count())
            resizeBucketCount(1u);
        iterator iter = find(key);
        key_type keyTemp = key;
        if (iter == this->end()) {
            auto count = calculateIndexByKey(key);
            container_[count].emplace_front(std::forward<E>(key), std::forward<P>(mappedValue));
            setSize(1 + size());
            rehash();
        }
        else {
            extract(key);
            return insert(std::forward<E>(key), std::forward<P>(mappedValue));
        }
        return find(keyTemp);
    }

    std::optional<T> get(const Key& key);

    template <class... Args>
    std::pair<iterator, bool> emplace(Args&&... args) {
        value_type node(std::forward<Args>(args)...);
        bool isExists = get(node.first).has_value();
        // TODO: Optimize: write insert_hint
        return std::make_pair(insert(std::move(node.first), std::move(node.second)), isExists);
    }

    template <class E>
    T& operator[](E && key) {
        HashMap::iterator it = find(key);

        // TODO: remove double find
        if (it == this->end())
            it = insert(std::forward<E>(key), T()); // bad if value_type hasn't a default constructor
        return it -> second;
    }

    //TODO: make const
    node_type extract(iterator position) {
        return extract(position->first);
    }

    node_type extract(const key_type& x) {
        iterator it = find(x);
        auto & list = *(it.getContainerIterator());
        node_type element(std::move(*it));
        list.erase(it.getListIterator());
        setSize(size() - 1);
        rehash();
        return element;
    }

    iterator find(const key_type& key);
    //const_iterator find(const Key& key) const;

    size_type size() const noexcept { return size_; }
    bool empty() const noexcept { return (size() == 0); }

    //--------------------Bucket Interface-------------------------//

    size_type bucket_count() const noexcept { return container_.size() - 1u; }    // minus end-empty element
    size_type bucket(const key_type& key) const noexcept { return calculateIndexByKey(key); }
    size_type max_bucket_count() const noexcept { return container_.max_size(); }
    iterator begin(size_type bucket) {
        auto & beg = container_.begin(); return iterator((beg + bucket)->begin(), (beg + bucket), &container_);
    }
    iterator end(size_type bucket) {
        auto & beg = container_.begin(); return iterator((beg + bucket)->end(), (beg + bucket), &container_);
    }

    void clear();

    //-----------------------Iterators--------------------------//

    iterator begin() {
        auto it = container_.begin();
        return iterator(it->begin(), it);
    }
    iterator end() {
        auto it = container_.begin();
		// Info: (bucket_count() == container_.size() - 1)
        std::advance(it, bucket_count());   //last element is end-empty one
        return iterator(it->end(), it);
    }
    const_iterator cbegin() const {
        auto cit = container_.cbegin();
        return const_iterator(cit->cbegin(), cit);
    }
    const_iterator cend() const {
        auto cit = container_.cbegin();
        std::advance(cit, bucket_count());
        return const_iterator(cit->cend(), cit);
    }

    //-------------------Load factor interface----------------//

    ThresholdType load_factor() const {
        return size() / (ThresholdType(1) * (static_cast<size_type>(!static_cast<bool>(bucket_count())) + bucket_count()));
    }
    ThresholdType max_load_factor() const { return maxLoadFactor_; }
    void max_load_factor(ThresholdType newMaxLoadFactor) { maxLoadFactor_ = newMaxLoadFactor; }
    ThresholdType sizeReminderOfDecreaseRehash() const { return 0.25; }

private:
    // if you add sm members then you need to add it to swap method
    size_type size_;            // equals the count of elements in the HashMap (size_ != container.size())
    ContainerType container_;
    ThresholdType maxLoadFactor_ = 3.14f;

private:
	// TODO: refactor to setElementCount
    void setSize(size_type newSize) { size_ = newSize; }
    void resizeBucketCount(size_type newBucketCount) { container_.resize(1u + newBucketCount); }
    size_type calculateIndexByKey(key_type const & key) { return Hash{}(key) % bucket_count(); }
    size_type calculateIndexByKey(key_type const & key, size_type bucketCount) { return Hash{}(key) % bucketCount; }

    //-------------Rehash---------------//

    void rehash();
    void regroupHashMap(size_type oldSize, size_type newSize);
    bool isBucketIncrease() const {
        return load_factor() > max_load_factor();
    }
    bool isBucketDecrease() const {
        return (load_factor() < sizeReminderOfDecreaseRehash() * max_load_factor());
    }
};

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
void HashMap<Key, T, Hash, KeyEqual, TAllocator>::
    regroupHashMap(HashMap::size_type oldSize, HashMap::size_type newSize)
{
    for (size_type i = 0; i < oldSize; i++) {
        auto & list = container_[i];
        for (auto prevIt = list.before_begin(), it = list.begin(); it != list.end(); ) {
            auto newIndex = calculateIndexByKey(it->first, newSize);
            if (newIndex == i) {
                ++prevIt;
                ++it;
            }
            else {
                container_[newIndex].push_front(std::move(*it));
                it = list.erase_after(prevIt);
            }
        }
    }
}

// need to test completely
template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
void HashMap<Key, T, Hash, KeyEqual, TAllocator>::rehash() {
    // condition to decide: rehash or not
    if (isBucketIncrease()) {
        auto oldSize = bucket_count();
        auto newSize = 2 * oldSize;
        resizeBucketCount(newSize);
        regroupHashMap(oldSize, newSize);
    }
    else if (isBucketDecrease()) {
        auto oldSize = bucket_count();
        auto newSize = bucket_count() / 2;
        regroupHashMap(oldSize, newSize);
        resizeBucketCount(newSize);
    }
}

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
void HashMap<Key, T, Hash, KeyEqual, TAllocator>::clear() {
    for (auto & list : container_) {
        list.clear();
    }
    rehash();
}

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
std::optional<T> HashMap<Key, T, Hash, KeyEqual, TAllocator>::get(const Key& key) {
    HashMap::iterator it = find(key);
    if (it == this->end())
        return std::nullopt;
    return it -> second;
}

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
typename HashMap<Key, T, Hash, KeyEqual, TAllocator>::iterator
HashMap<Key, T, Hash, KeyEqual, TAllocator>::find(const key_type& key) {
    if (!empty()) {
        size_type count = calculateIndexByKey(key);
        auto containerIter = container_.begin();
        std::advance(containerIter, count);

        auto nodeIter = std::find_if(containerIter->begin(), containerIter->end(),
            [&key] (const HashMap::value_type & node) {
                return KeyEqual()(node.first, key);
            });
        if (nodeIter == containerIter->end())
            return this->end();
        return HashMap::iterator(nodeIter, containerIter);
    }
    return this->end();
}

//---------------------Nested Template Classes-------------------//

template <bool B, class T>
struct IContainer {};
template <class T>
struct IContainer<true, T> {
    static typename T::iterator begin(T & list) { return list.begin(); }
    static typename T::iterator end(T & list) { return list.end(); }
};
template <class T>
struct IContainer<false, T> {
    static typename T::const_iterator begin(const T & list) { return list.cbegin(); }
    static typename T::const_iterator end(const T & list) { return list.cend(); }
};

using lipaboy_lib::enable_if_else;

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
template <IteratorType iterType>
class HashMap<Key, T, Hash, KeyEqual, TAllocator>::
        IteratorCommon {
public:
    friend HashMap<Key, T, Hash, KeyEqual, TAllocator>;

    typedef HashMap::ListType ListType;

    using NodeType = typename enable_if_else<iterType == ITERATOR,
                HashMap::value_type, const HashMap::value_type>::type;
	using ListIterator = typename enable_if_else<iterType == ITERATOR,
                typename HashMap::ListType::iterator, typename HashMap::ListType::const_iterator>::type;
	using ContainerIterator = typename enable_if_else<iterType == ITERATOR,
                typename HashMap::ContainerType::iterator, typename HashMap::ContainerType::const_iterator>::type;
public:
    IteratorCommon() = delete;
protected:
    IteratorCommon(ListIterator listIter, ContainerIterator containerIter)
        : listIter_(listIter), containerIter_(containerIter) {}
public:
    IteratorCommon(IteratorCommon const & other)
        : listIter_(other.listIter_), containerIter_(other.containerIter_) {}
    virtual ~IteratorCommon() {}

    bool operator!= (IteratorCommon const & other) {
        return (containerIter_ != other.containerIter_) // the order of comparations must be such that
			|| (listIter_ != other.listIter_);
    }
    bool operator== (IteratorCommon const & other) { return !((*this) != other); }

    NodeType& operator*() { return *listIter_; }
    NodeType* operator->() { return &(*listIter_); }
    // pre-increment
    IteratorCommon operator++() {
        if ((++listIter_) == listEnd()) {
            ++containerIter_;
            listIter_ = listBegin();
        }
        return (*this);
    }
    // post-increment
    IteratorCommon operator++(int) {
        IteratorCommon prev = (*this);
        ++(*this);
        return std::move(prev);
    }
    // cast from iter to const
    operator IteratorCommon<CONST_ITERATOR>() {
        return IteratorCommon<CONST_ITERATOR>(listIter_, containerIter_);
    }

protected:
    ListIterator listBegin() { return IContainer<iterType == ITERATOR, ListType>::begin(*containerIter_); }
    ListIterator listEnd() { return IContainer<iterType == ITERATOR, ListType>::end(*containerIter_); }
    ContainerIterator getContainerIterator() const { return containerIter_; }
    ListIterator getListIterator() const { return listIter_; }

protected:
    ListIterator listIter_;
    ContainerIterator containerIter_;

#ifdef IS_GTEST_HASH_MAP_RUN
    friend hash_map_tests::HashMapTest;
#endif
};

template <class Key,
          class T,
          class Hash,
          class KeyEqual,
          class TAllocator>
class HashMap<Key, T, Hash, KeyEqual, TAllocator>::node_type {
public:
    typedef HashMap::key_type KeyType;
    typedef HashMap::MappedType MappedType;
public:
    node_type(std::pair<const KeyType, MappedType>&& pair)
        : key_(std::move(pair.first)), value_(std::move(pair.second)) {}

    KeyType& key() { return key_; }
    MappedType& value() { return value_; }
    const KeyType& key() const { return key_; }
    const MappedType& value() const { return value_; }
private:
    KeyType key_;
    MappedType value_;
};

}

#endif // HASH_MAP_H
