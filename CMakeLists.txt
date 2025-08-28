cmake_minimum_required(VERSION 3.16)

project(NetWeaverD LANGUAGES CXX VERSION 0.1.0)

# Формирование строки версии
set(NV_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск утилиты protobuf
find_package(Protobuf REQUIRED)

# Пытаемся найти Abseil
find_package(absl REQUIRED)

# Добавляем подпроект
add_subdirectory(external/rgc)

# Генерация software_info.hpp
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/software_info.hpp.in
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/software_info.hpp
    @ONLY
)

# Получение основных исходников проекта
file(GLOB SRC_FILES "src/*.c" "src/*.cpp" "src/*.cc")
file(GLOB INC_FILES "inc/*.h" "inc/*.hpp")

add_executable(NetWeaverD ${SRC_FILES} ${INC_FILES})

# include(GNUInstallDirs)

# install(TARGETS NetWeaverD
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )
