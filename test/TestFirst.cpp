#include <gtest/gtest.h>
#include "protobuf_parser/DelimitedMessagesStreamParser.hpp"
#include "protobuf_parser/helpers.hpp"

TEST(MyOwnTestParser, FullMsg) {
    //Create FullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "19851019T050107.333";
    fast->set_current_date_time(s);

    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(209);

    TestTask::Messages::RequestForSlowResponse* slow_request = m.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(10);

    TestTask::Messages::RequestForFastResponse* fast_request = m.mutable_request_for_fast_response();

    //FullMsg to string
    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(data->begin(), data->end());

    //Parsing
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
        Parser parser;
    std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> parsedMessages = parser.parse(data_str);
    for (auto msg : parsedMessages) {
        ASSERT_TRUE(msg->has_request_for_fast_response());
        ASSERT_TRUE(msg->has_fast_response());
        ASSERT_TRUE(msg->has_slow_response());
        ASSERT_TRUE(msg->has_request_for_slow_response());
        ASSERT_TRUE(msg->fast_response().has_current_date_time());
        ASSERT_TRUE(msg->slow_response().has_connected_client_count());
        ASSERT_TRUE(msg->request_for_slow_response().has_time_in_seconds_to_sleep());
    }
    EXPECT_EQ(parsedMessages.size(), 1);
}

TEST(MyOwnTestParser, ThreeFullMsg) {
    //Create FullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "20851019T050107.666";
    fast->set_current_date_time(s);

    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(9);

    TestTask::Messages::RequestForSlowResponse* slow_request = m.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(0);

    TestTask::Messages::RequestForFastResponse* fast_request = m.mutable_request_for_fast_response();

    //FullMsg to string
    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(data->begin(), data->end());
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
        Parser parser;
        std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> parsedMessages;
        std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> resultList;


    //Parsing 3 times
    for (int i=0; i<3; i++) {
        parsedMessages = parser.parse(data_str);
        for (auto msg : parsedMessages) {
            ASSERT_TRUE(msg->has_request_for_fast_response());
            ASSERT_TRUE(msg->has_fast_response());
            ASSERT_TRUE(msg->has_slow_response());
            ASSERT_TRUE(msg->has_request_for_slow_response());
            ASSERT_TRUE(msg->fast_response().has_current_date_time());
            ASSERT_TRUE(msg->slow_response().has_connected_client_count());
            ASSERT_TRUE(msg->request_for_slow_response().has_time_in_seconds_to_sleep());
            resultList.push_back(msg);
        }
    }
    EXPECT_EQ(resultList.size(), 3);
}

TEST(MyOwnTestParser, TwoMsgWithExtraBytesBetween) {
    //Create FirstFullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "10851209T050107.111";
    fast->set_current_date_time(s);

    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(5);

    TestTask::Messages::RequestForSlowResponse* slow_request = m.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(3);

    TestTask::Messages::RequestForFastResponse* fast_request = m.mutable_request_for_fast_response();

    //Create SecondMsg
    TestTask::Messages::WrapperMessage m1;
    fast = m1.mutable_fast_response();
    s = "20851019T050107.666";
    fast->set_current_date_time(s);
    slow_request = m1.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(1);

    fast_request = m1.mutable_request_for_fast_response();


    //FullMsg to string
    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(data->begin(), data->end());
    //Add extra bytes between msgs
    data_str.append(std::string("9blablabla"));
    //Second msg to string
    data = serializeDelimited(m1);
    data_str.append(data->begin(), data->end());
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
        Parser parser;
        std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> parsedMessages;

    //Parsing
        parsedMessages = parser.parse(data_str);
        for (auto msg : parsedMessages) {
            ASSERT_TRUE(msg->has_request_for_fast_response());
            ASSERT_TRUE(msg->has_fast_response());
            ASSERT_TRUE(msg->has_request_for_slow_response());
            ASSERT_TRUE(msg->request_for_slow_response().has_time_in_seconds_to_sleep());
        }
    EXPECT_EQ(parsedMessages.size(), 1);
}

TEST(MyOwnTestParser, DamagedMsg) {
    //Create FullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(9);

    //FullMsg to string
    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(data->begin(), data->end());
    //Damaging msg
    data_str[data_str.length()-1] = 'b';
    data_str[data_str.length()-2] = 'l';
    data_str[data_str.length()-3] = 'a';

    //Parsing
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
    Parser parser;
    EXPECT_THROW(parser.parse(data_str), std::runtime_error);
}

