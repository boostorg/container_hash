// Copyright 2026 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#if defined(__clang__)
# pragma clang diagnostic ignored "-Wmismatched-tags"
#endif

#include <boost/container_hash/is_described_class.hpp>
#include <boost/core/lightweight_test_trait.hpp>

#if defined(BOOST_NO_CXX11_HDR_TUPLE)

#include <boost/config/pragma_message.hpp>

BOOST_PRAGMA_MESSAGE("Test skipped becaise <tuple> is unavailable")
int main() {}

#else

#include <tuple>

struct X
{
    int a;
    int b;
};

template<std::size_t I> int& get( X& x );
template<std::size_t I> int const& get( X const& x );

template<> int& get<0>( X& x )
{
    return x.a;
}

template<> int const& get<0>( X const& x )
{
    return x.a;
}

template<> int& get<1>( X& x )
{
    return x.b;
}

template<> int const& get<1>( X const& x )
{
    return x.b;
}

namespace std
{

template<> struct tuple_size<X>: std::integral_constant<std::size_t, 2>
{
};

} // namespace std

int main()
{
    using boost::container_hash::is_described_class;

    BOOST_TEST_TRAIT_FALSE((is_described_class<X>));

    return boost::report_errors();
}

#endif // defined(BOOST_NO_CXX11_HDR_TUPLE)
