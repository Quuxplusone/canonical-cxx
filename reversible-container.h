#pragma once

#include <iterator>  // reverse_iterator

template<class CRTP>
struct reversible_container {
    using reverse_iterator = std::reverse_iterator<typename CRTP::iterator>;
    using const_reverse_iterator = std::reverse_iterator<typename CRTP::const_iterator>;

    reverse_iterator rbegin() { return reverse_iterator(this->end()); }
    reverse_iterator rbegin() const { return crbegin(); }
    const_reverse_iterator crbegin() { return const_reverse_iterator(this->cend()); }

    reverse_iterator rend() { return reverse_iterator(this->begin()); }
    reverse_iterator rend() const { return crend(); }
    const_reverse_iterator crend() { return const_reverse_iterator(this->cbegin()); }

    // The SGI STL's "ReversibleContainer" concept includes the two member functions
    // typename reverse_iterator::reference back() { return *rbegin(); }
    // typename const_reverse_iterator::reference back() const { return *crbegin(); }
    // but as `class CRTP` itself may know a more efficient way to compute back(),
    // we don't presume to implement it here.
};
