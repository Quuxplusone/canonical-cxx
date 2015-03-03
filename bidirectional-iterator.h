#pragma once

#include <cstddef>  // ptrdiff_t
#include <iterator>  // iterator, bidirectional_iterator_tag
#include <memory>  // addressof
#include <type_traits>  // remove_cv_t

#include "reversible-container.h"

template<
    class QualifiedType,
    class UnqualifiedType = std::remove_cv_t<QualifiedType>,
    class IteratorBase = std::iterator<
        std::bidirectional_iterator_tag,
        UnqualifiedType,
        std::ptrdiff_t,
        QualifiedType*,
        QualifiedType&
    >
> struct BidirectionalVectorIterator;

// This is just a simple container class for illustrative purposes.
//
template<class T>
class BidirectionalVector : reversible_container<BidirectionalVector<T>> {
    T data[10];

  public:
    using iterator = BidirectionalVectorIterator<T>;
    using const_iterator = BidirectionalVectorIterator<const T>;

    iterator begin() { return iterator(data); }
    const_iterator begin() const { return cbegin(); }
    const_iterator cbegin() const { return const_iterator(data); }
    iterator end() { return iterator(data+10); }
    const_iterator end() const { return cbegin(); }
    const_iterator cend() const { return const_iterator(data+10); }
};

// Inheriting from std::iterator ensures that there exists an appropriate
// specialization of the std::iterator_traits class template.
//
// Notice that `BidirectionalVectorIterator<const T>::value_type` is `T`,
// not `const T`. This follows the precedent set by the standard library
// containers.
//
// [iterator.requirements.general]p4: In the language of the standard,
// `BidirectionalVectorIterator<T>` is a bidirectional iterator, a forward iterator,
// an input iterator, an output iterator, and a mutable iterator.
// `BidirectionalVectorIterator<const T>` is a bidirectional iterator,
// a forward iterator, an input iterator, and a constant iterator.
//
template<class QualifiedType,
         class UnqualifiedType /* = std::remove_cv_t<QualifiedType> */,
         class IteratorBase /* = std::iterator<...> */ >
struct BidirectionalVectorIterator : IteratorBase
{
    using typename IteratorBase::reference;
    using typename IteratorBase::pointer;

    // [forward.iterators]p1.2: MyBidirectionalIterator must be DefaultConstructible,
    // which implies that a default-constructed MyBidirectionalIterator must be able to be
    // the source of a move or copy operation. (Don't leave any data members of
    // primitive types uninitialized!)
    //
    // [forward.iterators]p2: Comparing `MyBidirectionalIterator{} == MyBidirectionalIterator{}`
    // must deterministically return `true`. It is often useful to have
    // `MyBidirectionalIterator{} == MyContainer{}.end()`, but it's not required.
    //
    // Using `{}` instead of `=default` makes the constructor user-provided, which
    // means the user is allowed to construct `const Vector<int>::iterator foo;`
    // with no initializer.
    //
    BidirectionalVectorIterator() {}

    // You'll usually need some way to construct an iterator from the innards
    // of a container. Remember to make this constructor `private` and `explicit`
    // so that it will interfere as little as possible with the user's own code.
    // Implicit conversions from `nullptr` are particularly insidious.
    //
    friend class BidirectionalVector<UnqualifiedType>;
  private:
    explicit BidirectionalVectorIterator(QualifiedType* ptr) {
        // perform your custom initialization
    }
  public:

