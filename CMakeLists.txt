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

include_directories(include)

link_directories(
    ${CMAKE_SOURCE_DIR}/libs64/lib
)
    
set(Exes
    src/main.cpp
    src/engine/animations/animations.cpp
    src/engine/animations/easing.cpp
    src/engine/audio/audio.cpp
)

add_executable(${PROJECT_NAME} ${Exes})

set(Libs
    bass.lib
    bass_fx.lib
    SFML::Graphics
    SFML::Window
    SFML::System
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${Libs})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Songs"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/Songs"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Songs"
    COMMENT "Копируем ASSETS & SONGS в выходную директорию\n"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        "${CMAKE_SOURCE_DIR}/libs64/dll/bass.dll"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MACHINE:X64"
    )
endif()

