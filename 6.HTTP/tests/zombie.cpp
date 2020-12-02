#include <iostream>
#include <string>
#include "connection.h"

int main() {
    tcp::Connection c({"127.0.0.1", 8080});
}
