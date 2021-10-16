
// Copyright 2005-2014 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

//  Based on Peter Dimov's proposal
//  http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2005/n1756.pdf
//  issue 6.18.
//
//  This also contains public domain code from MurmurHash. From the
//  MurmurHash header:

// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#if !defined(BOOST_FUNCTIONAL_HASH_HASH_HPP)
#define BOOST_FUNCTIONAL_HASH_HASH_HPP

#include <boost/container_hash/hash_fwd.hpp>
#include <boost/container_hash/detail/hash_float.hpp>
#include <boost/type_traits/is_enum.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_floating_point.hpp>
#include <boost/type_traits/make_unsigned.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/limits.hpp>
#include <boost/cstdint.hpp>
#include <iterator>
#include <string>
#include <climits>

#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
#include <typeindex>
#endif

#if !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)
#include <system_error>
#endif

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
#include <string_view>
#endif

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)
#include <optional>
#endif

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)
#include <variant>
#endif

#if defined(BOOST_MSVC)
#pragma warning(push)

#if BOOST_MSVC >= 1400
#pragma warning(disable:6295) // Ill-defined for-loop : 'unsigned int' values
                              // are always of range '0' to '4294967295'.
                              // Loop executes infinitely.
#endif

#endif

#if defined(_MSC_VER)
#   define BOOST_FUNCTIONAL_HASH_ROTL32(x, r) _rotl(x,r)
#else
#   define BOOST_FUNCTIONAL_HASH_ROTL32(x, r) (x << r) | (x >> (32 - r))
#endif

namespace boost
{
    namespace hash_detail
    {
        template <typename T>
        struct hash_base
        {
            typedef T argument_type;
            typedef std::size_t result_type;
        };
    }

    template <typename T>
    typename boost::enable_if<boost::is_integral<T>, std::size_t>::type
        hash_value(T);

    template <typename T>
    typename boost::enable_if<boost::is_enum<T>, std::size_t>::type
        hash_value(T);

    template <typename T>
    typename boost::enable_if<boost::is_floating_point<T>, std::size_t>::type
        hash_value(T);

    template <class T> std::size_t hash_value(T* const&);

    template< class T, unsigned N >
    std::size_t hash_value(const T (&x)[N]);

    template< class T, unsigned N >
    std::size_t hash_value(T (&x)[N]);

    template <class Ch, class A>
    std::size_t hash_value(
        std::basic_string<Ch, std::char_traits<Ch>, A> const&);

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    template <class Ch>
    std::size_t hash_value(
        std::basic_string_view<Ch, std::char_traits<Ch> > const&);
#endif

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)
    template <typename T>
    std::size_t hash_value(std::optional<T> const&);
#endif

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)
    std::size_t hash_value(std::monostate);
    template <typename... Types>
    std::size_t hash_value(std::variant<Types...> const&);
#endif

#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
    std::size_t hash_value(std::type_index);
#endif

#if !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)
    std::size_t hash_value(std::error_code const&);
    std::size_t hash_value(std::error_condition const&);
