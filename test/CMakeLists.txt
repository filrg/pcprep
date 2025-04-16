cmake_minimum_required(VERSION 3.21)

project(pcprepTests LANGUAGES C)

include(../cmake/folders.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
    find_package(pcprep REQUIRED)
    enable_testing()
endif()

# ---- Tests ----
set(TEST_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../assets")

add_executable(pc_io source/pc_io.c)
add_executable(tiling source/tiling.c)
add_executable(subsampling source/subsampling.c)

target_link_libraries(pc_io PRIVATE pcprep::pcprep)
target_link_libraries(tiling PRIVATE pcprep::pcprep)
target_link_libraries(subsampling PRIVATE pcprep::pcprep)

target_compile_features(pc_io PRIVATE c_std_99)
target_compile_features(tiling PRIVATE c_std_99)
target_compile_features(subsampling PRIVATE c_std_99)


add_test(NAME pc_io COMMAND pc_io ${TEST_ASSETS_DIR}/longdress0000.ply)
add_test(NAME tiling COMMAND tiling ${TEST_ASSETS_DIR}/longdress0000.ply 2 2 2 1 test)
add_test(NAME subsampling COMMAND subsampling ${TEST_ASSETS_DIR}/longdress0000.ply 0.5 ouput.ply)

if(BUILD_APP)
    add_test(NAME pcp_io COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o IO_test.ply)
    add_test(NAME pcp_tiling COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o tile%04d.ply --pre-process=TILE -t 2,2,2)
    add_test(NAME pcp_p_sample COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o half.ply -p sample 0.5 0)
    add_test(NAME pcp_p_voxel COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o voxel.ply -p voxel 3)
    add_test(NAME pcp_p_remove_duplicates COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o clean.ply -p remove-duplicates)
    add_test(NAME pcp_s_aabb COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o dummy.ply --pre-process=TILE -t 2,2,2 -s aabb 1 0 bbox%04d.ply)
    add_test(NAME pcp_s_pixel_per_tile COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o dummy.ply -s pixel-per-tile ${TEST_ASSETS_DIR}/cam-matrix.json 2,2,2 visi.json)
    add_test(NAME pcp_s_save_viewport COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o dummy.ply -s save-viewport ${TEST_ASSETS_DIR}/cam-matrix.json 255,255,255 view%04d.tile%04d.png)
    add_test(NAME pcp_s_screen_area_estimation COMMAND pcp -i ${TEST_ASSETS_DIR}/longdress0000.ply -o tile%04d.ply --pre-process=TILE -t 2,2,2 -s screen-area-estimation ${TEST_ASSETS_DIR}/cam-matrix.json screen-area-tile%04d.json)
endif()
# ---- End-of-file commands ----

add_folders(Test)
