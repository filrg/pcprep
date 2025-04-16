cmake_minimum_required(VERSION 3.21)

project(pcp C)

include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcprep REQUIRED)
endif()

add_custom_target(run-app)

function(add_app NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcprep::pcprep)
  target_compile_features("${NAME}" PRIVATE c_std_99)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-app "run_${NAME}")
endfunction()

add_app(pcp)

add_folders(Pcp)
