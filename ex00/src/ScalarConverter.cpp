#include "ScalarConverter.hpp"

#include <iostream>
#include <iomanip>
#include <limits>
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <cmath>
#include <cerrno>

namespace {

// 文字列ユーティリティ（C++98なので簡易実装）
std::string trim(const std::string& s) {
    std::string::size_type b = 0, e = s.size();
    while (b < e && std::isspace(static_cast<unsigned char>(s[b]))) ++b;
    while (e > b && std::isspace(static_cast<unsigned char>(s[e-1]))) --e;
    return s.substr(b, e - b);
}

bool isCharLiteral(const std::string& s, char& out) {
    if (s.size() == 3 && s[0] == '\'' && s[2] == '\'') {
        out = s[1];
        return true;
    }
    // 便宜的に 1 文字・非数字を char とみなす（42系の慣習対応）
    if (s.size() == 1 && !std::isdigit(static_cast<unsigned char>(s[0]))) {
        out = s[0];
        return true;
    }
    return false;
}

bool isIntLiteral(const std::string& s) {
    if (s.empty()) return false;
    std::size_t i = 0;
    if (s[i] == '+' || s[i] == '-') ++i;
    if (i >= s.size()) return false;
    bool hasDigit = false;
    for (; i < s.size(); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(s[i]))) return false;
        hasDigit = true;
    }
    return hasDigit;
}

bool isFloatPseudo(const std::string& s) {
    return s == "nanf" || s == "+inff" || s == "-inff";
}

bool isDoublePseudo(const std::string& s) {
    return s == "nan" || s == "+inf" || s == "-inf";
}

bool isFloatLiteral(const std::string& s) {
    // [-+]? [digits]? '.' [digits]? 'f'  （少なくともどこかに数字を含む）
    if (s.size() < 2) return false;
    if (s[s.size()-1] != 'f') return false;
    std::size_t i = 0;
    if (s[i] == '+' || s[i] == '-') ++i;
    bool hasDigit = false, hasDot = false;
    for (; i < s.size()-1; ++i) {
        char c = s[i];
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDot && hasDigit;
}

bool isDoubleLiteral(const std::string& s) {
    // [-+]? [digits]? '.' [digits]?  （少なくともどこかに数字）
    std::size_t i = 0;
    if (s[i] == '+' || s[i] == '-') ++i;
    bool hasDigit = false, hasDot = false;
    for (; i < s.size(); ++i) {
        char c = s[i];
        if (c == '.') {
            if (hasDot) return false;
            hasDot = true;
        } else if (std::isdigit(static_cast<unsigned char>(c))) {
            hasDigit = true;
        } else {
            return false;
        }
    }
    return hasDot && hasDigit;
}

bool isIntegral(double v) {
    if (!std::isfinite(v)) return false;
    double iv = std::floor(v);
    return (v == iv);
}

void printCharFromIntLike(long v, bool possible) {
    if (!possible) { std::cout << "char: impossible\n"; return; }
    if (v < 0 || v > 127) { std::cout << "char: impossible\n"; return; }
    char c = static_cast<char>(v);
    if (!std::isprint(static_cast<unsigned char>(c))) {
        std::cout << "char: Non displayable\n";
    } else {
        std::cout << "char: '" << c << "'\n";
    }
}

void printCharFromDouble(double v) {
    if (!std::isfinite(v) || v < 0.0 || v > 127.0) {
        std::cout << "char: impossible\n";
        return;
    }
    char c = static_cast<char>(static_cast<int>(v));
    if (!std::isprint(static_cast<unsigned char>(c))) {
        std::cout << "char: Non displayable\n";
    } else {
        std::cout << "char: '" << c << "'\n";
    }
}

void printIntFromDouble(double v) {
    if (!std::isfinite(v) ||
        v < static_cast<double>(std::numeric_limits<int>::min()) ||
        v > static_cast<double>(std::numeric_limits<int>::max())) {
        std::cout << "int: impossible\n";
        return;
    }
    int i = static_cast<int>(v);
    std::cout << "int: " << i << "\n";
}

void printFloat(double v) {
    if (std::isnan(v)) {
        std::cout << "float: nanf\n";
        return;
    }
    if (std::isinf(v)) {
        if (v > 0) std::cout << "float: +inff\n";
        else       std::cout << "float: -inff\n";
        return;
    }
    float f = static_cast<float>(v);
    if (isIntegral(f)) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "float: " << f << "f\n";
        std::cout.unsetf(std::ios::fixed);
        std::cout << std::setprecision(6);
    } else {
        // 既定精度（6桁）で十分
        std::cout << "float: " << f << "f\n";
    }
}

