#pragma once
#include <string>
#include <string_view>
#define varOrReturn(var, init)               \
  auto var = (init);                         \
  if (!var)                                  \
    return nullptr

#define matchOrReturn(tok, msg)              \
  if (nextToken.kind != tok)                 \
    return report(nextToken.location, msg);

namespace hlx{
    struct SourceLocation{
        std::string_view filepath;
        int line;
        int col;
    };
    struct SourceFile{
        std::string_view path;
        std::string buffer="";
    };
    std::nullptr_t report(SourceLocation location, std::string_view message,
                          bool isWarning = false);
}
