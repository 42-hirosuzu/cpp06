#ifndef SERIALIZER_HPP
#define SERIALIZER_HPP

#include <stdint.h>   // C++98 環境でも利用可（uintptr_t）
#include "Data.hpp"

class Serializer {
private:
    // インスタンス化・コピー不可（C++98なのでprivate宣言で封じる）
    Serializer();
    Serializer(const Serializer&);
    Serializer& operator=(const Serializer&);
    ~Serializer();

public:
    // ポインタ → 整数
    static uintptr_t serialize(Data* ptr);
    // 整数 → ポインタ
    static Data* deserialize(uintptr_t raw);
};

#endif

