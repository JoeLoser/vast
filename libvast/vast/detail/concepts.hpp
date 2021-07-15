//    _   _____   __________
//   | | / / _ | / __/_  __/     Visibility
//   | |/ / __ |_\ \  / /          Across
//   |___/_/ |_/___/ /_/       Space and Time
//
// SPDX-FileCopyrightText: (c) 2021 The VAST Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <iterator>

namespace vast::detail {

template <class T>
concept transparent = requires {
  typename T::is_transparent;
};

// Types that work with std::data and std::size (= containers)

template <class T>
concept container = requires(T t) {
  std::data(t);
  std::size(t);
};

} // namespace vast::detail
