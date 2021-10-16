
// Copyright 2005-2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18.

// This implements the extensions to the standard.
// It's undocumented, so you shouldn't use it....

#ifndef BOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP
#define BOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP

#include <boost/container_hash/hash.hpp>
#include <boost/config.hpp>

namespace boost
{
    //
    // boost::hash
    //

    template <class T> struct hash
        : boost::hash_detail::hash_base<T>
    {
        std::size_t operator()(T const& val) const
        {
            return hash_value(val);
        }
    };
}

#endif // #ifndef BOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP
