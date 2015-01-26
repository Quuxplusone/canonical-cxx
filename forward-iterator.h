#pragma once

#include <iterator>  // iterator, iterator_traits, forward_iterator_tag
#include <memory>  // addressof

// Inheriting from std::iterator is the preferred way to get the following
// (required) member typedefs:
//   using value_type = MyValueType;
//   using difference_type = std::ptrdiff_t;
//   using pointer = MyValueType*;
//   using reference = MyValueType&;
//   using iterator_category = std::forward_iterator_tag;
//
// It also ensures that there exists an appropriate specialization of the
// std::iterator_traits class template.
//
struct MyForwardIterator :
    std::iterator<std::forward_iterator_tag, MyValueType>
{
    // [forward.iterators]p1.2: MyForwardIterator must be DefaultConstructible,
    // which implies that a default-constructed MyForwardIterator must be able to be
    // the source of a move or copy operation. (Don't leave any data members of
    // primitive types uninitialized!)
    //
    // [forward.iterators]p2: Comparing `MyForwardIterator{} == MyForwardIterator{}`
    // must deterministically return `true`. It is often useful to have
    // `MyForwardIterator{} == MyContainer{}.end()`, but it's not required.
    //
    // Using `{}` instead of `=default` makes the constructor user-provided, which
    // means the user is allowed to construct `const MyForwardIterator foo;`
    // with no initializer.
    //
    MyForwardIterator() {}

    // Explicitly defaulting these special member functions prevents you from
    // accidentally disabling any of them via (for example) adding a user-provided
    // destructor.
    //
    MyForwardIterator(MyForwardIterator const&) = default;
    MyForwardIterator& operator=(MyForwardIterator const&) = default;
    ~MyForwardIterator() = default;

#if POSSIBLE
    // Explicitly writing `noexcept` ensures that when a `std::vector<MyForwardIterator>`
    // is resized, it will move-construct its elements instead of copy-constructing them.
    //
    MyForwardIterator(MyForwardIterator&&) noexcept = default;
#else
    // If any of your non-static data members lack `noexcept` move-constructors,
    // the above declaration will give a compiler error: [dcl.fct.def.default]p3.
    // You could remove the `noexcept`, but that only pushes the problem downstream
    // to YOUR users. Better to fix the problem by adding a user-provided move-constructor
    // that is properly `noexcept`.
    //
    MyForwardIterator(MyForwardIterator&& rhs) noexcept {
        // perform memberwise swap with rhs (using std::swap is okay)
    }
#endif

#if CONTAINS_ANY_BADLY_BEHAVED_DATA_MEMBERS
    // Standard library types (for example std::string) are not required to support
    // move-assignments of the form `x = std::move(x)`. Since the default definition
    // of move-assignment is defined in terms of move-assigning each member in turn,
    // this means that you must write a user-provided move-assignment operator if
    // MyForwardIterator has any non-static data members of standard library types.
    //
    MyForwardIterator& operator=(MyForwardIterator&& rhs) {
        if (this != &rhs) {
            // perform memberwise swap with rhs (using std::swap is okay)
        }
        return *this;
    }
#else
    // If your data members are only of primitive types (for example int)
    // and known-safe user-defined types (for example MyForwardIterator),
    // then it is safe to rely on the default move-assignment operator.
    // Explicitly default it for clarity.
    //
    MyForwardIterator& operator=(MyForwardIterator&&) = default;
#endif

    MyForwardIterator operator++(int) {
        MyForwardIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    MyForwardIterator& operator++() {
        // perform your custom pre-increment operation
        return *this;
    }

    // Be aware of [forward.iterators]p2: Comparing two default-constructed
    // iterators must deterministically return `true`.
    //
    bool operator==(MyForwardIterator const& other) const {
        // perform your custom iterator-comparison operation
    }

    bool operator!=(MyForwardIterator const& other) const { return !(*this == other); }

    // InputIterator requires that `*(*this)` be convertible to MyValueType.
    // OutputIterator requires that `*(*this) = v;` be well-formed.
    // The simplest way to satisfy both requirements is to make `*(*this)` return
    // a reference to MyValueType.
    //
    // Notice that `const vector::iterator` (a const iterator to non-const data)
    // is not the same type as `vector::const_iterator` (a non-const iterator
    // to const data). If you want to support a `const_iterator`-style type,
    // the easiest way is to replace `MyValueType` by `const MyValueType`
    // throughout this file. (This will make `reference` a typedef for
    // `const MyValueType&`, and so on, which is exactly what a `const_iterator`
    // type wants.)
    //
    reference operator*() const {
        // perform your custom dereference operation
    }

    // If you know that MyValueType doesn't overload the unary `&` operator,
    // feel free to use `&` instead of `std::addressof` here.
    //
    pointer operator->() const { return std::addressof(*(*this)); }

private:
    // declare your custom data members
};
