#include <filesystem>

namespace hlx{
    struct CompilerOptions{
        std::filesystem::path source;
        std::filesystem::path output;
        bool displayHelp=false;
        bool astDump=false;
        bool resDump=false;
        bool llvmDump=false;
        bool cfgDump=false;
    };
    CompilerOptions parseArguments(int argc,const char **argv);
    void displayHelp();
    [[noreturn]] void error(std::string_view msg);
}