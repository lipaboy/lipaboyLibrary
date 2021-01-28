#ifndef FORWARD_LIST_SIZE_STORAGED_H
#define FORWARD_LIST_SIZE_STORAGED_H

#include <forward_list>
#include <memory>

namespace hash_map_space {

using std::forward_list;

template <class T,
          class TAllocator = std::allocator<T> >
class ForwardListStoragedSize {
public:
    typedef size_t size_type;
    typedef T ValueType;
    using ForwardListType = forward_list<ValueType
	//	, TAllocator
	>;
    typedef typename ForwardListType::iterator iterator;
    typedef typename ForwardListType::const_iterator const_iterator;
    typedef typename ForwardListType::reference reference;
public:
    ForwardListStoragedSize() : list(), size_(0) {}
    ForwardListStoragedSize(const ForwardListStoragedSize& obj) :
        list(obj.list), size_(obj.size_) {}
    ForwardListStoragedSize(ForwardListStoragedSize&& obj) noexcept : list(std::move(obj.list)), size_(obj.size_) {}

    virtual ~ForwardListStoragedSize() { list.clear(); }

    template <class E>
    reference push_front(E && value) {
        return emplace_front(std::forward<E>(value));
    }

    template<class... Args >
    reference emplace_front(Args&&... args) {
        setSize(1 + size());
        return list.emplace_front(std::forward<Args>(args)...);
    }

    iterator erase_after(const_iterator pos) {
        if (empty())
            return list.erase_after(pos, pos);  //remove constness of const_iterator
        setSize(size() - 1);
        return list.erase_after(pos);
    }

    // Complexity: O(size())
    iterator erase(const_iterator pos) {
        if (empty())
            return list.erase_after(pos, pos);  //remove constness of const_iterator
        setSize(size() - 1);
        auto iter = before_begin();
        auto previous = iter;
        for (; iter != pos; iter++)
            previous = iter;
        return list.erase_after(previous);
    }

    void clear() { list.clear(); setSize(0); }

    iterator before_begin() { return list.before_begin(); }
    iterator begin() { return list.begin(); }
    iterator end()   { return list.end(); }
    const_iterator cbegin() const { return list.cbegin(); }
    const_iterator cend() const { return list.cend(); }
    size_type size() const noexcept { return size_; }
    size_type empty() const noexcept { return (size_ == 0); }

private:
    ForwardListType list;
    size_type size_;

    void setSize(size_type newSize) { size_ = newSize; }
};

}

#endif // FORWARD_LIST_SIZE_STORAGED_H
