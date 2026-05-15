cmake_minimum_required(VERSION 3.10)
project(GeoSniffer VERSION 1.0)

# Standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# PATHs for headers
include_directories(/usr/local/include)

# main C++ files
add_executable(geo_sniffer 
    main.cpp 
    src/ParseWifi.cpp 
    src/PostGC.cpp
    src/TGBot.cpp
)

target_include_directories(geo_sniffer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Required libs to compile
target_link_libraries(geo_sniffer
    PRIVATE
    TgBot              # -lTgBot
    boost_system       # -lboost_system
    ssl                # -lssl
    crypto             # -lcrypto
    pthread            # -lpthread
    curl               # Per le Google API
    gpiodcxx           # Per il buzzer/bottone (libgpiod v2)
    rt                 # Per le funzioni di tempo real-time (consigliato)
)

# Optional optimization for compiling
if(MSVC)
    target_compile_options(geo_sniffer PRIVATE /W4 /O2)
else()
    target_compile_options(geo_sniffer PRIVATE -Wall -Wextra -O3)
endif()