#ifndef SCALARCONVERTER_HPP
#define SCALARCONVERTER_HPP

#include <string>

class ScalarConverter {
private:
    // インスタンス化不可
    ScalarConverter();
    ScalarConverter(const ScalarConverter&);
    ScalarConverter& operator=(const ScalarConverter&);
    ~ScalarConverter();

public:
    static void convert(const std::string& literal);
};

#endif
