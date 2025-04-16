# cmake/my-docs-ci.cmake

# 1. Check input variables
foreach(var IN ITEMS PROJECT_SOURCE_DIR PROJECT_BINARY_DIR)
  if(NOT DEFINED "${var}")
    message(FATAL_ERROR "${var} must be defined")
  endif()
endforeach()

# 2. Set paths
set(DOXYGEN_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/docs")
file(MAKE_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}")

set(DOXYFILE_IN "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in")
set(DOXYFILE_OUT "${DOXYGEN_OUTPUT_DIRECTORY}/Doxyfile")

# 3. Check if doxygen exists
find_program(DOXYGEN_EXECUTABLE doxygen)
if(NOT DOXYGEN_EXECUTABLE)
  message(FATAL_ERROR "Doxygen not found. Please install it.")
endif()

# 4. Configure the Doxyfile
configure_file("${DOXYFILE_IN}" "${DOXYFILE_OUT}" @ONLY)

# 5. Run doxygen
execute_process(
  COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYFILE_OUT}"
  WORKING_DIRECTORY "${DOXYGEN_OUTPUT_DIRECTORY}"
  RESULT_VARIABLE result
)
if(NOT result EQUAL 0)
  message(FATAL_ERROR "Doxygen failed with exit code ${result}")
endif()

