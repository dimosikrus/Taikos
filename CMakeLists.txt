cmake_minimum_required (VERSION 3.10)
project ("Taikos")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

set(Exes
    src/main.cpp
    src/engine/animations/animations.cpp
    src/engine/animations/easing.cpp
)

add_executable(${PROJECT_NAME} ${Exes})

if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Songs" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Songs"
    COMMENT "Assets and Songs copied."
)