#include <iostream>
#include "DelimitedMessagesStreamParser.hpp"
#include "helpers.hpp"

int main() {
//FIRST MSG TEST
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "19851019T050107.333";
    fast->set_current_date_time(s);

    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(209);

    TestTask::Messages::RequestForSlowResponse* slow_request = m.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(10);

    TestTask::Messages::RequestForFastResponse* fast_request = m.mutable_request_for_fast_response();


//SECOND MSG TEST
    TestTask::Messages::WrapperMessage m1;
    fast = m1.mutable_fast_response();
    s = "20851019T050107.666";
    fast->set_current_date_time(s);

    // slow = m1.mutable_slow_response();
    // slow->set_connected_client_count(9);

    slow_request = m1.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(1);

    fast_request = m1.mutable_request_for_fast_response();


// START PARSING
typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
    Parser parser;

    bool a = false;
    bool b = false;
    std::string m_str, m1_str;
    m.SerializeToString(&m_str);
    m1.SerializeToString(&m1_str);

    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(data->begin(), data->end());
    data = serializeDelimited(m1);
    data_str.append(data->begin(), data->end());

    std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> parsedMessages = parser.parse(data_str);
    for (auto msg : parsedMessages) {
        std::string msg_str;
        msg->SerializeToString(&msg_str);

        if (msg_str == m_str)  a = true;
        if (msg_str == m1_str) b = true;

        msg->PrintDebugString();
        std::cout << std::endl;
    }

    if (a && b) std::cout << "All Messages were correctly decrypted!" << std::endl;

    return 0;
}
