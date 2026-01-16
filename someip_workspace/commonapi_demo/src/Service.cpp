#include <iostream>
#include <thread>
#include <CommonAPI/CommonAPI.hpp>
#include "v1/commonapi/demo/DemoServiceStubDefault.hpp"

// Kế thừa Stub mặc định
class DemoServiceStubImpl : public v1::commonapi::demo::DemoServiceStubDefault {
public:
    DemoServiceStubImpl() = default;

    // 1. Implement hàm Add (Dễ hơn raw vsomeip rất nhiều!)
    virtual void add(const std::shared_ptr<CommonAPI::ClientId> _client, 
                     uint32_t _num1, uint32_t _num2, addReply_t _reply) override {
        
        std::cout << "SERVICE: Request Add: " << _num1 << " + " << _num2 << std::endl;
        // Gửi phản hồi
        _reply(_num1 + _num2);
    }
};

void run_speedometer(std::shared_ptr<DemoServiceStubImpl> service) {
    // 1. Khai báo struct (được sinh ra từ FIDL)
    v1::commonapi::demo::DemoService::VehicleState state;
    
    int speed = 0;
    int gear = 0;

    while(true) {
        // Giả lập dữ liệu
        speed = (speed + 2) % 200;
        
        // Logic giả lập số (Gear) dựa trên tốc độ
        if (speed == 0) gear = 0; // N
        else if (speed < 20) gear = 1;
        else if (speed < 50) gear = 2;
        else gear = 3;

        // 2. Điền dữ liệu vào Struct
        // CommonAPI sinh ra hàm set... cho từng field
        state.setSpeed(speed);
        state.setGear(gear);
        state.setFuelLevel(75); // Giả sử xăng còn 75%

        // 3. Bắn cả cục Struct đi
        service->setCarStatusAttribute(state); 
        
        std::cout << "SERVICE: Updated State -> Speed:" << speed 
                  << " | Gear:" << gear << std::endl;

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    // Khởi tạo CommonAPI Runtime
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

    // Khởi tạo Service của mình
    std::shared_ptr<DemoServiceStubImpl> myService = std::make_shared<DemoServiceStubImpl>();

    // Đăng ký Service với tên "local:commonapi.demo.DemoService:commonapi.demo.DemoService"
    // (Format: local:<Interface>:<Instance>)
    runtime->registerService("local", "commonapi.demo.DemoService", myService);

    std::cout << "SERVICE: Ready!" << std::endl;

    std::thread t(run_speedometer, myService);
    t.join();
}