TEST(MyOwnTestParser, ExtraBytesBeforeAndAfterMsg) {
    //Create FullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(9);

    //FullMsg to string
    std::string data_str;
    auto data = serializeDelimited(m);
    data_str.append(std::string(2,'2'));
    data_str.append(data->begin(), data->end());
    data_str.append(std::string(2,'3'));

    //Parsing
    typedef DelimitedMessagesStreamParser<TestTask::Messages::WrapperMessage> Parser;
    Parser parser;
    std::list<std::shared_ptr<const TestTask::Messages::WrapperMessage>> parsedMessages;

    parsedMessages = parser.parse(data_str);
    EXPECT_EQ(parsedMessages.size(), 0);
}

TEST(MyOwnTestParseDelimited, FullMsg) {
    //Create FullMsg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "19851019T050107.333";
    fast->set_current_date_time(s);

    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(209);

    TestTask::Messages::RequestForSlowResponse* slow_request = m.mutable_request_for_slow_response();
    slow_request->set_time_in_seconds_to_sleep(10);

    TestTask::Messages::RequestForFastResponse* fast_request = m.mutable_request_for_fast_response();

    auto data = serializeDelimited(m);

    //Parsing
    std::shared_ptr<TestTask::Messages::WrapperMessage> result;
    size_t bytesConsumed = 0;

    result = parseDelimited<TestTask::Messages::WrapperMessage>(data->data(), data->size(), &bytesConsumed);

    ASSERT_FALSE(result == nullptr);
    ASSERT_TRUE(result->has_request_for_fast_response());
    ASSERT_TRUE(result->has_fast_response());
    ASSERT_TRUE(result->has_slow_response());
    ASSERT_TRUE(result->has_request_for_slow_response());
    ASSERT_TRUE(result->fast_response().has_current_date_time());
    ASSERT_TRUE(result->slow_response().has_connected_client_count());
    ASSERT_TRUE(result->request_for_slow_response().has_time_in_seconds_to_sleep());
    EXPECT_EQ(bytesConsumed, data->size());

}


TEST(MyOwnTestParseDelimited, NonExistentMessage)
{
  std::string data = "\x09hardlevel";
  EXPECT_THROW(parseDelimited<TestTask::Messages::WrapperMessage>(data.data(), data.size()), std::runtime_error);
}

TEST(MyOwnTestParseDelimited, NullMessage)
{
  std::shared_ptr<TestTask::Messages::WrapperMessage> result;
  std::string data = "";
  size_t bytesConsumed = 0;

  result = parseDelimited<TestTask::Messages::WrapperMessage>(data.data(), data.size(), &bytesConsumed);

  ASSERT_TRUE(result == nullptr);
  EXPECT_EQ(bytesConsumed, 0);

}

TEST(MyOwnTestParseDelimited, DamagedMsg)
{

    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::SlowResponse* slow = m.mutable_slow_response();
    slow->set_connected_client_count(9);

  auto data = serializeDelimited(m);

  std::string data_str = std::string(data->begin(), data->end());
  //Damaging msg
  data_str[data_str.length()-2] = 'a';

  EXPECT_THROW(parseDelimited<TestTask::Messages::WrapperMessage>(data_str.data(), data_str.size()), std::runtime_error);
}

TEST(MyOwnTestParseDelimited, WrongLengthPrefixedByte) {
    //Create Msg
    TestTask::Messages::WrapperMessage m;
    TestTask::Messages::FastResponse* fast = m.mutable_fast_response();
    std::string s = "19851019T050107.333";
    fast->set_current_date_time(s);

    auto data = serializeDelimited(m);
    std::string data_str = std::string(data->begin(), data->end());
    data_str[0] += 10;

    //Parsing
    std::shared_ptr<TestTask::Messages::WrapperMessage> result;
    size_t bytesConsumed = 0;

    result = parseDelimited<TestTask::Messages::WrapperMessage>(data_str.data(), data_str.size(), &bytesConsumed);

    ASSERT_TRUE(result == nullptr);
    EXPECT_EQ(bytesConsumed, 0);

}

