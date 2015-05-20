#ifndef VAST_ACTOR_SOURCE_BRO_H
#define VAST_ACTOR_SOURCE_BRO_H

#include "vast/schema.h"
#include "vast/actor/source/line_based.h"

namespace vast {
namespace source {

/// A Bro log file source.
class bro : public line_based<bro>
{
public:
  /// Spawns a Bro source.
  /// @param is The input stream to read Bro logs from.
  bro(std::unique_ptr<io::input_stream> is);

  result<event> extract();

  schema sniff();

  void set(schema const& sch);

private:
  trial<std::string> parse_header_line(std::string const& line,
                                       std::string const& prefix);

  trial<void> parse_header();

  vast::schema schema_;
  int timestamp_field_ = -1;
  std::string separator_ = " ";
  std::string set_separator_;
  std::string empty_field_;
  std::string unset_field_;
  type type_;
};

} // namespace source
} // namespace vast

#endif
