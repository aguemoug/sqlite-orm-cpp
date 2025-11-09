# CPP-SQLite-ORM
*A Lightweight, Compile-Time ORM Code Generator for C++ and SQLite*

[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
![Python 3.7+](https://img.shields.io/badge/python-3.7%2B-blue)
![C++17+](https://img.shields.io/badge/C%2B%2B-17%2B-blue)

---

## 🚀 Overview

**CPP-SQLite-ORM** is a lightweight, **Python-based code generator** that automatically produces **type-safe, compile-time CRUD operations** for **SQLite** directly from annotated C++ `struct` definitions.  
It eliminates runtime ORM overhead while maintaining full **type safety** and **developer control**.

---

## ✨ Key Features

- ⚡ **Zero Runtime Overhead** — all database operations are generated at compile-time.  
- 🛡️ **Type-Safe Interface** — operations mirror your C++ types.  
- 🎯 **Annotation-Driven** — define database entities using intuitive C++ attributes.  
- 🔑 **Full CRUD Generation** — Insert, Select, Update, and Delete.  
- 👓 **View Support** — automatic generation for read-only SQL views.  
- 🧩 **Auto-Increment & PKs** — supports composite and single primary keys.  
- 🗄️ **Optimized for SQLite** — built on top of `sqlite_modern_cpp`.  
- 🧰 **clang-format Integration** — automatically formats generated code.  

---

## 🧩 Example Workflow

### 1️⃣ Define Your Entities

```cpp
// entities.h
#pragma once
#include <string>
#include "orm.h"
struct  TABLE(users) User {
  PK AUTOINC int id;
  std::string username;
  std::string email;
  int age;
  long created_at;
};

struct TABLE(products) Product
{
  PK int product_id;
  std::string name;
  std::string description;
  double price;
  int stock_quantity;
  bool is_available;
};

struct TABLE(orders) Order {
  PK AUTOINC int order_id;
  int user_id; // FK to users.id
  double total_amount;
  std::string status;
};

struct VIEW(user_summary) UserSummary {
   int user_id;
   std::string username;
   int order_count;
   double total_spent;
};

// This struct will be ignored
struct IGNORE InternalConfig {
  std::string config_key;
  std::string config_value;
};
```

### 2️⃣ Generate CRUD Operations

```bash
python odb.py entities.h -o crud_operations.hpp 
```

### 3️⃣ Use the Generated Code

```cpp
#include "crud_operations.hpp"
#include <sqlite3.h>

int main() {
    sqlite3* db;
    sqlite3_open(":memory:", &db);

    myapp::insertUser(db, {"john_doe", "john@example.com", 30});
    auto users = myapp::selectAllUsers(db);

    sqlite3_close(db);
}
```

---

## ⚙️ Installation

### **Dependencies**

- Python 3.7+
- `clang` & `libclang-dev`
- `clang-format`
- SQLite3 dev libraries

### **Setup**

```bash
sudo apt install python3-pip clang libclang-dev clang-format libsqlite3-dev
git clone https://github.com/yourusername/cpp-sqlite-orm.git
cd cpp-sqlite-orm
pip install -r requirements.txt
```

For macOS:

```bash
brew install llvm python3 sqlite3
export PATH="/usr/local/opt/llvm/bin:$PATH"
```

---

## 🧠 Annotation Reference

| Annotation | Type | Description | Example |
|-------------|------|-------------|----------|
| `TABLE(name)` | Entity | Marks struct as a table | `TABLE(users) struct User {...};` |
| `VIEW(name)` | Entity | Marks struct as a read-only view | `VIEW(stats) struct Stats {...};` |
| `IGNORE` | Entity | Excludes struct from generation | `IGNORE struct Debug {...};` |
| `PK` | Field | Primary key | `PK int id;` |
| `AUTOINC` | Field | Auto-increment primary key | `PK AUTOINC int id;` |
| `FK` | Field | Foreign key | `FK int user_id;` |
| `READONLY` | Field | View-only field | `READONLY int count;` |
| `IGNORE` | Field | Exclude field | `IGNORE std::string tmp;` |

---

## 🛠️ Command Line Options

```bash
python odb.py [OPTIONS] INPUT_FILE
```

| Option | Description | Default |
|--------|-------------|----------|
| `-o`, `--output` | Output file path | `output.hpp` |
| `-n`, `--namespace` | Target C++ namespace | `data` |
| `-s`, `--style` | clang-format style | `file` |
| `-v`, `--verbose` | Verbose output | Off |

---

## 🧱 Project Structure

```
cpp-sqlite-orm/
├── odb.py                 # Main code generator
├── orm.h                  # ORM helper definitions
├── templates/             # CRUD Jinja2 templates
│   ├── master.jinja
│   ├── insert.jinja
│   ├── select.jinja
│   ├── select-all.jinja
│   ├── update.jinja
│   └── delete.jinja
├── examples/
│   ├── example_entities.h
│   └── example_output.hpp
└── README.md
```

---

## 🧰 Build System Integration

### **CMake Example**

```cmake
find_package(Python3 REQUIRED)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/crud.hpp
  COMMAND Python3::Interpreter ${CMAKE_SOURCE_DIR}/odb.py
          ${CMAKE_SOURCE_DIR}/entities.h
          -o ${CMAKE_BINARY_DIR}/crud.hpp -n myapp
  DEPENDS ${CMAKE_SOURCE_DIR}/entities.h
)

add_executable(app main.cpp ${CMAKE_BINARY_DIR}/crud.hpp)
```

### **Makefile Example**

```makefile
CRUD = crud.hpp
$(CRUD): entities.h
	python3 odb.py entities.h -o $(CRUD) -n myapp
```

---

## 🧩 Advanced Customization

- **Custom clang arguments:** Add include paths or defines via `StructParser.parse_file()`.
- **Template customization:** Edit files under `templates/` for new code generation patterns.
- **Namespace and formatting:** Configure via CLI options.

---

## 🐛 Troubleshooting

1. **libclang not found**
   ```bash
   find /usr -name "libclang.so"
   python odb.py entities.h --libclang /usr/lib/llvm-14/lib/libclang.so
   ```
2. **Template not found**
   - Run the script from project root so `templates/` is visible.
3. **Parse errors**
   - Add missing include directories with `-I`.
4. **Generated code fails to compile**
   - Check type definitions and include all dependent headers.

---

## 🤝 Contributing

1. Fork the repository  
2. Create a feature branch: `git checkout -b feature/my-feature`  
3. Commit your changes  
4. Open a pull request 🚀  

---

## 📄 License

This project is released under the **MIT License** — see the [LICENSE](LICENSE) file.

---

## 🙏 Acknowledgments

- Built with **libclang** for robust C++ parsing  
- Uses **Jinja2** for flexible code generation  
- Inspired by modern ORM and metaprogramming practices in C++
