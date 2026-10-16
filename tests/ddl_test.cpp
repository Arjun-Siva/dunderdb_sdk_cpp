#include <iostream>
#include <dunderdb/client.h>

#include "dunderdb/util.h"

int main() {
    dunderdb::Client client("localhost", 5555, 5556);
    client.connect();
    std::cout<<"Connected"<<std::endl;

    Schema test_schema("test_schema");
    test_schema.add_column(Column {.name = "name", .type = FieldType::STRING, .max_length = 10, .nullable = false});
    test_schema.add_column(Column {.name = "age", .type = FieldType::NUMBER, .nullable = true});

    dunderdb::Reply reply = client.create_schema(test_schema);
    std::cout<<reply.reply_message()<<std::endl;

    Schema updated_schema("test_schema");
    updated_schema.add_column(Column {.name = "name", .type = FieldType::STRING, .max_length = 20, .nullable = false});
    updated_schema.add_column(Column {.name = "age", .type = FieldType::NUMBER, .nullable = false});

    reply = client.update_schema(updated_schema);
    std::cout<<reply.reply_message()<<std::endl;

    reply = client.drop_schema("test_schema");
    std::cout<<reply.reply_message();

    return 0;
}
