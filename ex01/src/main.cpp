#include <iostream>
#include <iomanip>
#include "Serializer.hpp"

static void dumpData(const char* title, const Data* p) {
    std::cout << title << "\n";
    std::cout << "  address : " << static_cast<const void*>(p) << "\n";
    if (p) {
        std::cout << "  id      : " << p->id    << "\n"
                  << "  name    : " << p->name  << "\n"
                  << "  value   : " << p->value << "\n";
    }
}

int main() {
    // 元のオブジェクト（スタック上）
    Data d;
    d.id = 42;
    d.name = "forty-two";
    d.value = 4.2;

    dumpData("[Original]", &d);

    // ポインタ → uintptr_t
    uintptr_t raw = Serializer::serialize(&d);

    // 表示のために16進で出力（幅は環境依存なのでそのまま）
    std::ios::fmtflags f = std::cout.flags();
    std::cout << "\n[Serialized]\n"
              << "  raw     : 0x" << std::hex << raw << std::dec << "\n";
    std::cout.flags(f);

    // uintptr_t → ポインタ
    Data* p = Serializer::deserialize(raw);

    dumpData("\n[Deserialized]", p);

    // ポインタ同値の検証
    std::cout << "\n[Check]\n"
              << "  (&d == p) : " << ( (&d == p) ? "YES" : "NO" ) << "\n";

    // ついでに、逆参照で元が同一であることを確認（値を書き換える）
    if (p) {
        p->value += 1.0;
        std::cout << "  mutate via p: p->value += 1.0\n";
    }
    std::cout << "  d.value now : " << d.value << "\n";

    return 0;
}
