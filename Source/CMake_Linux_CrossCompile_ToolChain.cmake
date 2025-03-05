# Define the toolchain

add_definitions(-UWIN32)
add_definitions(-UWINDOWS)

set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(TOOLCHAIN_ROOT C:/UnrealToolchains/v20_clang-13.0.1-centos7/x86_64-unknown-linux-gnu)

# set(CMAKE_SYSROOT C:/UnrealToolchains/v20_clang-13.0.1-centos7/x86_64-unknown-linux-gnu)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc")

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --sysroot=${CMAKE_SYSROOT} -I${TOOLCHAIN_ROOT}/include")
set(CMAKE_C_COMPILER clang)
set(CMAKE_C_COMPILER ${TOOLCHAIN_ROOT}/bin/clang.exe)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_ROOT}/bin/clang++.exe)
set(CMAKE_FIND_ROOT_PATH ${TOOLCHAIN_ROOT})
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# include_directories(BEFORE SYSTEM ${MY_BUILD_DIRECTORY}/gcc-4.8.5-aarch64/install/aarch64-unknown-linux-gnu/sysroot/usr/include/)
# set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
# ${TOOLCHAIN_ROOT}/usr/include
# )

# set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES
# ${TOOLCHAIN_ROOT}/include/c++/4.85
# )

# include_directories(${TOOLCHAIN_ROOT}/usr/include)

# \include\c++\4.8.5

# Set the path to the directory containing the Extra header files
# include_directories(${TOOLCHAIN_ROOT}/usr/include)
include_directories(${TOOLCHAIN_ROOT}/include/c++/4.8.5)
include_directories(${TOOLCHAIN_ROOT}/include/c++/4.8.5/tr1)
include_directories(${TOOLCHAIN_ROOT}/include/c++/4.8.5/parallel)
include_directories(${TOOLCHAIN_ROOT}/include/c++/4.8.5/x86_64-unknown-linux-gnu)
include_directories(${TOOLCHAIN_ROOT}/lib/gcc/x86_64-unknown-linux-gnu/4.8.5/include)

# include_directories(${TOOLCHAIN_ROOT}/lib/clang/13.0.1/include)
set(X11_INCLUDE_DIR ${TOOLCHAIN_ROOT}/usr/include/X11)

# include_directories(${TOOLCHAIN_ROOT}/usr/include/)

# Add the directory to the include path
include_directories(${X11_INCLUDE_DIR})

# set(CMAKE_SYSTEM_NAME Linux)

# # set(CMAKE_SYSTEM_PROCESSOR arm)
# set(tools C:/UnrealToolchains/v20_clang-13.0.1-centos7/x86_64-unknown-linux-gnu)
# set(triple x86_64-unknown-linux-gnu)

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_C_COMPILER_TARGET ${triple})
# set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_CXX_COMPILER_TARGET ${triple})

# set(CMAKE_SYSTEM_NAME Linux)
# set(CMAKE_SYSTEM_PROCESSOR x64)

# if(MINGW OR CYGWIN OR WIN32)
# set(UTIL_SEARCH_CMD where)
# elseif(UNIX OR APPLE)
# set(UTIL_SEARCH_CMD which)
# endif()

# set(TOOLCHAIN_PREFIX x86_64-unknown-linux-gnu-)
# set(TOOLCHAIN_TRIPLE x86_64-unknown-linux-gnu)

# execute_process(
# COMMAND ${UTIL_SEARCH_CMD} ${TOOLCHAIN_PREFIX}gcc
# OUTPUT_VARIABLE BINUTILS_PATH
# OUTPUT_STRIP_TRAILING_WHITESPACE
# )

# if(MINGW OR CYGWIN OR WIN32)
# if(BINUTILS_PATH)
# string(REPLACE "\n" ";" BINUTILS_PATH "${BINUTILS_PATH}")
# list(GET BINUTILS_PATH 0 BINUTILS_PATH)
# endif()
# endif()

# if(NOT BINUTILS_PATH)
# message(FATAL_ERROR "linux GCC toolchain BINUTILS_PATH not found")
# endif()

# get_filename_component(ARM_TOOLCHAIN_DIR ${BINUTILS_PATH} DIRECTORY)
# set(ARM_GCC_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
# execute_process(COMMAND ${ARM_GCC_C_COMPILER} -print-sysroot
# OUTPUT_VARIABLE ARM_GCC_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
#
# # get GNU ARM GCC version
# execute_process(COMMAND ${ARM_GCC_C_COMPILER} --version
# OUTPUT_VARIABLE ARM_GCC_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
# string(REGEX MATCH " [0-9]+\.[0-9]+\.[0-9]+ " ARM_GCC_VERSION ${ARM_GCC_VERSION})
# string(STRIP ${ARM_GCC_VERSION} ARM_GCC_VERSION)

# set compiler triple
# set(triple ${TOOLCHAIN_TRIPLE})
# set(CMAKE_ASM_COMPILER clang)
# set(CMAKE_ASM_COMPILER_TARGET ${triple})
# set(CMAKE_C_COMPILER clang)
# set(CMAKE_C_COMPILER_TARGET ${triple})
# set(CMAKE_CXX_COMPILER clang++)
# set(CMAKE_CXX_COMPILER_TARGET ${triple})

# set(CMAKE_C_FLAGS_INIT " -B${ARM_TOOLCHAIN_DIR}")
# set(CMAKE_CXX_FLAGS_INIT " -B${ARM_TOOLCHAIN_DIR} ")

# # Without that flag CMake is not able to pass test compilation check
# if(${CMAKE_VERSION} VERSION_EQUAL "3.6.0" OR ${CMAKE_VERSION} VERSION_GREATER "3.6")
# set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
# else()
# set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostdlib")
# endif()

# set(CMAKE_OBJCOPY llvm-objcopy CACHE INTERNAL "objcopy tool")
# set(CMAKE_SIZE_UTIL llvm-size CACHE INTERNAL "size tool")

# # Default C compiler flags
# set(CMAKE_C_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG")
# set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_C_FLAGS_RELEASE_INIT "-O3 -Wall")
# set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_C_FLAGS_MINSIZEREL_INIT "-Oz -Wall")
# set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_C_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -Wall")
# set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)

# # Default C++ compiler flags
# set(TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG "")
# string(APPEND TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG " -cxx-isystem ${ARM_GCC_SYSROOT}/include/c++/${ARM_GCC_VERSION}")
# string(APPEND TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG " -cxx-isystem ${ARM_GCC_SYSROOT}/include/c++/${ARM_GCC_VERSION}/{TOOLCHAIN_TRIPLE}")
# string(APPEND TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG " -cxx-isystem ${ARM_GCC_SYSROOT}/include/c++/${ARM_GCC_VERSION}/backward")
# set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g3 -Og -Wall -pedantic -DDEBUG ${TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG}")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_RELEASE_INIT "-O3 -Wall ${TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG}")
# set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_MINSIZEREL_INIT "-Oz -Wall ${TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG}")
# set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL_INIT}" CACHE STRING "" FORCE)
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT "-O2 -g -Wall ${TOOLCHAIN_CXX_INCLUDE_DIRS_FLAG}")
# set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO_INIT}" CACHE STRING "" FORCE)

# set(CMAKE_SYSROOT ${ARM_GCC_SYSROOT})
# set(CMAKE_FIND_ROOT_PATH ${ARM_GCC_SYSROOT})
# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)