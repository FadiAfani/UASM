#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <memory>
#include <vector>

namespace UASM {

    class Error {
        private:
            std::string error_report;
        public:
            void print();
            Error(size_t col, size_t row, const char* msg);
    };

    class ErrorLogger {
        private:
            std::vector<std::unique_ptr<Error>> errors;
        public:
            ErrorLogger();
            void log(size_t col, size_t row, const char* msg);
    };

}



#endif
