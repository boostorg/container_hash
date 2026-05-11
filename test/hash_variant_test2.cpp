// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/container_hash/hash.hpp>
#include <boost/config.hpp>
#include <boost/config/pragma_message.hpp>

#if defined(BOOST_NO_CXX17_HDR_VARIANT)

BOOST_PRAGMA_MESSAGE( "Skipping test because BOOST_NO_CXX17_HDR_VARIANT is defined" )
int main() {}

#else

#include <boost/core/lightweight_test.hpp>
#include <variant>

struct X
{
    operator int() const { throw 5; }
};

std::variant<int, float> make_valueless_variant()
{
    std::variant<int, float> v;

    try
    {
        v.emplace<int>( X() );
    }
    catch( int )
    {
    }

    BOOST_TEST( v.valueless_by_exception() );

    return v;
}

int main()
{
    std::variant<int, float> v1, v2 = make_valueless_variant();

    BOOST_TEST_NE( (boost::hash<std::variant<int, float>>()( v1 )), (boost::hash<std::variant<int, float>>()( v2 )) );

    return boost::report_errors();
}

#endif
