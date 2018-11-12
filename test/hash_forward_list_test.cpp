
// Copyright 2018 Alexander Zaitsev.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "./config.hpp"

#ifdef BOOST_HASH_TEST_EXTENSIONS
#  ifdef BOOST_HASH_TEST_STD_INCLUDES
#    include <functional>
#  else
#    include <boost/container_hash/hash.hpp>
#  endif
#endif

#include <boost/core/lightweight_test.hpp>

#ifdef BOOST_HASH_TEST_EXTENSIONS

#include <forward_list>

using std::forward_list;
#define CONTAINER_TYPE forward_list
#include "./hash_sequence_test.hpp"

#endif // BOOST_HASH_TEST_EXTENSIONS

int main()
{
#ifdef BOOST_HASH_TEST_EXTENSIONS
    forward_list_tests::forward_list_hash_integer_tests();
#endif

    return boost::report_errors();
}
