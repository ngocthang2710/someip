cmake_minimum_required(VERSION 3.0)
project(someip_demo)

find_package(vsomeip3 3.1.20 REQUIRED)
find_package(Boost 1.55 COMPONENTS system thread log REQUIRED)

# Thêm dòng này để trình biên dịch tìm thấy các file .hpp trong thư mục src
include_directories(${Boost_INCLUDE_DIRS} src)

# Build Service
add_executable(service_sample src/service.cpp)
target_link_libraries(service_sample vsomeip3 ${Boost_LIBRARIES})

# Build Client
add_executable(client_sample src/client.cpp)
target_link_libraries(client_sample vsomeip3 ${Boost_LIBRARIES})