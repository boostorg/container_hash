
// Copyright 2017 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Not a test, just a small program to write out configuration info

#include <boost/container_hash/hash.hpp>
#include <iostream>

int main() {
#if defined(__cplusplus)
    std::cout << "__cplusplus: "
        << __cplusplus
        << std::endl;
#endif
    std::cout << "BOOST_HASH_HAS_STRING_VIEW: "
        << BOOST_HASH_HAS_STRING_VIEW
        << std::endl;
}
