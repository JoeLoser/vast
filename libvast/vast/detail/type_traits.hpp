//    _   _____   __________
//   | | / / _ | / __/_  __/     Visibility
//   | |/ / __ |_\ \  / /          Across
//   |___/_/ |_/___/ /_/       Space and Time
//
// SPDX-FileCopyrightText: (c) 2016 The VAST Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <iterator>
#include <streambuf>
#include <string>
#include <optional>
#include <tuple>
#include <type_traits>
#include <vector>

#include <experimental/type_traits>

namespace vast::detail {

// -- is_* --------------------------------------------------------------------

/// Checks whether a type is a std::tuple.
template <class>
struct is_tuple : std::false_type {};

template <class... Ts>
struct is_tuple<std::tuple<Ts...>> : std::true_type {};

template <class T>
constexpr bool is_tuple_v = is_tuple<T>::value;

/// Checks whether a type derives from `basic_streambuf<Char>`.
template <class T, class U = void>
struct is_streambuf : std::false_type {};

template <class T>
struct is_streambuf<T, std::enable_if_t<std::is_base_of_v<
                         std::basic_streambuf<typename T::char_type>, T>>>
  : std::true_type {};

template <class T>
constexpr bool is_streambuf_v = is_streambuf<T>::value;

// std::pair<T, U>

template <class T>
struct is_pair : std::false_type {};

template <class T, class U>
struct is_pair<std::pair<T, U>> : std::true_type {};

template <class T>
constexpr bool is_pair_v = is_pair<T>::value;

// deref - generic version of std::remove_pointer

template <class T>
using deref_t_helper = decltype(*std::declval<T>());

template <class T>
using deref_t = std::experimental::detected_t<deref_t_helper, T>;

// Types that work with std::data and std::size (= containers)

template <class T>
using std_data_t = decltype(std::data(std::declval<T>()));

template <class T>
using std_size_t = decltype(std::size(std::declval<T>()));

template <class T, class = void>
struct is_container : std::false_type {};

template <class T>
struct is_container<
  T, std::enable_if_t<
       std::experimental::is_detected_v<
         std_data_t, T> && std::experimental::is_detected_v<std_size_t, T>>>
  : std::true_type {};

template <class T>
inline constexpr bool is_container_v = is_container<T>::value;

/// Contiguous byte buffers

template <class T, class = void>
struct is_byte_container : std::false_type {};

template <class T>
struct is_byte_container<
  T,
  std::enable_if_t<
    std::experimental::is_detected_v<
      std_data_t,
      T> && std::experimental::is_detected_v<std_size_t, T> && sizeof(deref_t<std_data_t<T>>) == 1>>
  : std::true_type {};

template <class T>
inline constexpr bool is_byte_container_v = is_byte_container<T>::value;

// -- SFINAE helpers ---------------------------------------------------------
// http://bit.ly/uref-copy.

template <class A, class B>
using is_same_or_derived = std::is_base_of<A, std::remove_reference_t<B>>;

template <class A, class B>
using is_same_or_derived_t = typename is_same_or_derived<A, B>::type;

template <class A, class B>
inline constexpr bool is_same_or_derived_v = is_same_or_derived<A, B>::value;

// -- traits -----------------------------------------------------------------

template <class T, class... Ts>
inline constexpr bool is_any_v = std::disjunction_v<std::is_same<T, Ts>...>;

template <class T, class... Ts>
inline constexpr bool are_same_v = std::conjunction_v<std::is_same<T, Ts>...>;

// Utility for usage in `static_assert`. For example:
//
//   template <class T>
//   void f() {
//     if constexpr (is_same_v<T, int>)
//       ...
//     else
//       static_assert(always_false_v<T>, "error message");
//   }
//
template <class>
struct always_false : std::false_type {};

template <class T>
inline constexpr bool always_false_v = always_false<T>::value;

// -- tuple ------------------------------------------------------------------

// Wraps a type into a tuple if it is not already a tuple.
template <class T>
using tuple_wrap = std::conditional_t<is_tuple_v<T>, T, std::tuple<T>>;

// -- optional ---------------------------------------------------------------

template <class T>
struct remove_optional {
  using type = T;
};

template <class T>
struct remove_optional<std::optional<T>> {
  using type = T;
};

template <class T>
using remove_optional_t = typename remove_optional<T>::type;

// -- checks for stringification functions -----------------------------------

template <typename T>
using to_string_t = decltype(to_string(std::declval<T>()));

template <typename T>
inline constexpr bool has_to_string
  = std::experimental::is_detected_v<to_string_t, T>;

template <typename T>
using name_getter_t =
  typename std::is_convertible<decltype(std::declval<T>().name()),
                               std::string_view>::type;

template <typename T>
inline constexpr bool has_name_getter
  = std::experimental::is_detected_v<name_getter_t, T>;

template <typename T>
using name_member_t =
  typename std::is_convertible<decltype(std::declval<T>().name),
                               std::string_view>::type;

template <typename T>
inline constexpr bool has_name_member
  = std::experimental::is_detected_v<name_member_t, T>;

// -- compile time computation of sum -----------------------------------------
template <auto... Values>
constexpr auto sum = (0 + ... + Values);

} // namespace vast::detail
