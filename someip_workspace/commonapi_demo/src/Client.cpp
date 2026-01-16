#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "v1/commonapi/demo/DemoServiceProxy.hpp"

using namespace v1::commonapi::demo;

int main() {
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    // Tạo Proxy để nói chuyện với Service
    std::shared_ptr<DemoServiceProxy<>> myProxy = 
        runtime->buildProxy<DemoServiceProxy>("local", "commonapi.demo.DemoService");
    if (!myProxy) {
        std::cerr << "CLIENT: Proxy creation failed! Check commonapi.ini" << std::endl;
        return 1;
    }
    // Đợi kết nối
    std::cout << "CLIENT: Waiting for service..." << std::endl;
    while (!myProxy->isAvailable()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    std::cout << "CLIENT: Service Connected!" << std::endl;

    // 1. Đăng ký nhận Event Tốc độ
    myProxy->getCarStatusAttribute().getChangedEvent().subscribe([&](const v1::commonapi::demo::DemoService::VehicleState& val) {
            
            // 2. Truy cập các trường trong struct
            std::cout << "CLIENT: [VehicleState] Speed: " << val.getSpeed() 
                    << " km/h | Gear: " << (int)val.getGear() 
                    << " | Fuel: " << (int)val.getFuelLevel() << "%" << std::endl;
        });

    // 2. Gọi hàm Add (Synchronous - gọi xong chờ kết quả luôn cho gọn)
    CommonAPI::CallStatus callStatus;
    uint32_t result;
    uint32_t n1 = 100, n2 = 500;

    std::cout << "CLIENT: Calling Add(100, 500)..." << std::endl;
    myProxy->add(n1, n2, callStatus, result);

    if (callStatus == CommonAPI::CallStatus::SUCCESS) {
        std::cout << "CLIENT: [Response] Result: " << result << std::endl;
    } else {
        std::cout << "CLIENT: Call failed!" << std::endl;
    }

    // Giữ chương trình chạy để nhận Event
    while (true) { std::this_thread::sleep_for(std::chrono::seconds(1)); }
}