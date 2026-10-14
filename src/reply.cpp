#include "dunderdb/reply.h"

namespace dunderdb
{
    Reply::Reply(std::string status, std::string message)
        : status_(std::move(status)),
          reply_message_(std::move(message)) {}

    void Reply::add_row(Message row) {
        rows_.push_back(std::move(row));
    }

    void Reply::add_raw_json(std::string raw_json) {
        raw_json_ = std::move(raw_json);
    }

    const std::string& Reply::status() {
        return status_;
    }

    const std::string& Reply::reply_message() {
        return reply_message_;
    }

    const std::string & Reply::raw_json() {
        return raw_json_;
    }

    const std::vector<Message>& Reply::rows() {
        return rows_;
    }

    bool Reply::successful() const {
        return status_ == "ok";
    }
} // namespace dunderdb
