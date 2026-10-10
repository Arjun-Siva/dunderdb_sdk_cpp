#include <iostream>
#include <dunderdb/client.h>

int main() {
    dunderdb::Client client("localhost", 5555, 5556);
    client.connect();
    std::cout<<"Connected"<<std::endl;

    dunderdb::SendRequest request("sales");

    dunderdb::Field item("item", "clock");
    dunderdb::Field price("price", 23.5);
    dunderdb::Field category("category", "electronics");

    dunderdb::Message message({item, category, price});
    request.add_message(message);

    dunderdb::Reply reply = client.send(request);
    std::cout<<reply.status()<<std::endl;
    std::cout<<reply.reply_message()<<std::endl;

    return 0;
}