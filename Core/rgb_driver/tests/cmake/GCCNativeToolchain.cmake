set(CMAKE_SYSTEM_NAME               Windows)
set(CMAKE_SYSTEM_PROCESSOR          CMAKE_HOST_SYSTEM_PROCESSOR)

set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

# Some default GCC settings

set(CMAKE_C_COMPILER                gcc)
set(CMAKE_ASM_COMPILER              ${CMAKE_C_COMPILER})
set(CMAKE_CXX_COMPILER              g++)
set(CMAKE_LINKER                    g++)
set(CMAKE_OBJCOPY                   objcopy)
set(CMAKE_SIZE                      size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM     ".exe")
set(CMAKE_EXECUTABLE_SUFFIX_C       ".exe")
set(CMAKE_EXECUTABLE_SUFFIX_CXX     ".exe")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lgtest -lgtest_main -lpthread -Wall")
set(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

