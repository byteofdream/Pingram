cmake_minimum_required(VERSION 3.20)
project(PingramClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)
find_package(nlohmann_json REQUIRED)

add_executable(pingram_client
    app/main.cpp
    ui/main_window.cpp
    ui/premium_orb_widget.cpp
    transport/http_client.cpp
    api/auth/auth_requests.cpp
    api/messages/messages_requests.cpp
    api/users/users_requests.cpp
    api/files/files_requests.cpp
)

target_include_directories(pingram_client PRIVATE .)

target_link_libraries(pingram_client PRIVATE
    Qt6::Widgets
    Qt6::Network
    nlohmann_json::nlohmann_json
)
