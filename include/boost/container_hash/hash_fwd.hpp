// Copyright 2005-2009 Daniel James.
// Copyright 2021 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_FUNCTIONAL_HASH_FWD_HPP
#define BOOST_FUNCTIONAL_HASH_FWD_HPP

#include <cstddef>

namespace boost
{
    template <class T> struct hash;

    template <class T> void hash_combine(std::size_t& seed, T const& v);

    template <class It> std::size_t hash_range(It, It);
    template <class It> void hash_range(std::size_t&, It, It);

    template <class It> std::size_t hash_unordered_range(It, It);
    template <class It> void hash_unordered_range(std::size_t&, It, It);
}

#endif // #ifndef BOOST_FUNCTIONAL_HASH_FWD_HPP
