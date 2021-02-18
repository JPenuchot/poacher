/// -ftime-trace wrapper for clang.

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

/// Wrapper for a given clang command.

int get_timetrace_file(std::filesystem::path const out, std::string const &cmd,
                       std::filesystem::path obj_path) {
  namespace fs = std::filesystem;

  { // Run command
    std::ostringstream new_cmd;

    new_cmd << cmd << " -ftime-trace";
    std::cout << new_cmd.str() << '\n';
    std::system(new_cmd.str().c_str());
  } // namespace std::filesystem;

  fs::copy_file(obj_path.replace_extension(".json"), out,
                fs::copy_options::overwrite_existing);

  return 0;
}

constexpr int exec_id = 0;
constexpr int path_id = 1;
constexpr int cmd_start_id = 2;

// Some parameters
int main(int argc, char const *argv[]) {
  namespace fs = std::filesystem;

  if (argc < 3) {
    std::cout << "Usage: " << argv[exec_id]
              << "time_trace_export_path.json [COMMAND]";
    return 1;
  }

  // Building path and finding obj_path
  std::ostringstream cmd_builder;
  fs::path obj_path;

  cmd_builder << argv[cmd_start_id];
  for (auto beg = &argv[cmd_start_id + 1], end = &argv[argc]; beg < end;
       beg++) {
    // Object path finding
    if (*beg == std::string_view("-o") && (beg + 1) != end)
      obj_path = *(beg + 1);
    cmd_builder << ' ' << *beg;
  }

  return get_timetrace_file(argv[path_id], cmd_builder.str(),
                            std::move(obj_path));
}
