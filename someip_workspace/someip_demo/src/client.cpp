#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
#include "payload_helper.hpp"

#define SAMPLE_SERVICE_ID 0x1234
#define SAMPLE_INSTANCE_ID 0x5678
#define SAMPLE_METHOD_ID 0x0001

#define SAMPLE_EVENT_ID 0x8001
#define SAMPLE_EVENTGROUP_ID 0x4465

std::shared_ptr<vsomeip::application> app;
std::mutex mutex;
std::condition_variable condition;
bool service_available = false;

// Xử lý tin nhắn đến (Cả phản hồi Method VÀ Event)
void on_message(const std::shared_ptr<vsomeip::message> &_msg) {
    std::shared_ptr<vsomeip::payload> pl = _msg->get_payload();
    uint32_t val = deserialize_uint32(pl->get_data(), 0);

    // Kiểm tra xem tin nhắn là Response hay Event
    if (_msg->get_method() == SAMPLE_METHOD_ID && _msg->get_message_type() == vsomeip::message_type_e::MT_RESPONSE) {
        std::cout << "CLIENT: [Method Response] Sum result: " << std::dec << val << std::endl;
    } 
    else if (_msg->get_method() == SAMPLE_EVENT_ID && _msg->get_message_type() == vsomeip::message_type_e::MT_NOTIFICATION) {
        std::cout << "CLIENT: [Event Notification] Current Speed: " << std::dec << val << " km/h" << std::endl;
    }
}

void on_availability(vsomeip::service_t _service, vsomeip::instance_t _instance, bool _is_available) {
    std::cout << "CLIENT: Service is " << (_is_available ? "ONLINE" : "OFFLINE") << std::endl;
    if (_is_available) {
        // Khi thấy Service, thực hiện SUBSCRIBE ngay
        
        // 1. Request Event (Báo cho stack biết tôi muốn dùng event này)
        std::set<vsomeip::eventgroup_t> its_groups;
        its_groups.insert(SAMPLE_EVENTGROUP_ID);
        app->request_event(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, its_groups, vsomeip::event_type_e::ET_EVENT);
        
        // 2. Subscribe Event Group (Đăng ký nhận tin)
        app->subscribe(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENTGROUP_ID);
        std::cout << "CLIENT: Subscribed to Speed EventGroup." << std::endl;

        std::unique_lock<std::mutex> lock(mutex);
        service_available = true;
        condition.notify_one();
    }
}

void run_request_thread() {
    std::unique_lock<std::mutex> lock(mutex);
    condition.wait(lock, []{ return service_available; });

    // Client vẫn thử gọi hàm cộng số 1 lần để test song song
    std::shared_ptr<vsomeip::message> request = vsomeip::runtime::get()->create_request();
    request->set_service(SAMPLE_SERVICE_ID);
    request->set_instance(SAMPLE_INSTANCE_ID);
    request->set_method(SAMPLE_METHOD_ID);

    std::shared_ptr<vsomeip::payload> payload = vsomeip::runtime::get()->create_payload();
    std::vector<vsomeip::byte_t> payload_data;
    serialize_uint32(payload_data, 100);
    serialize_uint32(payload_data, 200);
    payload->set_data(payload_data);
    request->set_payload(payload);
    
    app->send(request);
}

int main() {
    app = vsomeip::runtime::get()->create_application("client-sample");
    app->init();
    
    app->register_availability_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, on_availability);
    
    // Đăng ký nhận tin cho Method ID (Cộng số)
    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_METHOD_ID, on_message);
    
    // Đăng ký nhận tin cho Event ID (Tốc độ)
    app->register_message_handler(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID, SAMPLE_EVENT_ID, on_message);

    app->request_service(SAMPLE_SERVICE_ID, SAMPLE_INSTANCE_ID);

    std::thread sender(run_request_thread);
    sender.detach();

    app->start();
}