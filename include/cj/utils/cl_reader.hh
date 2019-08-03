/**
 * \file   cl_reader.hh
 * \brief  Helpers to read command line arguments.
 */
#ifndef CJ_CL_READER_HH_
#define CJ_CL_READER_HH_

#include "cj/common.hh"
#include "cj/utils/detail/match_arg.hh"

namespace cj {

  /**
   * \brief Gets the value for a given command-line option.
   *
   * For example, given "-foo=42 --bar baz=0.6", this function will return:
   *
   *   std::optional<uint32_t>{42} for get_arg<uint32_t>(argc, argv, "foo")
   *   std::nullopt for get_arg<int>(argc, argv, "baaz")
   *   std::optional<double>{0.6} for get_arg<double>(argc, argv, "baz")
   *   std::optional<string>{""} for get_arg<string>(argc, argv, "bar")
   *
   * \param argc   Number of command-line arguments.
   * \param argv   The command-line arguments.
   * \param opt    The option.
   * \return       Value for the option, std::nullopt if the option wasn't found.
   */
  template<typename T>
  auto get_arg(int argc, char **argv, char const* opt) -> std::optional<T> {
    auto const res = match_arg(argc, argv, opt);
    return res.first == -1? std::nullopt : std::optional<T>{boost::lexical_cast<T>(res.second)};
  }

  /**
   * \brief Gets the value for a given command-line option or a default value if not found.
   *
   * For example, given "-foo=42 --bar baz=0.6", this function will return:
   *
   *   42 for get_arg<uint32_t>(argc, argv, "foo", 5)
   *   14 for get_arg<int>(argc, argv, "baaz", 14)
   *   0.6 for get_arg<double>(argc, argv, "baz", 14.1)
   *   "" for get_arg<string>(argc, argv, "bar", "hello")
   *
   * \param argc   Number of command-line arguments.
   * \param argv   The command-line arguments.
   * \param opt    The option.
   * \param def    Default value if argument not found.
   * \return       Value for the option, std::nullopt if the option wasn't found.
   */
  template<typename T>
  auto get_arg(int argc, char** argv, char const* opt, T const& def) -> T {
    auto const res = match_arg(argc, argv, opt);
    return res.first == -1? def : boost::lexical_cast<T>(res.second);
  }

} /* end namespace cj */

#endif
