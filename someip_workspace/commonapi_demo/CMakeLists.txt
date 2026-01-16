cmake_minimum_required(VERSION 3.0)
project(commonapi_demo)

# Tìm các gói thư viện
find_package(CommonAPI 3.2 REQUIRED)
find_package(CommonAPI-SomeIP 3.2 REQUIRED)
find_package(vsomeip3 3.1.20 REQUIRED)

# In ra màn hình để debug xem nó tìm thấy đường dẫn nào (Optional)
message(STATUS "CommonAPI Include: ${CommonAPI_INCLUDE_DIRS}")
message(STATUS "CommonAPI-SomeIP Include: ${CommonAPI-SomeIP_INCLUDE_DIRS}")

# --- QUAN TRỌNG: Thêm đường dẫn header vào project ---
include_directories(
    src
    src-gen
    /usr/local/include/CommonAPI-3.2         # Header của CommonAPI Core
    /usr/local/include/CommonAPI-SomeIP-3.2  # Header của Binding
    /usr/local/include/vsomeip               # Header của vsomeip
)

# Tìm tất cả file code sinh ra
file(GLOB GEN_SRCS "src-gen/v1/commonapi/demo/*.cpp")

# Build Service
add_executable(service src/Service.cpp ${GEN_SRCS})
target_link_libraries(service CommonAPI CommonAPI-SomeIP vsomeip3 pthread)

# Build Client
add_executable(client src/Client.cpp ${GEN_SRCS})
target_link_libraries(client CommonAPI CommonAPI-SomeIP vsomeip3 pthread)