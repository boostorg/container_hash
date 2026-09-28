// Copyright 2022, 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_HASH_IS_DESCRIBED_CLASS_HPP_INCLUDED
#define BOOST_HASH_IS_DESCRIBED_CLASS_HPP_INCLUDED

#include "is_tuple_like.hpp"
#include "is_range.hpp"
#include <boost/describe/bases.hpp>
#include <boost/describe/members.hpp>
#include <type_traits>

namespace boost
{
namespace container_hash
{

#if defined(BOOST_DESCRIBE_CXX11)

template<class T> struct is_described_class: std::integral_constant<bool,
    std::is_class<T>::value &&
    describe::has_describe_bases<T>::value &&
    describe::has_describe_members<T>::value &&
    !is_tuple_like<T>::value &&
    !is_range<T>::value>
{
};

#else

template<class T> struct is_described_class: std::false_type
{
};

#endif

} // namespace container_hash
} // namespace boost

#endif // #ifndef BOOST_HASH_IS_DESCRIBED_CLASS_HPP_INCLUDED
