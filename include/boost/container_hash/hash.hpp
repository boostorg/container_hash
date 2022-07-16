// Copyright 2005-2014 Daniel James.
// Copyright 2021 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

// Based on Peter Dimov's proposal
// http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
// issue 6.18.
//
// This also contains public domain code from MurmurHash. From the
// MurmurHash header:

// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef BOOST_FUNCTIONAL_HASH_HASH_HPP
#define BOOST_FUNCTIONAL_HASH_HASH_HPP

#include <boost/container_hash/hash_fwd.hpp>
#include <boost/container_hash/detail/is_range.hpp>
#include <boost/container_hash/detail/is_contiguous_range.hpp>
#include <boost/container_hash/detail/is_unordered_range.hpp>
#include <boost/container_hash/detail/hash_tuple.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/is_signed.hpp>
#include <boost/type_traits/is_unsigned.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/type_traits/enable_if.hpp>
#include <boost/type_traits/conjunction.hpp>
#include <boost/cstdint.hpp>
#include <string>
#include <iterator>
#include <complex>
#include <utility>
#include <limits>
#include <climits>
#include <cstring>

#if !defined(BOOST_NO_CXX11_SMART_PTR)
# include <memory>
#endif

#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
#include <typeindex>
#endif

#if !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)
#include <system_error>
#endif

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)
#include <optional>
#endif

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)
#include <variant>
#endif

#if defined(_MSC_VER)
#   define BOOST_FUNCTIONAL_HASH_ROTL32(x, r) _rotl(x,r)
#else
#   define BOOST_FUNCTIONAL_HASH_ROTL32(x, r) (x << r) | (x >> (32 - r))
#endif

namespace boost
{

    //
    // boost::hash_value
    //

    // integral types

    namespace hash_detail
    {
        template<class T,
            bool bigger_than_size_t = (sizeof(T) > sizeof(std::size_t)),
            bool is_unsigned = boost::is_unsigned<T>::value,
            std::size_t size_t_bits = sizeof(std::size_t) * CHAR_BIT,
            std::size_t type_bits = sizeof(T) * CHAR_BIT>
        struct hash_integral_impl;

        template<class T, bool is_unsigned, std::size_t size_t_bits, std::size_t type_bits> struct hash_integral_impl<T, false, is_unsigned, size_t_bits, type_bits>
        {
            static std::size_t fn( T v )
            {
                return static_cast<std::size_t>( v );
            }
        };

        template<class T, std::size_t size_t_bits, std::size_t type_bits> struct hash_integral_impl<T, true, false, size_t_bits, type_bits>
        {
            static std::size_t fn( T v )
            {
                typedef typename boost::make_unsigned<T>::type U;

                if( v >= 0 )
                {
                    return hash_integral_impl<U>::fn( static_cast<U>( v ) );
                }
                else
                {
                    return ~hash_integral_impl<U>::fn( static_cast<U>( ~static_cast<U>( v ) ) );
                }
            }
        };

