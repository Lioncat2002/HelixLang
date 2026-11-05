#include "Loader.h"
#include "../core/lexer/Lexer.h"
#include "../core/parser/Parser.h"
#include "Utils.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

namespace fs = std::filesystem;

void hlx::Loader::load(const std::filesystem::path &path,
                       const SourceLocation *importedAt) {

  if (!visited.insert(fs::weakly_canonical(path).string()).second)
    return;

  std::ifstream file(path);
  if (!file) {
    std::string message = "failed to open '" + path.string() + '\'';
    if (importedAt)
      report(*importedAt, message);
    else
      std::cerr << "error: " << message << '\n';
    success = false;
    return;
  }

  std::stringstream buffer;
  buffer << file.rdbuf();

  paths.push_back(path.string());
  SourceFile source{paths.back(), buffer.str()};

  Lexer lexer(source);
  Parser parser(lexer);

  auto [parsed, parsedOk] = parser.parseSourceFile();

  if (!parsedOk)
    success = false;

  for (auto &fn : parsed)
    functions.emplace_back(std::move(fn));

  // Import paths are relative to the file containing the import.
  for (const Import &import : parser.imports)
    load(path.parent_path() / import.path, &import.location);
}

std::pair<std::vector<std::unique_ptr<hlx::FunctionDecl>>, bool>
hlx::Loader::loadProgram(const fs::path &entry) {
  load(entry, nullptr);
  return {std::move(functions), success};
}