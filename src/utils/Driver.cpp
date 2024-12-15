#include "Driver.h"
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace hlx {
CompilerOptions parseArguments(int argc, const char **argv) {
  CompilerOptions options;

  int idx = 1;
  while (idx < argc) {
    std::string_view arg = argv[idx];

    if (arg[0] != '-') {
      if (!options.source.empty()) {
        error("unexpected argument '" + std::string(arg) + '\'');
      }
      options.source = arg;
    } else {
      if (arg == "-h")
        options.displayHelp = true;
      else if (arg == "-o")
        options.output = ++idx >= argc ? "" : argv[idx];
      else if (arg == "-ast-dump")
        options.astDump = true;
      else if (arg=="-res-dump")
        options.resDump=true;
      else if (arg == "-llvm-dump")
        options.llvmDump = true;
      else if (arg == "-cfg-dump")
        options.cfgDump = true;
      else
        error("unexpected option '" + std::string(arg) + '\'');
    }
    ++idx;
  }

  return options;
}

[[noreturn]] void error(std::string_view msg) {
  std::cerr << "error: " << msg << '\n';
  std::exit(1);
}

void displayHelp() {
  std::cout << "Usage:\n"
            << "  compiler [options] <source_file>\n\n"
            << "Options:\n"
            << "  -h           display this message\n"
            << "  -o <file>    write executable to <file>\n"
            << "  -ast-dump    print the abstract syntax tree\n"
            << "  -res-dump    print the resolved syntax tree\n"
            << "  -llvm-dump   print the llvm module\n";
}
} // namespace hlx