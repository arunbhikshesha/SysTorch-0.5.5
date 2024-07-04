include(ewfiles.cmake)

add_library(GeneratedCodeLib STATIC
${EMWIFILES}
)

target_include_directories(GeneratedCodeLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}

)
