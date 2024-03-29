find_library(Catch2 NAMES Catch2 Catch2d REQUIRED)

add_subdirectory(init)

add_executable(utils-tests
    appMain.cpp
    bits.cpp
    ExecAround.cpp
    GlobalRegistry.cpp
    Logger.cpp
    property.cpp
    Result.cpp
    ScopedExit.cpp
    StateMachine.cpp
    Watchdog.cpp
)

target_compile_definitions(utils-tests
    # This is required to selectively run Catch2 tests using tags on platforms, where cmd line arguments are not available.
    PRIVATE $<$<BOOL:${TEST_TAGS}>:TEST_TAGS="${TEST_TAGS}">
)

target_link_libraries(utils-tests
    PRIVATE optimized Catch2 debug Catch2d osal::cpp platform::init platform::main utils::bits utils::fsm
            utils::functional utils::info utils::logger utils::registry utils::types utils::watchdog
)

if (OSAL_PLATFORM STREQUAL linux)
    add_subdirectory(network)
endif ()

if (PLATFORM STREQUAL freertos-arm)
    objcopy_generate_bin(utils-tests)
endif ()
