#include <iostream>
#include "httpservice.h"

class TestListener : public http::IHttpListener {
    void OnRequest(http::HttpConnection& cn) {
        std::cout << cn.adress() <<
    }
};
