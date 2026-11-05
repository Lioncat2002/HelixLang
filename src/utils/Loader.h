#pragma once
#include "../core/ast/Ast.h"
#include <deque>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace hlx {

class Loader {
  std::deque<std::string> paths;
  std::unordered_set<std::string> visited;
  std::vector<std::unique_ptr<FunctionDecl>> functions;
  bool success = true;

private:
  void load(const std::filesystem::path &path,
            const SourceLocation *importedAt);

public:
  std::pair<std::vector<std::unique_ptr<FunctionDecl>>, bool>
  loadProgram(const std::filesystem::path &entry);
};

} // namespace hlx