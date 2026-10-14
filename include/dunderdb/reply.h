#pragma once

#include "message.h"

#include <string>
#include <vector>

namespace dunderdb {

    class Reply {
    public:
        Reply(std::string status, std::string message);

        void add_row(Message row);
        void add_raw_json(std::string raw_json);

        const std::string& status();
        const std::string& reply_message();
        const std::string& raw_json();
        const std::vector<Message>& rows();

        bool successful() const;

    private:
        std::string raw_json_;
        std::string status_;
        std::string reply_message_;
        std::vector<Message> rows_;
    };

} // namespace dunderdb