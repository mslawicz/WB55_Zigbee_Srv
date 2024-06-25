set(CMAKE_SYSTEM_NAME               Windows)
set(CMAKE_SYSTEM_PROCESSOR          ${CMAKE_HOST_SYSTEM_PROCESSOR})

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings
# arm-none-eabi- must be part of path environment
set(TOOLCHAIN_PREFIX                C:/msys64/ucrt64/bin/)

set(CMAKE_C_COMPILER                ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_LINKER                    ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY                   ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE                      ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".exe")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".exe")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".exe")

#set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)