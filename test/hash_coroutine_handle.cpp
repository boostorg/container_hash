// Copyright 2023 Christian Mazakas.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <boost/config.hpp>

#if defined(BOOST_NO_CXX20_HDR_COROUTINE)

#include <boost/config/pragma_message.hpp>

BOOST_PRAGMA_MESSAGE(
    "hash_coroutine_handle test requires C++20 and <coroutine> support")
int main() {}
#else

#include <boost/container_hash/hash.hpp>
#include <boost/core/lightweight_test.hpp>

#include <coroutine>

struct promise;

struct task : std::coroutine_handle<promise> {
  using promise_type = ::promise;
};

struct promise {
  task get_return_object() { return {task::from_promise(*this)}; }

  void return_void() {}
  void unhandled_exception() {}
  std::suspend_always initial_suspend() { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
};

task make_task() { co_return; }

namespace {

void coroutine_handle_tests() {
  task t = make_task();
  void *p = t.address();

  {
    auto h = boost::hash<std::coroutine_handle<promise>>()(t);
    BOOST_TEST_EQ(h, boost::hash<void *>()(p));
  }

  std::coroutine_handle<> t2 = t;
  {
    auto h = boost::hash<std::coroutine_handle<>>()(t2);
    BOOST_TEST_EQ(h, boost::hash<void *>()(p));
  }

  t.destroy();
}

} // namespace

int main() {
  coroutine_handle_tests();
  return boost::report_errors();
}

#endif
