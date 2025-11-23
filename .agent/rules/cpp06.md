---
trigger: always_on
---

# Workspace Rules — 42 C++ Modules (Module 06: C++ casts)

## Project Snapshot
- Goal: 42 C++ Module課題を、評価要件を満たすC++98コードで実装・検証する。
- Priority: 仕様準拠 > コンパイル通過 > 振る舞い一致 > 可読性。
- Learning mode: ユーザーの学習を優先する。いきなり完成コードを投げず、方針→理由→最小差分の順で支援する。

## Fixed Constraints (must follow)
### Language / Compiler
- C++98準拠。必ず `c++ -Wall -Wextra -Werror -std=c++98` で通ること。  
- `-std=c++98` で落ちる記法や機能は禁止。

### Allowed / Forbidden
- 外部ライブラリは禁止（Boost / C++11以降の機能含む）。
- 禁止関数: `printf()`, `alloc*()`, `free()` などC系の低レベルI/Oやメモリ関数。  
- `using namespace ...;` と `friend` は **明示的に許可されていない限り禁止**。
- STLの **コンテナ（vector/list/map等）と <algorithm> を使うアルゴリズムは Module 08/09まで禁止**。  
  - 例: `<vector> <list> <map> <set> <algorithm> <iterator> <numeric>` は使わない。
  - `std::string`, `<iostream>`, `<sstream>`, `<limits>`, `<cstdlib>` などC++98標準の基礎は可。

### Design Requirements
- `new` を使うならリーク厳禁。寿命管理を明確に。
- Module 02〜09は **Orthodox Canonical Form（O.C.F.）必須**。  
  - ただし **課題で例外が明記されている場合はその指示を優先**。  
  - Module 06 ex02はO.C.F不要。
- ヘッダに関数実装を書かない（テンプレ除く）。書いたら0点。
- 各ヘッダは単独インクルードでコンパイルできること。  
  - include guards必須。二重インクルード対策。

### Formatting / Naming
- ディレクトリ名は `ex00/ ex01/ ex02/`。
- クラス名は UpperCamelCase。ファイル名はクラス名と一致させる。  
  - 例: `ScalarConverter.hpp / ScalarConverter.cpp`
- 出力は標準出力、各行は改行で終わること。

## Exercise-specific Notes (Module 06)
### ex00 — ScalarConverter
- `ScalarConverter` は **staticメソッド `convert(string)` のみ**持ち、**インスタンス生成不可**にする。
- 入力のリテラル種別を判定→実型に変換→他3型へ **明示的キャスト**して表示。
- 擬似リテラル対応:
  - float: `-inff +inff nanf`
  - double: `-inf +inf nan`
- char変換:
  - 表示不能なら `"Non displayable"`  
  - 変換不可能なら `"impossible"`
- overflowや意味不明の変換は `"impossible"` を出す。

### ex01 — Serializer
- `Serializer` は **staticメソッドのみ**、生成不可。
- `uintptr_t serialize(Data* ptr);`
- `Data* deserialize(uintptr_t raw);`
- `Data` 構造体は **空でない**こと（メンバあり）。
- serialize→deserialize後のポインタ一致をテストで確認。

### ex02 — Identify real type
- `Base` は public virtual destructor のみ。
- `A/B/C` は空クラスで `Base` をpublic継承。
- `std::typeinfo` および `#include <typeinfo>` 禁止。
- `generate()` は A/B/C をランダム生成し Base* で返す。
- `identify(Base*)` と `identify(Base&)` で実型名 `A/B/C` を出力。
  - 参照版は **ポインタ使用禁止**。

## Agent Behavior (how you should help)
- まず課題PDFと既存コードを読んで、**やるべき仕様を箇条書きで再確認**してから実装に入る。
- 大きな変更はしない。**最小差分で通す**方針を優先。
- 仕様に曖昧さがあれば、先に「解釈の候補」を示す（勝手に決め打ちしない）。
- 実装提案はこの順で:
  1) 型判定ロジックの設計  
  2) 例外/擬似リテラル/境界の扱い  
  3) キャストの種類選定（static_cast / reinterpret_cast / dynamic_cast 等）  
  4) 出力フォーマット  
  5) テストケース
- ユーザーが自分の案やコードを出した場合、**ダメ出し→修正案→理由**の順で短く返す。

## Commands / Validation
- Build: `make`
- Rebuild: `make re`
- 必須チェック:
  - `-Wall -Wextra -Werror -std=c++98` で全exが通る
  - 禁止ヘッダ/禁止関数なし
  - サンプル入出力が一致
  - リーク/未定義動作の疑いがない

## Definition of Done
- 42の必須制約をすべて満たす。
- 各exの実行テストで期待出力が出る。
- 防衛（口頭説明）で「なぜそのキャストを使ったか」説明できる構造になっている。
