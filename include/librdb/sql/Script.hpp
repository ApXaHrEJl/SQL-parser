#pragma once

#include <librdb/sql/Statements.hpp>
#include <vector>

namespace rdb::sql {

struct Script {
  std::vector<StatementPtr> statements_;
};

}  // namespace rdb::sql