    // Explicitly defaulting these special member functions prevents you from
    // accidentally disabling any of them via (for example) adding a user-provided
    // destructor.
    //
#if ITERATOR_IS_MOVEONLY
    // [iterator.iterators]p2.1: According to the standard, iterators must be
    // copyable; a move-only type does not conform to the "Iterator" concept.
    // However, in practice it's reasonable to imagine an iterator (such as a
    // database cursor) that cannot be meaningfully copied. For such
    // iterators, it suffices to delete the special member functions and the
    // postfix operator++.
    //
    BidirectionalVectorIterator(BidirectionalVectorIterator const&) = delete;
    BidirectionalVectorIterator& operator=(BidirectionalVectorIterator const&) = delete;
#else
    BidirectionalVectorIterator(BidirectionalVectorIterator const&) = default;
    BidirectionalVectorIterator& operator=(BidirectionalVectorIterator const&) = default;
#endif
    ~BidirectionalVectorIterator() = default;

#if POSSIBLE
    // Explicitly writing `noexcept` ensures that when a `std::vector<MyBidirectionalIterator>`
    // is resized, it will move-construct its elements instead of copy-constructing them.
    //
    BidirectionalVectorIterator(BidirectionalVectorIterator&&) noexcept = default;
#else
    // If any of your non-static data members lack `noexcept` move-constructors,
    // the above declaration will give a compiler error: [dcl.fct.def.default]p3.
    // You could remove the `noexcept`, but that only pushes the problem downstream
    // to YOUR users. Better to fix the problem by adding a user-provided move-constructor
    // that is properly `noexcept`.
    //
    BidirectionalVectorIterator(BidirectionalVectorIterator&& rhs) noexcept {
        // perform memberwise swap with rhs (using std::swap is okay)
    }
#endif

#if CONTAINS_ANY_BADLY_BEHAVED_DATA_MEMBERS
    // Standard library types (for example std::string) are not required to support
    // move-assignments of the form `x = std::move(x)`. Since the default definition
    // of move-assignment is defined in terms of move-assigning each member in turn,
    // this means that you must write a user-provided move-assignment operator if
    // MyBidirectionalIterator has any non-static data members of standard library types.
    //
    BidirectionalVectorIterator& operator=(BidirectionalVectorIterator&& rhs) {
        if (this != &rhs) {
            // perform memberwise swap with rhs (using std::swap is okay)
        }
        return *this;
    }
#else
    // If your data members are only of primitive types (for example int)
    // and known-safe user-defined types (for example BidirectionalVectorIterator),
    // then it is safe to rely on the default move-assignment operator.
    // Explicitly default it for clarity.
    //
    BidirectionalVectorIterator& operator=(BidirectionalVectorIterator&&) = default;
#endif

#if not ITERATOR_IS_MOVEONLY
    BidirectionalVectorIterator operator++(int) {
        BidirectionalVectorIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    BidirectionalVectorIterator operator--(int) {
        BidirectionalVectorIterator tmp = *this;
        --(*this);
        return tmp;
    }
#endif

    BidirectionalVectorIterator& operator++() {
        // perform your custom pre-increment operation
        return *this;
    }

    // A bidirectional iterator is simply a forward iterator that also
    // supports pre- and post- versions of operator--.
    //
    BidirectionalVectorIterator& operator--() {
        // perform your custom pre-decrement operation
        return *this;
    }

    // InputIterator requires that `*(*this)` be convertible to value_type.
    // OutputIterator requires that `*(*this) = v;` be well-formed.
    // The simplest way to satisfy both requirements is to make `*(*this)` return
    // a (qualified) reference to value_type.
    //
    reference operator*() const {
        // perform your custom dereference operation
    }

    // If you know that `value_type` doesn't overload the unary `&` operator,
    // feel free to use `&` instead of `std::addressof` here.
    //
    pointer operator->() const { return std::addressof(*(*this)); }

    // [iterator.iterators]p2.1: Iterators must be swappable.
    // Placing this `friend` function inline is the easiest way to ensure
    // that it will end up in the right namespace to be picked up by ADL.
    //
    friend void swap(BidirectionalVectorIterator& a, BidirectionalVectorIterator& b)
    {
        // If your data members are only of primitive types (for example int)
        // and known-safe user-defined types (for example BidirectionalVectorIterator),
        // then it is safe to remove the following guard.
        // Otherwise, you must keep this guard in order to protect against
        // self-move-assignment inside std::swap.
        //
        if (&a != &b) {
            // perform memberwise swap between a and b (using std::swap is okay)
        }
    }

    // Be aware of [forward.iterators]p2: Comparing two default-constructed
    // iterators must deterministically return `true`.
    //
    // [container.requirements.general]p7: `v.begin() == v.cbegin()` must
    // be well-formed.
    //
    template<class QT>
    bool operator==(BidirectionalVectorIterator<QT> const& other) const {
        // perform your custom iterator-comparison operation
    }

    template<class QT>
    bool operator!=(BidirectionalVectorIterator<QT> const& other) const { return !(*this == other); }

    // [container.requirements.general] Table 96: A plain iterator must be
    // convertible (i.e., implicitly convertible) to a const_iterator.
    // This means you must provide either a constructor or a conversion operator.
    // Since you have to avoid any conflict with the copy constructor, the
    // first option would look like this:
    //
    //   template<class QT>
    //   BidirectionalVectorIterator(const BidirectionalVectorIterator<QT>&) { ... }
    //
    // However, we can avoid templates if we go with the second option.
    //
#if ITERATOR_IS_MOVEONLY
    operator BidirectionalVectorIterator<const UnqualifiedType>() && {
        // perform your custom conversion
    }
#else
    operator BidirectionalVectorIterator<const UnqualifiedType>() const {
        // perform your custom conversion
    }
#endif

  private:
    // declare your custom data members
};
