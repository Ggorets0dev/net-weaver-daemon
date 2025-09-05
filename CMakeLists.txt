cmake_minimum_required(VERSION 3.16)

project(NetWeaverD LANGUAGES CXX VERSION 0.1.0)

# Формирование строки версии
set(NV_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (PROJECT_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}/third-party)

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

# Get all third-party dependencies
file(GLOB TC_SRC_FILES "third-party/*.c" "third-party/*.cpp" "third-party/*.cc")
file(GLOB TC_INC_FILES "third-party/*.h" "third-party/*.hpp")

add_executable(NetWeaverD ${SRC_FILES} ${INC_FILES} ${TC_INC_FILES} ${TC_SRC_FILES})

target_include_directories(NetWeaverD PRIVATE ${PROJECT_INCLUDE_DIRS})

target_link_libraries(NetWeaverD PRIVATE
	${Protobuf_LIBRARIES}     
	log4cxx)

# include(GNUInstallDirs)

# install(TARGETS NetWeaverD
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
# )

# Install log config into ~./config/..
install(CODE "
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory \$ENV{HOME}/.config/net-weaver)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/log4cxx.properties
    \$ENV{HOME}/.config/net-weaver/log4cxx.properties
  )
")
