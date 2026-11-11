// Copyright 2026 Peter Dimov
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/container_hash/is_contiguous_range.hpp>
#include <boost/core/lightweight_test_trait.hpp>
#include <boost/config.hpp>
#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)
# include <string_view>
#endif
#if !defined(BOOST_NO_CXX20_HDR_SPAN)
# include <span>
#endif

struct X
{
};

int main()
{
    using boost::container_hash::is_contiguous_range;

#if !defined(BOOST_NO_CXX17_HDR_STRING_VIEW)

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::string_view >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::string_view const >));

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::wstring_view >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::wstring_view const >));

#endif

#if !defined(BOOST_NO_CXX20_HDR_SPAN)

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X> >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X> const >));

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X const> >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X const> const >));

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X, 4> >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X, 4> const >));

    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X const, 4> >));
    BOOST_TEST_TRAIT_TRUE((is_contiguous_range< std::span<X const, 4> const >));

#endif

    return boost::report_errors();
}
