#include "neterr.h"

net::EPollError::EPollError(const std::string& what)
    : Error(what) {}
