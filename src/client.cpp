#include <stdexcept>
#include <utility>

#include "dunderdb/client.h"

#include "dunderdb/util.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

namespace dunderdb
{
    Client::Client(
        std::string host,
        const int insert_port,
        const int query_port
    )
        : host_(std::move(host)),
          insert_port_(insert_port),
          query_port_(query_port),
          context_(1),
          insert_socket_(context_, zmq::socket_type::req),
          query_socket_(context_, zmq::socket_type::req) {}

    Client::~Client() = default;

    void Client::connect() {
        if (connected_) {
            throw std::runtime_error("Client is already connected");
        }

        const std::string insert_endpoint =
            "tcp://" + host_ + ":" + std::to_string(insert_port_);

        const std::string query_endpoint =
            "tcp://" + host_ + ":" + std::to_string(query_port_);

        insert_socket_.connect(insert_endpoint);
        query_socket_.connect(query_endpoint);

        connected_ = true;
    }

    Reply Client::send(const SendRequest& request) {
        if (!connected_) {
            throw std::runtime_error("Client is not connected");
        }
        constexpr uint8_t request_type = 0;

        // Serialize SendRequest
        // Send multipart ZMQ message
        // Receive server response
        // Parse response into Reply
        insert_socket_.send(
            zmq::buffer(std::to_string(request_type)),
            zmq::send_flags::sndmore
        );

        insert_socket_.send(
            zmq::buffer(request.service_name()),
            zmq::send_flags::sndmore
        );

        const auto& messages = request.messages();

        if (messages.empty()) {
            throw std::runtime_error("SendRequest is empty");
        }

        for (size_t i = 0; i < messages.size(); ++i) {
            std::string message_json = Client::message_to_json(messages[i]);

            const auto flags =
                (i + 1 < messages.size())
                    ? zmq::send_flags::sndmore
                    : zmq::send_flags::none;

            insert_socket_.send(
                zmq::buffer(message_json),
                flags
            );
        }

        zmq::message_t reply;
        auto _ = insert_socket_.recv(reply);

        const std::string json_reply(
            static_cast<char*>(reply.data()),
            reply.size());

        return Client::json_to_reply(json_reply);
    }

    Reply Client::query(const QueryRequest& request) {
        if (!connected_) {
            throw std::runtime_error("Client is not connected");
        }

        constexpr uint8_t request_type = 1;

        // Serialize QueryRequest
        // Send ZMQ request
        // Receive server response
        // Parse response into Reply
        query_socket_.send(
            zmq::buffer(std::to_string(request_type)),
            zmq::send_flags::sndmore
        );

        std::string json = Client::query_request_to_json(request);
        query_socket_.send(
            zmq::buffer(json),
            zmq::send_flags::none
        );

        zmq::message_t reply;
        auto _ = query_socket_.recv(reply);

        const std::string json_reply(
            static_cast<char*>(reply.data()),
            reply.size());

        Reply reply_obj = Client::json_to_reply(json_reply);
        reply_obj.add_raw_json(std::move(json_reply));
        return reply_obj;
    }

    Reply Client::create_schema(const Schema &schema) {
        const std::string json = Client::schema_to_json("new", schema);
        return this->send_ddl_request(schema.service_name(), std::move(json));
    }

    Reply Client::update_schema(const Schema &schema) {
        const std::string json = Client::schema_to_json("update", schema);
        return this->send_ddl_request(schema.service_name(), std::move(json));;
    }

    Reply Client::drop_schema(const std::string& service_name) {
        std::string json = Client::delete_schema_to_json(service_name);
         return this->send_ddl_request(service_name, std::move(json));
    }

    Reply Client::send_ddl_request(std::string service_name, std::string json_message) {
        if (!connected_) {
            throw std::runtime_error("Client is not connected");
        }

        constexpr uint8_t request_type = 2;

        insert_socket_.send(
            zmq::buffer(std::to_string(request_type)),
            zmq::send_flags::sndmore
        );

        insert_socket_.send(
            zmq::buffer(std::move(service_name)),
            zmq::send_flags::sndmore
        );

        insert_socket_.send(
            zmq::buffer(std::move(json_message)),
            zmq::send_flags::none
        );

        zmq::message_t reply;
        auto _ = insert_socket_.recv(reply);

        const std::string json_reply(
            static_cast<char*>(reply.data()),
            reply.size());

        Reply reply_obj = Client::json_to_reply(json_reply);
        reply_obj.add_raw_json(std::move(json_reply));
        return reply_obj;
    }

