add_library(logger logger.cpp)

include(FetchContent)

FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG        v1.8.5
)

set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(spdlog)

target_compile_definitions(logger
    PUBLIC
        -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_DEBUG
)

target_link_libraries(logger
    PUBLIC
        spdlog::spdlog_header_only
)

target_include_directories(logger
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)
