#pragma once

#include "query_request.h"
#include "reply.h"
#include "send_request.h"
#include "schema.h"

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
        Reply create_schema(const Schema& schema);
        Reply update_schema(const Schema& schema);
        Reply drop_schema(const std::string& service_name);

    private:
        std::string host_;
        int insert_port_;
        int query_port_;

        zmq::context_t context_;
        zmq::socket_t insert_socket_;
        zmq::socket_t query_socket_;

        bool connected_{false};

        Reply send_ddl_request(std::string service_name, std::string json_message);

        static std::string message_to_json(const Message& message);
        static Reply json_to_reply(const std::string& json_message);
        static std::string query_request_to_json(const QueryRequest& query_request);
        static std::string schema_to_json(const std::string& change_type, const Schema& schema);
        static std::string delete_schema_to_json(const std::string& service_name);
    };

} // namespace dunderdb