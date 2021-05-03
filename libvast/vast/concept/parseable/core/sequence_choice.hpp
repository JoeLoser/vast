//    _   _____   __________
//   | | / / _ | / __/_  __/     Visibility
//   | |/ / __ |_\ \  / /          Across
//   |___/_/ |_/___/ /_/       Space and Time
//
// SPDX-FileCopyrightText: (c) 2016 The VAST Contributors
// SPDX-License-Identifier: BSD-3-Clause

#pragma once

#include <tuple>
#include <optional>
#include <type_traits>

#include "vast/concept/parseable/core/parser.hpp"
#include "vast/concept/parseable/core/optional.hpp"

namespace vast {

// (LHS >> ~RHS) | RHS
template <class Lhs, class Rhs>
class sequence_choice_parser : public parser<sequence_choice_parser<Lhs, Rhs>> {
public:
  using lhs_type = Lhs;
  using rhs_type = Rhs;
  using lhs_attribute = typename Lhs::attribute;
  using rhs_attribute = typename Rhs::attribute;

  // LHS = unused && RHS = unused  =>  unused
  // LHS = T && RHS = unused       =>  std::optional<LHS>
  // LHS = unused && RHS = T       =>  std::optional<RHS>
  // LHS = T && RHS = U            =>  std:tuple<std::optional<LHS>, std::optional<RHS>>
  using attribute = std::conditional_t<
    std::is_same<lhs_attribute, unused_type>{}
      && std::is_same<rhs_attribute, unused_type>{},
    unused_type,
    std::conditional_t<
      std::is_same<rhs_attribute, unused_type>{}, std::optional<lhs_attribute>,
      std::conditional_t<
        std::is_same<lhs_attribute, unused_type>{}, std::optional<rhs_attribute>,
        std::tuple<std::optional<lhs_attribute>, std::optional<rhs_attribute>>>>>;

  sequence_choice_parser(Lhs lhs, Rhs rhs)
    : lhs_{std::move(lhs)}, rhs_{rhs}, rhs_opt_{std::move(rhs)} {
  }

  template <class Iterator, class Attribute>
  bool parse(Iterator& f, const Iterator& l, Attribute& a) const {
    std::optional<rhs_attribute> rhs_attr;
    if (lhs_(f, l, left_attr(a)) && rhs_opt_(f, l, rhs_attr)) {
      right_attr(a) = std::move(rhs_attr);
      return true;
    }
    return rhs_(f, l, right_attr(a));
  }

private:
  template <
    class Attribute,
    class L = lhs_attribute,
    class R = rhs_attribute
  >
  static auto left_attr(Attribute&)
    -> std::enable_if_t<std::is_same<L, unused_type>{}, unused_type&> {
    return unused;
  }

  template <class Attribute, class L = lhs_attribute, class R = rhs_attribute>
  static auto left_attr(Attribute& a) -> std::enable_if_t<
    std::conjunction_v<std::negation<std::is_same<L, unused_type>>,
                       std::is_same<R, unused_type>>,
    std::optional<L>&> {
    return a;
  }

  template <class... Ts, class L = lhs_attribute, class R = rhs_attribute>
  static auto left_attr(std::tuple<Ts...>& t) -> std::enable_if_t<
    std::negation_v<std::disjunction<std::is_same<L, unused_type>,
                                     std::is_same<R, unused_type>>>,
    std::optional<L>&> {
    return std::get<0>(t);
  }

  template <
    class Attribute,
    class L = lhs_attribute,
    class R = rhs_attribute
  >
  static auto right_attr(Attribute&)
    -> std::enable_if_t<std::is_same<R, unused_type>{}, unused_type&> {
    return unused;
  }

  template <class Attribute, class L = lhs_attribute, class R = rhs_attribute>
  static auto right_attr(Attribute& a) -> std::enable_if_t<
    std::conjunction_v<std::is_same<L, unused_type>,
                       std::negation<std::is_same<R, unused_type>>>,
    std::optional<R>&> {
    return a;
  }

  template <class... Ts, class L = lhs_attribute, class R = rhs_attribute>
  static auto right_attr(std::tuple<Ts...>& t) -> std::enable_if_t<
    std::negation_v<std::disjunction<std::is_same<L, unused_type>,
                                     std::is_same<R, unused_type>>>,
    std::optional<R>&> {
    return std::get<1>(t);
  }

  lhs_type lhs_;
  rhs_type rhs_;
  optional_parser<rhs_type> rhs_opt_;
};

} // namespace vast

