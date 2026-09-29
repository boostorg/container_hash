// Copyright 2026 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/container_hash/is_described_class.hpp>
#include <boost/core/lightweight_test_trait.hpp>

struct X
{
    char const* begin() const;
    char const* end() const;
};

int main()
{
    using boost::container_hash::is_described_class;

    BOOST_TEST_TRAIT_FALSE((is_described_class<X>));

    return boost::report_errors();
}
