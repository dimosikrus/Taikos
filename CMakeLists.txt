cmake_minimum_required (VERSION 3.10)
project ("Taikos")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- SFML через FetchContent ---
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Подключаем заголовки BASS из твоей новой папки
include_directories(include/bass)

# --- Логика выбора BASS под разные ОС ---
set(BASS_ROOT "${CMAKE_SOURCE_DIR}/thirdparty/bass")

if(WIN32)
    set(BASS_LIB "${BASS_ROOT}/win_x64/bass.lib")
    set(BASS_DLL "${BASS_ROOT}/win_x64/bass.dll")
elseif(APPLE)
    set(BASS_LIB "${BASS_ROOT}/osx_universal/libbass.dylib")
else()
    set(BASS_LIB "${BASS_ROOT}/linux_x64/libbass.so")
endif()

set(Exes
    src/main.cpp
    src/engine/animations/animations.cpp
    src/engine/animations/easing.cpp
    src/engine/audio/audio.cpp
)

add_executable(${PROJECT_NAME} ${Exes})

if(APPLE)
    # Говорим маку искать библиотеки в той же папке, где лежит сама программа
    set_target_properties(${PROJECT_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH TRUE
        INSTALL_RPATH "@executable_path"
    )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
    ${BASS_LIB}
    SFML::Graphics
    SFML::Window
    SFML::System
)

# --- Копирование ресурсов (Assets & Songs) ---
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/assets" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/Songs" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/Songs"
    COMMENT "Assets and Songs copied."
)

# --- Специфичное для Windows ---
if(WIN32)
    # Копируем только DLL (BASS_LIB на Windows — это .lib, он не нужен в bin)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BASS_DLL}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
    # ... (остальной твой код для DLL из windows64req) ...
endif()

# --- Специфичное для Linux ---
if(UNIX AND NOT APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${PROJECT_NAME} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BASS_LIB}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()

# --- Специфичное для MacOS ---
if(APPLE)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BASS_LIB}" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        COMMENT "Copying BASS dylib for MacOS"
    )
endif()