        template<class T> struct hash_integral_impl<T, true, true, 32, 64>
        {
            static std::size_t fn( T v )
            {
                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( v >> 32 ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( v ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        template<class T> struct hash_integral_impl<T, true, true, 32, 128>
        {
            static std::size_t fn( T v )
            {
                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( v >> 96 ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( v >> 64 ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( v >> 32 ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( v ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        template<class T> struct hash_integral_impl<T, true, true, 64, 128>
        {
            static std::size_t fn( T v )
            {
                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( v >> 64 ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( v ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

    } // namespace hash_detail

    template <typename T>
    typename boost::enable_if_<boost::is_integral<T>::value, std::size_t>::type
        hash_value( T v )
    {
        return hash_detail::hash_integral_impl<T>::fn( v );
    }

    // enumeration types

    template <typename T>
    typename boost::enable_if_<boost::is_enum<T>::value, std::size_t>::type
        hash_value( T v )
    {
        // This should in principle return the equivalent of
        //
        // boost::hash_value( to_underlying(v) );
        //
        // However, the C++03 implementation of underlying_type,
        //
        // conditional<is_signed<T>, make_signed<T>, make_unsigned<T>>::type::type
        //
        // generates a legitimate -Wconversion warning in is_signed,
        // because -1 is not a valid enum value when all the enumerators
        // are nonnegative.
        //
        // So the legacy implementation will have to do for now.

        return static_cast<std::size_t>( v );
    }

    // floating point types

    namespace hash_detail
    {
        template<class T,
            std::size_t Bits = sizeof(T) * CHAR_BIT,
            int Digits = std::numeric_limits<T>::digits,
            std::size_t size_t_bits = sizeof(std::size_t) * CHAR_BIT>
        struct hash_float_impl;

        // float
        template<class T, int Digits, std::size_t size_t_bits> struct hash_float_impl<T, 32, Digits, size_t_bits>
        {
            static std::size_t fn( T v )
            {
                boost::uint32_t w;
                std::memcpy( &w, &v, sizeof( v ) );

                return w;
            }
        };

        // double
        template<class T, int Digits> struct hash_float_impl<T, 64, Digits, 64>
        {
            static std::size_t fn( T v )
            {
                boost::uint64_t w;
                std::memcpy( &w, &v, sizeof( v ) );

                return w;
            }
        };

        template<class T, int Digits> struct hash_float_impl<T, 64, Digits, 32>
        {
            static std::size_t fn( T v )
            {
                boost::uint32_t w[ 2 ];
                std::memcpy( &w, &v, sizeof( v ) );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        // 80 bit long double in 12 bytes
        template<class T> struct hash_float_impl<T, 96, 64, 64>
        {
            static std::size_t fn( T v )
            {
                boost::uint64_t w[ 2 ] = {};
                std::memcpy( &w, &v, 80 / CHAR_BIT );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        template<class T> struct hash_float_impl<T, 96, 64, 32>
        {
            static std::size_t fn( T v )
            {
                boost::uint32_t w[ 3 ] = {};
                std::memcpy( &w, &v, 80 / CHAR_BIT );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[2] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        // 80 bit long double in 16 bytes
        template<class T> struct hash_float_impl<T, 128, 64, 64>
        {
            static std::size_t fn( T v )
            {
                boost::uint64_t w[ 2 ] = {};
                std::memcpy( &w, &v, 80 / CHAR_BIT );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        template<class T> struct hash_float_impl<T, 128, 64, 32>
        {
            static std::size_t fn( T v )
            {
                boost::uint32_t w[ 3 ] = {};
                std::memcpy( &w, &v, 80 / CHAR_BIT );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[2] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        // 128 bit long double
        template<class T, int Digits> struct hash_float_impl<T, 128, Digits, 64>
        {
            static std::size_t fn( T v )
            {
                boost::uint64_t w[ 2 ];
                std::memcpy( &w, &v, sizeof( v ) );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

        template<class T, int Digits> struct hash_float_impl<T, 128, Digits, 32>
        {
            static std::size_t fn( T v )
            {
                boost::uint32_t w[ 4 ];
                std::memcpy( &w, &v, sizeof( v ) );

                std::size_t seed = 0;

                seed ^= static_cast<std::size_t>( w[0] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[1] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[2] ) + ( seed << 6 ) + ( seed >> 2 );
                seed ^= static_cast<std::size_t>( w[3] ) + ( seed << 6 ) + ( seed >> 2 );

                return seed;
            }
        };

    } // namespace hash_detail

    template <typename T>
    typename boost::enable_if_<boost::is_floating_point<T>::value, std::size_t>::type
        hash_value( T v )
    {
        return boost::hash_detail::hash_float_impl<T>::fn( v + 0 );
    }

    // pointer types

    // `x + (x >> 3)` adjustment by Alberto Barbati and Dave Harris.
    template <class T> std::size_t hash_value( T* const& v )
    {
        boost::uintptr_t x = reinterpret_cast<boost::uintptr_t>( v );
        return boost::hash_value( x + (x >> 3) );
    }

    // array types

    template<class T, std::size_t N>
    inline std::size_t hash_value( T const (&x)[ N ] )
    {
        return boost::hash_range( x, x + N );
    }

    template<class T, std::size_t N>
    inline std::size_t hash_value( T (&x)[ N ] )
    {
        return boost::hash_range( x, x + N );
    }

    // complex

    template <class T>
    std::size_t hash_value( std::complex<T> const& v )
    {
        std::size_t re = boost::hash<T>()( v.real() );
        std::size_t im = boost::hash<T>()( v.imag() );

        im ^= re + ( im << 6 ) + ( im >> 2 );
        return im;
    }

    // pair

    template <class A, class B>
    std::size_t hash_value( std::pair<A, B> const& v )
    {
        std::size_t seed = 0;

        boost::hash_combine( seed, v.first );
        boost::hash_combine( seed, v.second );

        return seed;
    }

    // ranges (list, set, deque...)

    template <typename T>
    typename boost::enable_if_<hash_detail::is_range<T>::value && !hash_detail::is_contiguous_range<T>::value && !hash_detail::is_unordered_range<T>::value, std::size_t>::type
        hash_value( T const& v )
    {
        return boost::hash_range( v.begin(), v.end() );
    }

    // contiguous ranges (string, vector, array)

    template <typename T>
    typename boost::enable_if_<hash_detail::is_contiguous_range<T>::value, std::size_t>::type
        hash_value( T const& v )
    {
        return boost::hash_range( v.data(), v.data() + v.size() );
    }

    // unordered ranges (unordered_set, unordered_map)

    template <typename T>
    typename boost::enable_if_<hash_detail::is_unordered_range<T>::value, std::size_t>::type
        hash_value( T const& v )
    {
        return boost::hash_unordered_range( v.begin(), v.end() );
    }

#if defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION == 141 && BOOST_CXX_VERSION >= 201700L

    // resolve ambiguity with unconstrained stdext::hash_value in <xhash> :-/

    template<template<class...> class L, class... T>
    typename boost::enable_if_<hash_detail::is_range<L<T...>>::value && !hash_detail::is_contiguous_range<L<T...>>::value && !hash_detail::is_unordered_range<L<T...>>::value, std::size_t>::type
        hash_value( L<T...> const& v )
    {
        return boost::hash_range( v.begin(), v.end() );
    }

    // contiguous ranges (string, vector, array)

    template<template<class...> class L, class... T>
    typename boost::enable_if_<hash_detail::is_contiguous_range<L<T...>>::value, std::size_t>::type
        hash_value( L<T...> const& v )
    {
        return boost::hash_range( v.data(), v.data() + v.size() );
    }

    template<template<class, std::size_t> class L, class T, std::size_t N>
    typename boost::enable_if_<hash_detail::is_contiguous_range<L<T, N>>::value, std::size_t>::type
        hash_value( L<T, N> const& v )
    {
        return boost::hash_range( v.data(), v.data() + v.size() );
    }

    // unordered ranges (unordered_set, unordered_map)

    template<template<class...> class L, class... T>
    typename boost::enable_if_<hash_detail::is_unordered_range<L<T...>>::value, std::size_t>::type
        hash_value( L<T...> const& v )
    {
        return boost::hash_unordered_range( v.begin(), v.end() );
    }

#endif

    // std::unique_ptr, std::shared_ptr

#if !defined(BOOST_NO_CXX11_SMART_PTR)

    template <typename T>
    std::size_t hash_value( std::shared_ptr<T> const& x )
    {
        return boost::hash_value( x.get() );
    }

    template <typename T, typename Deleter>
    std::size_t hash_value( std::unique_ptr<T, Deleter> const& x )
    {
        return boost::hash_value( x.get() );
    }

#endif

    // std::type_index

#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)

    inline std::size_t hash_value( std::type_index const& v )
    {
        return v.hash_code();
    }

#endif

    // std::error_code, std::error_condition

#if !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)

    inline std::size_t hash_value( std::error_code const& v )
    {
        std::size_t seed = 0;

        boost::hash_combine( seed, v.value() );
        boost::hash_combine( seed, &v.category() );

        return seed;
    }

    inline std::size_t hash_value( std::error_condition const& v )
    {
        std::size_t seed = 0;

        boost::hash_combine( seed, v.value() );
        boost::hash_combine( seed, &v.category() );

        return seed;
    }

#endif

    // std::optional

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)

    template <typename T>
    std::size_t hash_value( std::optional<T> const& v )
    {
        if( !v )
        {
            // Arbitray value for empty optional.
            return 0x12345678;
        }
        else
        {
            return boost::hash<T>()(*v);
        }
    }

#endif

    // std::variant

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)

    inline std::size_t hash_value( std::monostate )
    {
        return 0x87654321;
    }

    template <typename... Types>
    std::size_t hash_value( std::variant<Types...> const& v )
    {
        std::size_t seed = 0;

        hash_combine( seed, v.index() );
        std::visit( [&seed](auto&& x) { hash_combine(seed, x); }, v );

        return seed;
    }

#endif

    //
    // boost::hash_combine
    //

    namespace hash_detail
    {

        template<std::size_t Bits> struct hash_combine_impl
        {
            template <typename SizeT>
            inline static SizeT fn(SizeT seed, SizeT value)
            {
                seed ^= value + 0x9e3779b9 + (seed<<6) + (seed>>2);
                return seed;
            }
        };

        template<> struct hash_combine_impl<32>
        {
            inline static boost::uint32_t fn(boost::uint32_t h1, boost::uint32_t k1)
            {
                const boost::uint32_t c1 = 0xcc9e2d51;
                const boost::uint32_t c2 = 0x1b873593;

                k1 *= c1;
                k1 = BOOST_FUNCTIONAL_HASH_ROTL32(k1,15);
                k1 *= c2;

                h1 ^= k1;
                h1 = BOOST_FUNCTIONAL_HASH_ROTL32(h1,13);
                h1 = h1*5+0xe6546b64;

                return h1;
            }
        };

        template<> struct hash_combine_impl<64>
        {
            inline static boost::uint64_t fn(boost::uint64_t h, boost::uint64_t k)
            {
                const boost::uint64_t m = (boost::uint64_t(0xc6a4a793) << 32) + 0x5bd1e995;
                const int r = 47;

                k *= m;
                k ^= k >> r;
                k *= m;

                h ^= k;
                h *= m;

                // Completely arbitrary number, to prevent 0's
                // from hashing to 0.
                h += 0xe6546b64;

                return h;
            }
        };
    }

#if defined(BOOST_MSVC)
#pragma warning(push)
#if BOOST_MSVC <= 1400
#pragma warning(disable:4267) // 'argument' : conversion from 'size_t' to
                              // 'unsigned int', possible loss of data
                              // A misguided attempt to detect 64-bit
                              // incompatability.
#endif
#endif

    template <class T>
    inline void hash_combine(std::size_t& seed, T const& v)
    {
        boost::hash<T> hasher;
        seed = boost::hash_detail::hash_combine_impl<sizeof(std::size_t) * CHAR_BIT>::fn(seed, hasher(v));
    }

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

    //
    // boost::hash_range
    //

    template <class It>
    inline std::size_t hash_range(It first, It last)
    {
        std::size_t seed = 0;

        hash_range(seed, first, last);

        return seed;
    }

    template <class It>
    inline void hash_range(std::size_t& seed, It first, It last)
    {
        for(; first != last; ++first)
        {
            hash_combine<typename std::iterator_traits<It>::value_type>(seed, *first);
        }
    }

    //
    // boost::hash
    //

    template <class T> struct hash
    {
        typedef T argument_type;
        typedef std::size_t result_type;

        std::size_t operator()( T const& val ) const
        {
            return hash_value( val );
        }
    };

#if defined(_MSVC_STL_VERSION) && _MSVC_STL_VERSION == 141 && BOOST_CXX_VERSION >= 201700L

    // msvc-14.1 has stdext::hash_value for basic_string in <xhash> :-/

    template<class E, class T, class A> struct hash< std::basic_string<E, T, A> >
    {
        typedef std::basic_string<E, T, A> argument_type;
        typedef std::size_t result_type;

        std::size_t operator()( std::basic_string<E, T, A> const& val ) const
        {
            return boost::hash_value( val );
        }
    };

#endif
}

#undef BOOST_FUNCTIONAL_HASH_ROTL32

#endif // #ifndef BOOST_FUNCTIONAL_HASH_HASH_HPP
