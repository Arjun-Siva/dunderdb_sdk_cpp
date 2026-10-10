#pragma once

#include "query_request.h"
#include "reply.h"
#include "send_request.h"

#include <string>
#include <zmq.hpp>

namespace dunderdb {

    class Client {
    public:
        Client(
            std::string host,
            int insert_port = 5555,
            int query_port = 5556
        );
        // TODO: implement timeout

        ~Client();

        void connect();

        Reply send(const SendRequest& request);
        Reply query(const QueryRequest& request);

    private:
        std::string host_;
        int insert_port_;
        int query_port_;

        zmq::context_t context_;
        zmq::socket_t insert_socket_;
        zmq::socket_t query_socket_;

        bool connected_{false};

        static std::string message_to_json(const Message& message);
        static Reply json_to_reply(const std::string& json_message);
        static std::string query_request_to_json(const QueryRequest& query_request);
    };

} // namespace dunderdb