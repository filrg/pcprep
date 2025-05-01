cmake_minimum_required(VERSION 3.21)

project(pcprepExamples C CXX)

include(../cmake/folders.cmake)

if(PROJECT_IS_TOP_LEVEL)
  find_package(pcprep REQUIRED)
endif()

add_custom_target(run-examples)

function(add_c_example NAME)
  add_executable("${NAME}" "${NAME}.c")
  target_link_libraries("${NAME}" PRIVATE pcprep::pcprep)
  target_compile_features("${NAME}" PRIVATE c_std_99)
  add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
  add_dependencies("run_${NAME}" "${NAME}")
  add_dependencies(run-examples "run_${NAME}")
endfunction()

add_executable(cam2mat cam2mat.cpp ../source/cJSON.c)
target_link_libraries(cam2mat PRIVATE pcprep::pcprep)
target_compile_features(cam2mat PRIVATE cxx_std_17)
add_custom_target("run_cam2mat" COMMAND "cam2mat" VERBATIM)
add_dependencies("run_cam2mat" "cam2mat")
add_dependencies(run-examples "run_cam2mat")


add_folders(Example)
