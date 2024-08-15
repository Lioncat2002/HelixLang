
#include "Utils.h"
#include <iostream>
std::nullptr_t hlx::report(SourceLocation location, std::string_view message, bool isWarning) {
    const auto &[file,line,col]=location;
    std::cerr<<file<<':'<<line<<':'<<col<<':'
    <<(isWarning? "warning: " : "error: ")<<message<<"\n";

    return nullptr;
}