#endif

    // Implementation

    namespace hash_detail
    {
        template<class T, bool bigger_than_size_t, bool size_t_64> struct hash_integral_impl;

        template<class T, bool size_t_64> struct hash_integral_impl<T, false, size_t_64>
        {
            static std::size_t fn( T v )
            {
                return static_cast<std::size_t>( v );
            }
        };

        template<class T> struct hash_integral_impl<T, true, false>
        {
            static std::size_t fn( T v )
            {
                // 4294967291 = 2^32-5, biggest prime under 2^32
                return static_cast<std::size_t>( static_cast<typename boost::make_unsigned<T>::type>( v ) % 4294967291 );
            }
        };

        template<class T> struct hash_integral_impl<T, true, true>
        {
            static std::size_t fn( T v )
            {
                // 18446744073709551557ULL = 2^64-59, biggest prime under 2^64
                return static_cast<std::size_t>( static_cast<typename boost::make_unsigned<T>::type>( v ) % 18446744073709551557ULL );
            }
        };

        template<int Bits> struct hash_combine_impl
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

    template <typename T>
    typename boost::enable_if<boost::is_integral<T>, std::size_t>::type
        hash_value(T v)
    {
        return hash_detail::hash_integral_impl<T, (sizeof(T) > sizeof(std::size_t)), (sizeof(std::size_t) * CHAR_BIT >= 64)>::fn( v );
    }

    template <typename T>
    typename boost::enable_if<boost::is_enum<T>, std::size_t>::type
        hash_value(T v)
    {
        return static_cast<std::size_t>(v);
    }

    template <typename T>
    typename boost::enable_if<boost::is_floating_point<T>, std::size_t>::type
        hash_value(T v)
    {
        return boost::hash_detail::float_hash_value(v);
    }

    // Implementation by Alberto Barbati and Dave Harris.
    template <class T> std::size_t hash_value(T* const& v)
    {
        std::size_t x = static_cast<std::size_t>(
           reinterpret_cast<boost::uintptr_t>(v));
        return x + (x >> 3);
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

    template <class It>
    inline std::size_t hash_range(It first, It last)
    {
        std::size_t seed = 0;

        for(; first != last; ++first)
        {
            hash_combine<typename std::iterator_traits<It>::value_type>(seed, *first);
        }

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

#if BOOST_WORKAROUND(BOOST_BORLANDC, BOOST_TESTED_AT(0x551))
    template <class T>
    inline std::size_t hash_range(T* first, T* last)
    {
        std::size_t seed = 0;

        for(; first != last; ++first)
        {
            boost::hash<T> hasher;
            seed ^= hasher(*first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }

        return seed;
    }

    template <class T>
    inline void hash_range(std::size_t& seed, T* first, T* last)
    {
        for(; first != last; ++first)
        {
            boost::hash<T> hasher;
            seed ^= hasher(*first) + 0x9e3779b9 + (seed<<6) + (seed>>2);
        }
    }
#endif

    template< class T, unsigned N >
    inline std::size_t hash_value(const T (&x)[N])
    {
        return hash_range(x, x + N);
    }

    template< class T, unsigned N >
    inline std::size_t hash_value(T (&x)[N])
    {
        return hash_range(x, x + N);
    }

    template <class Ch, class A>
    inline std::size_t hash_value(
        std::basic_string<Ch, std::char_traits<Ch>, A> const& v)
    {
        return hash_range(v.begin(), v.end());
    }

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    template <class Ch>
    inline std::size_t hash_value(
        std::basic_string_view<Ch, std::char_traits<Ch> > const& v)
    {
        return hash_range(v.begin(), v.end());
    }
#endif

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)
    template <typename T>
    inline std::size_t hash_value(std::optional<T> const& v) {
        if (!v) {
            // Arbitray value for empty optional.
            return 0x12345678;
        } else {
            boost::hash<T> hf;
            return hf(*v);
        }
    }
#endif

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)
    inline std::size_t hash_value(std::monostate) {
        return 0x87654321;
    }

    template <typename... Types>
    inline std::size_t hash_value(std::variant<Types...> const& v) {
        std::size_t seed = 0;
        hash_combine(seed, v.index());
        std::visit([&seed](auto&& x) { hash_combine(seed, x); }, v);
        return seed;
    }
#endif


#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
    inline std::size_t hash_value(std::type_index v)
    {
        return v.hash_code();
    }
#endif

#if !defined(BOOST_NO_CXX11_HDR_SYSTEM_ERROR)
    inline std::size_t hash_value(std::error_code const& v) {
        std::size_t seed = 0;
        hash_combine(seed, v.value());
        hash_combine(seed, &v.category());
        return seed;
    }

    inline std::size_t hash_value(std::error_condition const& v) {
        std::size_t seed = 0;
        hash_combine(seed, v.value());
        hash_combine(seed, &v.category());
        return seed;
    }
#endif

    //
    // boost::hash
    //

    // Define the specializations required by the standard. The general purpose
    // boost::hash is defined later in extensions.hpp if
    // BOOST_HASH_NO_EXTENSIONS is not defined.

    // BOOST_HASH_SPECIALIZE - define a specialization for a type which is
    // passed by copy.
    //
    // BOOST_HASH_SPECIALIZE_REF - define a specialization for a type which is
    // passed by const reference.
    //
    // These are undefined later.

#define BOOST_HASH_SPECIALIZE(type) \
    template <> struct hash<type> \
         : public boost::hash_detail::hash_base<type> \
    { \
        std::size_t operator()(type v) const \
        { \
            return boost::hash_value(v); \
        } \
    };

#define BOOST_HASH_SPECIALIZE_REF(type) \
    template <> struct hash<type> \
         : public boost::hash_detail::hash_base<type> \
    { \
        std::size_t operator()(type const& v) const \
        { \
            return boost::hash_value(v); \
        } \
    };

#define BOOST_HASH_SPECIALIZE_TEMPLATE_REF(type) \
    struct hash<type> \
         : public boost::hash_detail::hash_base<type> \
    { \
        std::size_t operator()(type const& v) const \
        { \
            return boost::hash_value(v); \
        } \
    };

    BOOST_HASH_SPECIALIZE(bool)
    BOOST_HASH_SPECIALIZE(char)
    BOOST_HASH_SPECIALIZE(signed char)
    BOOST_HASH_SPECIALIZE(unsigned char)
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_HASH_SPECIALIZE(wchar_t)
#endif
#if !defined(BOOST_NO_CXX11_CHAR16_T)
    BOOST_HASH_SPECIALIZE(char16_t)
#endif
#if !defined(BOOST_NO_CXX11_CHAR32_T)
    BOOST_HASH_SPECIALIZE(char32_t)
#endif
    BOOST_HASH_SPECIALIZE(short)
    BOOST_HASH_SPECIALIZE(unsigned short)
    BOOST_HASH_SPECIALIZE(int)
    BOOST_HASH_SPECIALIZE(unsigned int)
    BOOST_HASH_SPECIALIZE(long)
    BOOST_HASH_SPECIALIZE(unsigned long)

    BOOST_HASH_SPECIALIZE(float)
    BOOST_HASH_SPECIALIZE(double)
    BOOST_HASH_SPECIALIZE(long double)

    BOOST_HASH_SPECIALIZE_REF(std::string)
#if !defined(BOOST_NO_STD_WSTRING) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_HASH_SPECIALIZE_REF(std::wstring)
#endif
#if !defined(BOOST_NO_CXX11_CHAR16_T)
    BOOST_HASH_SPECIALIZE_REF(std::basic_string<char16_t>)
#endif
#if !defined(BOOST_NO_CXX11_CHAR32_T)
    BOOST_HASH_SPECIALIZE_REF(std::basic_string<char32_t>)
#endif

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
    BOOST_HASH_SPECIALIZE_REF(std::string_view)
#   if !defined(BOOST_NO_STD_WSTRING) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_HASH_SPECIALIZE_REF(std::wstring_view)
#   endif
#   if !defined(BOOST_NO_CXX11_CHAR16_T)
    BOOST_HASH_SPECIALIZE_REF(std::basic_string_view<char16_t>)
#   endif
#   if !defined(BOOST_NO_CXX11_CHAR32_T)
    BOOST_HASH_SPECIALIZE_REF(std::basic_string_view<char32_t>)
#   endif
#endif

#if !defined(BOOST_NO_LONG_LONG)
    BOOST_HASH_SPECIALIZE(boost::long_long_type)
    BOOST_HASH_SPECIALIZE(boost::ulong_long_type)
#endif

#if defined(BOOST_HAS_INT128)
    BOOST_HASH_SPECIALIZE(boost::int128_type)
    BOOST_HASH_SPECIALIZE(boost::uint128_type)
#endif

#if !defined(BOOST_NO_CXX17_HDR_OPTIONAL)
    template <typename T>
    BOOST_HASH_SPECIALIZE_TEMPLATE_REF(std::optional<T>)
#endif

#if !defined(BOOST_NO_CXX17_HDR_VARIANT)
    template <typename... T>
    BOOST_HASH_SPECIALIZE_TEMPLATE_REF(std::variant<T...>)
    BOOST_HASH_SPECIALIZE(std::monostate)
#endif

#if !defined(BOOST_NO_CXX11_HDR_TYPEINDEX)
    BOOST_HASH_SPECIALIZE(std::type_index)
#endif

#undef BOOST_HASH_SPECIALIZE
#undef BOOST_HASH_SPECIALIZE_REF
#undef BOOST_HASH_SPECIALIZE_TEMPLATE_REF

// Specializing boost::hash for pointers.

    template <class T>
    struct hash<T*>
        : public boost::hash_detail::hash_base<T*>
    {
        std::size_t operator()(T* v) const
        {
#if !BOOST_WORKAROUND(__SUNPRO_CC, <= 0x590)
            return boost::hash_value(v);
#else
            std::size_t x = static_cast<std::size_t>(
                reinterpret_cast<std::ptrdiff_t>(v));

            return x + (x >> 3);
#endif
        }
    };
}

#undef BOOST_FUNCTIONAL_HASH_ROTL32

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#endif // BOOST_FUNCTIONAL_HASH_HASH_HPP

// Include this outside of the include guards in case the file is included
// twice - once with BOOST_HASH_NO_EXTENSIONS defined, and then with it
// undefined.

#if !defined(BOOST_HASH_NO_EXTENSIONS) \
    && !defined(BOOST_FUNCTIONAL_HASH_EXTENSIONS_HPP)
#include <boost/container_hash/extensions.hpp>
#endif
