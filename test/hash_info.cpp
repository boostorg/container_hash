
// Copyright 2017 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// Not a test, just a small program to write out configuration info

#include <boost/container_hash/hash.hpp>
#include <iostream>
#include <algorithm>

#if defined(BOOST_MSVC)

struct msvc_version {
    std::size_t version;
    char const* description;

    bool operator<(std::size_t v) const {
        return version < v;
    }
    friend bool operator<(std::size_t v1, msvc_version const& v2) {
        return v1 < v2.version;
    }
};

void write_compiler_info() {
    // From:
    // https://en.wikipedia.org/wiki/Microsoft_Visual_C%2B%2B
    // https://blogs.msdn.microsoft.com/vcblog/2017/11/15/side-by-side-minor-version-msvc-toolsets-in-visual-studio-2017/
    msvc_version versions[] = {
        {0, "Old Visual C++"},
        {1000, "Visual C++ 4.x, VS4.0?"},
        {1100, "Visual C++ 5.0, VS97"},
        {1200, "Visual C++ 6.0, VS6.0"},
        {1300, "Visual C++ 7.0, VS.NET 2002"},
        {1310, "Visual C++ 7.1, VS.NET 2003"},
        {1400, "Visual C++ 8.0, VS2005"},
        {1500, "Visual C++ 9.0, VS2008"},
        {1600, "Visual C++ 10.0, VS2010"},
        {1700, "Visual C++ 11.0, VS2012"},
        {1800, "Visual C++ 12.0, VS2013"},
        {1900, "Visual C++ 14.00, VS2015"},
        {1910, "Visual C++ 14.10, VS2017 15.1/2"},
        {1911, "Visual C++ 14.11, VS2017 15.3/4"},
        {1912, "Visual C++ 14.12, VS2017 15.5"}
    };

    std::size_t msvc = BOOST_MSVC;
    msvc_version* v = std::upper_bound(versions,
        versions + sizeof(versions) / sizeof(*versions),
        msvc) - 1;
    std::size_t difference = BOOST_MSVC - v->version;

    std::cout << v->description << std::endl;
    if (difference) {
        std::cout << "+" << difference << std::endl;
    }
#if defined(_HAS_CXX17) && _HAS_CXX17
    std::cout << "C++17 mode" << std::endl;
#endif
}

#else

void write_compiler_info() {
}

#endif

int main() {
    write_compiler_info();

#if defined(__cplusplus)
    std::cout << "__cplusplus: "
        << __cplusplus
        << std::endl;
#endif
    std::cout << "BOOST_HASH_HAS_STRING_VIEW: "
        << BOOST_HASH_HAS_STRING_VIEW
        << std::endl;
}
