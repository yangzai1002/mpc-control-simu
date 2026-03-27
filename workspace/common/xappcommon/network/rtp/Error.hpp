#pragma once

#include <stdexcept>

class ExceptionParseNalus : public std::runtime_error {
    using std::runtime_error::runtime_error;
};