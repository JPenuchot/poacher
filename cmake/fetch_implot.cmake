# Downloading and extracting implot archive
file(DOWNLOAD
  https://github.com/epezent/implot/archive/v0.8.tar.gz
  implot.tar.gz)
file(ARCHIVE_EXTRACT INPUT implot.tar.gz DESTINATION implot)

# Setting variables
set(IMPLOT_ROOT "${CMAKE_BINARY_DIR}/implot/implot-0.8")
set(IMPLOT_INCLUDE_DIR "${IMPLOT_ROOT}")
file(GLOB_RECURSE IMPLOT_SOURCES "${IMPLOT_ROOT}/*.cpp")
