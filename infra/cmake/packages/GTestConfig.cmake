function(_GTest_build)
  if(NOT BUILD_GTEST)
    return()
  endif(NOT BUILD_GTEST)

  nnas_find_package(GTestSource QUIET)

  if(NOT GTestSource_FOUND)
    return()
  endif(NOT GTestSource_FOUND)

  nnas_include(ExternalBuildTools)
  ExternalBuild_CMake(CMAKE_DIR   ${GTestSource_DIR}
                      BUILD_DIR   ${CMAKE_BINARY_DIR}/externals/GTEST/build
                      INSTALL_DIR ${EXT_OVERLAY_DIR}
                      IDENTIFIER  "1.8.0-fix1"
                      PKG_NAME    "GTEST")

endfunction(_GTest_build)

_GTest_build()

### Find and use pre-installed Google Test
# Note: cmake supports GTest and does not find GTestConfig.cmake or GTest-config.cmake.
# Refer to "https://cmake.org/cmake/help/v3.5/module/FindGTest.html"
# find_package(GTest) creates options like GTEST_FOUND, not GTest_FOUND.
find_package(GTest)
find_package(Threads)

if(${GTEST_FOUND} AND TARGET Threads::Threads)
  if(NOT TARGET gtest)
    add_library(gtest INTERFACE)
    target_include_directories(gtest INTERFACE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(gtest INTERFACE ${GTEST_LIBRARIES} Threads::Threads)
  endif(NOT TARGET gtest)

  if(NOT TARGET gtest_main)
    add_library(gtest_main INTERFACE)
    target_include_directories(gtest_main INTERFACE ${GTEST_INCLUDE_DIRS})
    target_link_libraries(gtest_main INTERFACE gtest)
    target_link_libraries(gtest_main INTERFACE ${GTEST_MAIN_LIBRARIES})

    # GTest_AddTest(TGT ...) creates an executable target and registers that executable as a CMake test
    function(GTest_AddTest TGT)
      add_executable(${TGT} ${ARGN})
      target_link_libraries(${TGT} gtest_main)
      add_test(${TGT} ${TGT})
    endfunction(GTest_AddTest)
  endif(NOT TARGET gtest_main)

  set(GTest_FOUND TRUE)
endif(${GTEST_FOUND} AND TARGET Threads::Threads)