void printDouble(double v) {
    if (std::isnan(v)) {
        std::cout << "double: nan\n";
        return;
    }
    if (std::isinf(v)) {
        if (v > 0) std::cout << "double: +inf\n";
        else       std::cout << "double: -inf\n";
        return;
    }
    if (isIntegral(v)) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "double: " << v << "\n";
        std::cout.unsetf(std::ios::fixed);
        std::cout << std::setprecision(6);
    } else {
        std::cout << "double: " << v << "\n";
    }
}

} // unnamed namespace

// ===== ScalarConverter =====
ScalarConverter::ScalarConverter() {}
ScalarConverter::ScalarConverter(const ScalarConverter&) {}
ScalarConverter& ScalarConverter::operator=(const ScalarConverter&) { return *this; }
ScalarConverter::~ScalarConverter() {}

void ScalarConverter::convert(const std::string& arg) {
    const std::string s = trim(arg);

    // 1) char
    char ch;
    if (isCharLiteral(s, ch)) {
        int i = static_cast<int>(ch);
        std::cout << "char: ";
        if (!std::isprint(static_cast<unsigned char>(ch))) {
            std::cout << "Non displayable\n";
        } else {
            std::cout << "'" << ch << "'\n";
        }
        std::cout << "int: " << i << "\n";
        printFloat(static_cast<double>(i));
        printDouble(static_cast<double>(i));
        return;
    }

    // 2) float pseudo / double pseudo
    if (isFloatPseudo(s)) {
        // nanf, +inff, -inff
        double v;
        if (s == "nanf") v = std::numeric_limits<double>::quiet_NaN();
        else if (s == "+inff") v =  std::numeric_limits<double>::infinity();
        else v = -std::numeric_limits<double>::infinity();

        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        printFloat(v);
        printDouble(v);
        return;
    }
    if (isDoublePseudo(s)) {
        double v;
        if (s == "nan") v = std::numeric_limits<double>::quiet_NaN();
        else if (s == "+inf") v =  std::numeric_limits<double>::infinity();
        else v = -std::numeric_limits<double>::infinity();

        std::cout << "char: impossible\n";
        std::cout << "int: impossible\n";
        printFloat(v);
        printDouble(v);
        return;
    }

    // 3) float literal（末尾 f ）
    if (isFloatLiteral(s)) {
        // strtod は 'f' を読めないので、末尾 f を外してから読む
        std::string t = s.substr(0, s.size() - 1);
        char* end = 0;
        errno = 0;
        double dv = std::strtod(t.c_str(), &end);
        bool ok = (end && *end == '\0' && errno == 0);

        if (!ok) {
            std::cout << "char: impossible\n"
                      << "int: impossible\n"
                      << "float: impossible\n"
                      << "double: impossible\n";
            return;
        }
        printCharFromDouble(dv);
        printIntFromDouble(dv);
        printFloat(dv);
        printDouble(dv);
        return;
    }

    // 4) double literal（小数点あり）
    if (isDoubleLiteral(s)) {
        char* end = 0;
        errno = 0;
        double dv = std::strtod(s.c_str(), &end);
        bool ok = (end && *end == '\0' && errno == 0);

        if (!ok) {
            std::cout << "char: impossible\n"
                      << "int: impossible\n"
                      << "float: impossible\n"
                      << "double: impossible\n";
            return;
        }
        printCharFromDouble(dv);
        printIntFromDouble(dv);
        printFloat(dv);
        printDouble(dv);
        return;
    }

    // 5) int literal
    if (isIntLiteral(s)) {
        char* end = 0;
        errno = 0;
        long lv = std::strtol(s.c_str(), &end, 10);
        bool ok = (end && *end == '\0' && errno == 0);

        if (!ok ||
            lv < static_cast<long>(std::numeric_limits<int>::min()) ||
            lv > static_cast<long>(std::numeric_limits<int>::max())) {
            std::cout << "char: impossible\n"
                      << "int: impossible\n"
                      << "float: impossible\n"
                      << "double: impossible\n";
            return;
        }

        // 出力
        printCharFromIntLike(lv, true);
        std::cout << "int: " << static_cast<int>(lv) << "\n";
        printFloat(static_cast<double>(lv));
        printDouble(static_cast<double>(lv));
        return;
    }

    // ここまで当てはまらない場合
    std::cout << "char: impossible\n"
              << "int: impossible\n"
              << "float: impossible\n"
              << "double: impossible\n";
}
