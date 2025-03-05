#include "../include/error.h"
#include <iostream>

namespace UASM {

    Error::Error(size_t col, size_t row, const char* msg) {
    }
    void Error::print() {
        std::cout << error_report << std::endl;
    }

    ErrorLogger::ErrorLogger() {}

    void ErrorLogger::log(size_t col, size_t row, const char* msg) {
        if (msg != nullptr) {
            auto err = std::make_unique<Error>(col, row, msg);
            errors.push_back(std::move(err));
        }

    }
    const std::vector<std::unique_ptr<Error>>& ErrorLogger::get_errors() { return errors; }
}