    std::string Client::message_to_json(const Message& message) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

        writer.StartObject();
        for (const auto& field : message.fields()) {
            const auto& value = field.value();

            if (std::holds_alternative<std::nullptr_t>(value)) {
                continue;
            }

            writer.Key(field.name().c_str());

            if (std::holds_alternative<double>(value)) {
                writer.Double(std::get<double>(value));
            }
            else if (std::holds_alternative<std::string>(value)) {
                writer.String(std::get<std::string>(value).c_str());
            }
        }

        writer.EndObject();

        return buffer.GetString();
    }

    Reply Client::json_to_reply(const std::string& json_message) {
        rapidjson::Document document;
        document.Parse(json_message.c_str());

        if (document.HasParseError() || !document.IsObject()) {
            throw std::runtime_error("Invalid JSON response from server");
        }

        if (!document.HasMember("status") || !document["status"].IsString()) {
            throw std::runtime_error("Invalid response: missing status");
        }

        std::string status = document["status"].GetString();

        std::string reply_message;
        if (document.HasMember("message") && document["message"].IsString()) {
            reply_message = document["message"].GetString();
        }

        Reply reply {status, reply_message};

        std::vector<Message> messages;
        if (document.HasMember("rows") && document["rows"].IsArray()) {
            for (const auto& row : document["rows"].GetArray()) {
                if (!row.IsObject()) {
                    throw std::runtime_error("Invalid response: row is not an object");
                }

                std::string timestamp;

                if (row.HasMember("timestamp") && row["timestamp"].IsString()) {
                    timestamp = row["timestamp"].GetString();
                }

                if (!row.HasMember("content") || !row["content"].IsObject()) {
                    continue;
                }

                std::vector<Field> fields;

                const auto& content = row["content"];

                for (auto it = content.MemberBegin(); it != content.MemberEnd(); ++it) {
                    const std::string field_name = it->name.GetString();
                    const auto& value = it->value;

                    if (value.IsNumber()) {
                        fields.emplace_back(field_name,value.GetDouble());
                    }
                    else if (value.IsString()) {
                        fields.emplace_back(field_name,value.GetString());
                    }
                    else if (value.IsNull()) {
                        fields.emplace_back(field_name, nullptr);
                    }
                    else {
                        throw std::runtime_error("Invalid response: unsupported field type");
                    }
                }
                reply.add_row(Message{std::move(fields), dunderdb::util::iso_string_to_time_point(timestamp)});
            } // end - row loop
        }
        return reply;
    }

    std::string Client::query_request_to_json(const QueryRequest& query_request) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();

        writer.Key("service_name");
        writer.String(query_request.service_name().c_str());

        writer.Key("start_time");
        writer.String(util::time_point_to_iso_string(query_request.start_time()).c_str());

        writer.Key("end_time");
        writer.String(util::time_point_to_iso_string(query_request.end_time()).c_str());

        writer.EndObject();
        return buffer.GetString();
    }

    std::string Client::schema_to_json(const std::string& change_type, const Schema &schema) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();

        writer.Key("change_type");
        writer.String(change_type.c_str());

        writer.Key("service_name");
        writer.String(schema.service_name().c_str());

        writer.Key("columns");
        writer.StartArray();

        for (const auto& column : schema.columns()) {
            writer.StartObject();
            writer.Key("name");
            writer.String(column.name.c_str());

            writer.Key("type");
            if (column.type == FieldType::NUMBER) {
                writer.String("number");
            }
            else {
                writer.String("string");
            }

            if (column.type == FieldType::STRING) {
                writer.Key("max_characters");
                writer.Uint(column.max_length.value());
            }

            writer.Key("nullable");
            writer.Bool(column.nullable);
            writer.EndObject();
        }
        writer.EndArray();
        writer.EndObject();

        return buffer.GetString();
    }

    std::string Client::delete_schema_to_json(const std::string &service_name) {
        rapidjson::StringBuffer buffer;
        rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
        writer.StartObject();

        writer.Key("change_type");
        writer.String("drop");

        writer.Key("service_name");
        writer.String(service_name.c_str());

        writer.EndObject();
        return buffer.GetString();
    }
} // namespace dunderdb
