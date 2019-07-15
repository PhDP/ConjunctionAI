#ifndef CJ_MATCH_ARG_HH_
#define CJ_MATCH_ARG_HH_

#include "cj/common.hh"

namespace cj {

  inline auto match_arg(int argc, char** argv, char const* opt) -> pair<int, string> {
    auto p = pair<int, string>{-1, ""};
    auto const len = std::strlen(opt);

    for (auto i = 0u; i < argc; ++i) {
      int j = 0, offset = argv[i][0] && argv[i][0] == '-';
      offset += (offset && argv[i][1] && argv[i][1] == '-');
      while (j < len && argv[i][j + offset] && opt[j] == argv[i][j + offset]) {
        ++j;
      }
      if (j == len) {
        if (argv[i][j + offset] == '=') {
          p.second = string(&argv[i][j + offset + 1]);
        }
        p.first = i;
        return p;
      }
    }
    return p;
  }

} /* end namespace cj */

#endif
