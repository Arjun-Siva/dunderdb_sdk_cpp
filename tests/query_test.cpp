#include <iostream>
#include <dunderdb/client.h>

#include "dunderdb/util.h"

int main() {
    dunderdb::Client client("localhost", 5555, 5556);
    client.connect();
    std::cout<<"Connected"<<std::endl;

    const auto now = std::chrono::system_clock::now();
    const auto before_5_mins = now - std::chrono::minutes(5);

    std::cout<<dunderdb::util::time_point_to_iso_string(now)<<", "<<dunderdb::util::time_point_to_iso_string(before_5_mins)<<std::endl;

    dunderdb::QueryRequest request("sales", before_5_mins, now);
    dunderdb::Reply reply = client.query(request);
    std::cout<<reply.status()<<std::endl;
    std::cout<<reply.reply_message()<<std::endl;
    std::cout<<reply.raw_json()<<std::endl;

    return 0;
}
