#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>
#include <chrono> // Để dùng hàm sleep
#include "payload_helper.hpp"

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0001

// Định nghĩa ID cho Event
#define SAMPLE_EVENT_ID 0x8001
#define SAMPLE_EVENTGROUP_ID 0x4465

std::shared_ptr<vsomeip::application> app;
bool stop_offer = false;

// Hàm xử lý Request cộng số (Giữ lại tính năng cũ)
void on_message(const std::shared_ptr<vsomeip::message> &_request) {
    std::shared_ptr<vsomeip::payload> its_payload = _request->get_payload();
    vsomeip::length_t l = its_payload->get_length();
    vsomeip::byte_t *data = its_payload->get_data();

    if (l >= 8) {
        uint32_t num1 = deserialize_uint32(data, 0);
        uint32_t num2 = deserialize_uint32(data, 4);
        uint32_t result = num1 + num2;

        std::shared_ptr<vsomeip::message> response = vsomeip::runtime::get()->create_response(_request);
        std::shared_ptr<vsomeip::payload> resp_payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> resp_data;
        serialize_uint32(resp_data, result);
        resp_payload->set_data(resp_data);
        response->set_payload(resp_payload);
        app->send(response);
    }
}

// Luồng giả lập gửi sự kiện Tốc độ xe
void run_speedometer() {
    uint32_t speed = 0;
    while(!stop_offer) {
        // 1. Tạo Payload
        std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
        std::vector<vsomeip::byte_t> payload_data;
        
        // Tăng tốc độ dần dần
        speed = (speed + 5) % 200; 
        serialize_uint32(payload_data, speed);
        payload->set_data(payload_data);

        // 2. Bắn sự kiện (Notify)
        // Lưu ý: Event luôn gửi đi, ai subscribe thì nhận được
        app->notify(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, payload);
        
        // Log nhẹ để biết server đang chạy
        // std::cout << "SERVICE: Broadcasting Speed: " << speed << " km/h" << std::endl;

        // 3. Ngủ 1 giây
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    app = vsomeip::runtime::get()->create_application("service-sample");
    app->init();

    // --- CẤU HÌNH EVENT ---
    // 1. Khai báo Service có cung cấp Event ID này
    std::set<vsomeip::eventgroup_t> its_groups;
    its_groups.insert(SAMPLE_EVENTGROUP_ID);

    app->offer_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, 
                     its_groups, 
                     vsomeip::event_type_e::ET_EVENT, std::chrono::milliseconds::zero(),
                     false, true, nullptr, vsomeip::reliability_type_e::RT_UNKNOWN);

    // 2. Chào mời Event Group (Quan trọng)
    // app->offer_eventgroup(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
    // ----------------------

    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    app->offer_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);
    
    std::cout << "SERVICE: Started. Broadcasting speed events..." << std::endl;

    // Chạy luồng bắn tốc độ
    std::thread speedometer_thread(run_speedometer);
    
    app->start();
    
    stop_offer = true;
    speedometer_thread.join();
}