/// -ftime-trace wrapper for clang.

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

/// Wrapper for a given clang command.

int get_timetrace_file(std::filesystem::path const out, std::string cmd,
                       std::filesystem::path obj_path) {
  namespace fs = std::filesystem;

  // Run program
  std::system(cmd.c_str());
  if (auto const out_parent = out.parent_path(); !out_parent.empty()) {
    fs::create_directories(out.parent_path());
  }

  fs::copy_file(obj_path.replace_extension(".json"), out,
                fs::copy_options::overwrite_existing);

  return 0;
}

int main(int argc, char const *argv[]) {
  namespace fs = std::filesystem;

  constexpr int exec_id = 0;
  constexpr int path_id = 1;
  constexpr int cmd_start_id = 2;

  if (argc < 3) {
    std::cout << "Usage: " << argv[exec_id]
              << "time_trace_export_path.json [COMMAND]";
    return 1;
  }

  // Building path and finding obj_path
  std::ostringstream cmd_builder;
  fs::path obj_path;
  bool has_time_trace_flag = false;

  cmd_builder << argv[cmd_start_id];
  for (auto beg = &argv[cmd_start_id + 1], end = &argv[argc]; beg < end;
       beg++) {
    // Object path finding
    if (*beg == std::string_view("-o") && (beg + 1) != end) {
      obj_path = *(beg + 1);
    }

    if (*beg == std::string_view("-ftime-trace") ||
        *beg == std::string_view("--ftime-trace")) {
      has_time_trace_flag = true;
    }
    cmd_builder << ' ' << *beg;
  }

  if (!has_time_trace_flag) {
    cmd_builder << " -ftime-trace";
  }

  return get_timetrace_file(argv[path_id], std::move(cmd_builder.str()),
                            std::move(obj_path));
}
