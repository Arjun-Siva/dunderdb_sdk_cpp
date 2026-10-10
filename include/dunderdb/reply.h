#pragma once

#include "message.h"

#include <string>
#include <vector>

namespace dunderdb {

    class Reply {
    public:
        Reply(std::string status, std::string message);

        void add_row(Message row);

        const std::string& status();
        const std::string& reply_message();
        const std::vector<Message>& rows();

        bool successful() const;

    private:
        std::string status_;
        std::string reply_message_;
        std::vector<Message> rows_;
    };

} // namespace dunderdb