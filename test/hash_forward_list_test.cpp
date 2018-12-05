
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

#if defined(BOOST_MSVC)
#pragma warning(push)
#pragma warning(disable:4245) // signed/unsigned mismatch
#endif

namespace HASH_TEST_CAT(CONTAINER_TYPE, _tests)
{
    template <class T>
    void integer_tests(T* = 0)
    {
        typedef typename T::value_type value_type;

        const int number_of_containers = 11;
        T containers[number_of_containers];

        for(int i = 0; i < 5; ++i) {
            for(int j = 0; j < i; ++j)
                containers[i].push_front(0);
        }

        containers[5].push_front(value_type(1));
        containers[6].push_front(value_type(1));
        containers[6].push_front(value_type(1));
        containers[7].push_front(value_type(-1));
        containers[8].push_front(value_type(-1));
        containers[8].push_front(value_type(-1));
        containers[9].push_front(value_type(1));
        containers[9].push_front(value_type(-1));
        containers[10].push_front(value_type(-1));
        containers[10].push_front(value_type(1));

        BOOST_HASH_TEST_NAMESPACE::hash<T> hasher;

        for(int i2 = 0; i2 < number_of_containers; ++i2) {
            BOOST_TEST(hasher(containers[i2]) == hasher(containers[i2]));

            BOOST_TEST(hasher(containers[i2]) ==
                    BOOST_HASH_TEST_NAMESPACE::hash_value(containers[i2]));

            BOOST_TEST(hasher(containers[i2])
                    == BOOST_HASH_TEST_NAMESPACE::hash_range(
                        containers[i2].begin(), containers[i2].end()));

            for(int j2 = i2 + 1; j2 < number_of_containers; ++j2) {
                BOOST_TEST(
                        (containers[i2] == containers[j2]) ==
                        (hasher(containers[i2]) == hasher(containers[j2]))
                        );
            }
        }
    }

    void HASH_TEST_CAT(CONTAINER_TYPE, _hash_integer_tests())
    {
        integer_tests((CONTAINER_TYPE<char>*) 0);
        integer_tests((CONTAINER_TYPE<int>*) 0);
        integer_tests((CONTAINER_TYPE<unsigned long>*) 0);
        integer_tests((CONTAINER_TYPE<double>*) 0);
    }
}

#if defined(BOOST_MSVC)
#pragma warning(pop)
#endif

#undef CONTAINER_TYPE

#endif // BOOST_HASH_TEST_EXTENSIONS

int main()
{
#ifdef BOOST_HASH_TEST_EXTENSIONS
    forward_list_tests::forward_list_hash_integer_tests();
#endif

    return boost::report_errors();
}
