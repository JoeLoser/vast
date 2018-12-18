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

#include "vast/column_index.hpp"

#include "vast/expression_visitors.hpp"
#include "vast/load.hpp"
#include "vast/logger.hpp"
#include "vast/save.hpp"
#include "vast/table_slice.hpp"

namespace vast {

// -- free functions -----------------------------------------------------------

caf::expected<column_index_ptr> make_column_index(caf::actor_system& sys,
                                                  path filename,
                                                  type column_type,
                                                  size_t column) {
<<<<<<< HEAD
  struct impl : column_index {
    impl(caf::actor_system& sys, path&& fname, type&& ctype, size_t col)
      : column_index(sys, std::move(ctype), std::move(fname)),
        col_(col) {
        // nop
    }

    void add(const table_slice_ptr& x) override {
      VAST_TRACE(VAST_ARG(x));
      if (!has_skip_attribute_)
        x->append_column_to_index(col_, *idx_);
    }

    size_t col_;
  };
  return init_res(std::make_unique<impl>(sys, std::move(filename),
                                         std::move(column_type), column));
=======
  auto result = std::make_unique<column_index>(sys, std::move(column_type),
                                               std::move(filename), column);
  if (auto err = result->init())
    return err;
  return result;
>>>>>>> De-virtualize the column index
}

// -- constructors, destructors, and assignment operators ----------------------

column_index::column_index(caf::actor_system& sys, type index_type,
                           path filename, size_t column)
  : col_(column),
    has_skip_attribute_(vast::has_skip_attribute(index_type)),
    index_type_(std::move(index_type)),
    filename_(std::move(filename)),
    sys_(sys) {
  // nop
}

column_index::~column_index() {
  flush_to_disk();
}

// -- persistence --------------------------------------------------------------

caf::error column_index::init() {
  VAST_TRACE("");
  // Materialize the index when encountering persistent state.
  if (exists(filename_)) {
    detail::value_index_inspect_helper tmp{index_type_, idx_};
    if (auto err = load(sys_, filename_, last_flush_, tmp)) {
      VAST_ERROR(this, "failed to load value index from disk", sys_.render(err));
      return err;
    } else {
      VAST_DEBUG(this, "loaded value index with offset", idx_->offset());
    }
    return caf::none;
  }
  // Otherwise construct a new one.
  idx_ = value_index::make(index_type_);
  if (idx_ == nullptr) {
    VAST_ERROR(this, "failed to construct index");
    return make_error(ec::unspecified, "failed to construct index");
  }
  VAST_DEBUG(this, "constructed new value index");
  return caf::none;
}

caf::error column_index::flush_to_disk() {
  VAST_TRACE("");
  // The value index is null if and only if `init()` failed.
  if (idx_ == nullptr || !dirty())
    return caf::none;
  // Check whether there's something to write.
  auto offset = idx_->offset();
  VAST_DEBUG(this, "flushes index (" << (offset - last_flush_) << '/' << offset,
             "new/total bits)");
  last_flush_ = offset;
  detail::value_index_inspect_helper tmp{index_type_, idx_};
  return save(sys_, filename_, last_flush_, tmp);
}

// -- properties -------------------------------------------------------------

void column_index::add(const table_slice_ptr& x) {
  VAST_TRACE(VAST_ARG(x));
  if (has_skip_attribute_)
    return;
  auto offset = x->offset();
  for (table_slice::size_type row = 0; row < x->rows(); ++row)
    idx_->append(x->at(row, col_), offset + row);
}

caf::expected<bitmap> column_index::lookup(relational_operator op,
                                           data_view rhs) {
  VAST_TRACE(VAST_ARG(op), VAST_ARG(rhs));
  VAST_ASSERT(idx_ != nullptr);
  auto result = idx_->lookup(op, rhs);
  VAST_DEBUG(this, VAST_ARG(result));
  return result;
}

bool column_index::dirty() const noexcept {
  VAST_ASSERT(idx_ != nullptr);
  return idx_->offset() != last_flush_;
}

} // namespace vast
