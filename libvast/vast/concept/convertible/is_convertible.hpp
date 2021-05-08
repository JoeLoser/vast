//    _   _____   __________
//   | | / / _ | / __/_  __/     Visibility
//   | |/ / __ |_\ \  / /          Across
//   |___/_/ |_/___/ /_/       Space and Time
//
// SPDX-FileCopyrightText: (c) 2016 The VAST Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <experimental/type_traits>

namespace vast {
namespace detail {

template <class From, class To>
using is_convertible
  = decltype(convert(*std::declval<From>(), *std::declval<To>()));

} // namespace detail

/// Type trait that checks whether a type is convertible to another.
template <class From, class To>
using is_convertible = std::experimental::is_detected<
  detail::is_convertible, std::add_pointer_t<From>, std::add_pointer_t<To>>;

template <class From, class To>
inline constexpr bool is_convertible_v = is_convertible<From, To>::value;

} // namespace vast

