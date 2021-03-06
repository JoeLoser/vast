/******************************************************************************
 *                    _   _____   __________                                  *
 *                   | | / / _ | / __/_  __/     Visibility                   *
 *                   | |/ / __ |_\ \  / /          Across                     *
 *                   |___/_/ |_/___/ /_/       Space and Time                 *
 *                                                                            *
 * This file is part of VAST. It is subject to the license terms in the       *
 * LICENSE file found in the top-level directory of this distribution and at  *
 * http://vast.io/license. No part of VAST, including this file, may be       *
 * copied, modified, propagated, or distributed except according to the terms *
 * contained in the LICENSE file.                                             *
 ******************************************************************************/

#define SUITE query_supervisor

#include "vast/system/query_supervisor.hpp"

#include "vast/test/fixtures/actor_system.hpp"
#include "vast/test/test.hpp"

#include "vast/concept/parseable/to.hpp"
#include "vast/concept/parseable/vast/expression.hpp"
#include "vast/expression.hpp"
#include "vast/fwd.hpp"
#include "vast/ids.hpp"

using namespace vast;

namespace {

system::evaluator_actor::behavior_type
dummy_evaluator(system::evaluator_actor::pointer self, ids x) {
  return {
    [=](const system::index_client_actor& client) {
      self->send(client, x);
      return atom::done_v;
    },
  };
}

} // namespace

FIXTURE_SCOPE(query_supervisor_tests, fixtures::deterministic_actor_system)

TEST(lookup) {
  MESSAGE("spawn supervisor, it should register itself as a worker on launch");
  auto sv
    = sys.spawn(system::query_supervisor,
                caf::actor_cast<system::query_supervisor_master_actor>(self));
  run();
  expect((caf::atom_value, system::query_supervisor_actor),
         from(sv).to(self).with(atom::worker_v, sv));
  MESSAGE("spawn evaluators");
  auto e0 = sys.spawn(dummy_evaluator, make_ids({0, 2, 4, 6, 8}));
  auto e1 = sys.spawn(dummy_evaluator, make_ids({1, 7}));
  auto e2 = sys.spawn(dummy_evaluator, make_ids({3, 5}));
  run();
  MESSAGE("fill query map and trigger supervisor");
  system::query_map qm{{uuid::random(), {e0, e1}}, {uuid::random(), {e2}}};
  self->send(sv, unbox(to<expression>("x == 42")), std::move(qm),
             caf::actor_cast<system::index_client_actor>(self));
  run();
  MESSAGE("collect results");
  bool done = false;
  ids result;
  while (!done)
    self->receive([&](const ids& x) { result |= x; },
                  [&](atom::done) { done = true; });
  CHECK_EQUAL(result, make_ids({{0, 9}}));
  MESSAGE("after completion, the supervisor should register itself again");
  expect((caf::atom_value, system::query_supervisor_actor),
         from(sv).to(self).with(atom::worker_v, sv));
}

FIXTURE_SCOPE_END()
