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

### 3️⃣ Example of generated code

```cpp
#pragma once
// ===================================================
// Auto-generated CRUD operations
// Generated on: 2025-11-09 13:55:08
// Source: entities.h
// Tables: User, Order
// Views: UserSummary
// ===================================================

#include "entities.h"
#include <sqlite_modern_cpp.h>
#include <string>
#include <vector>

using namespace sqlite;
namespace data {

// ============================================================================
// User TABLE CRUD Operations
// Target: users
// ============================================================================

// User Insert function
inline void insert(sqlite::database &db, const User &obj) {
  db << "INSERT INTO users (username, email, age, created_at)  VALUES (?, ?, "
        "?, ?);"

     << obj.username << obj.email << obj.age << obj.created_at;
}

// User Select All function
inline std::vector<User> selectAll(sqlite::database &db) {
  std::vector<User> results;
  db << "SELECT id, username, email, age, created_at FROM users;" >>
      [&](int id, std::string username, std::string email, int age,
          long created_at) { results.emplace_back(); };
  return results;
}

// User Select By ID function
inline User selectById(sqlite::database &db, int id) {
  User obj;
  db << "SELECT id, username, email, age, created_at FROM users WHERE id = ?;"

     << id

      >> [&](int id, std::string username, std::string email, int age,
             long created_at) {
          obj.id = id;
          obj.username = username;
          obj.email = email;
          obj.age = age;
          obj.created_at = created_at;
        };
  return obj;
}

// User Update function
inline void update(sqlite::database &db, const User &obj) {
  db << "UPDATE users SET id =?, username =?, email =?, age =?, created_at = ? "
        "WHERE id = ?;"

     << obj.id << obj.username << obj.email << obj.age << obj.created_at

     << obj.id;
}

// User Delete function
inline void remove(sqlite::database &db, int id) {
  db << "DELETE FROM users WHERE id = ?;"

     << id;
}

// End of User table operations
// ============================================================================

// ============================================================================
// Order TABLE CRUD Operations
// Target: orders
// ============================================================================

// Order Insert function
inline void insert(sqlite::database &db, const Order &obj) {
  db << "INSERT INTO orders (user_id, total_amount, status)  VALUES (?, ?, ?);"

     << obj.user_id << obj.total_amount << obj.status;
}

// Order Select All function
inline std::vector<Order> selectAll(sqlite::database &db) {
  std::vector<Order> results;
  db << "SELECT order_id, user_id, total_amount, status FROM orders;" >>
      [&](int order_id, int user_id, double total_amount, std::string status) {
        results.emplace_back();
      };
  return results;
}

// Order Select By ID function
inline Order selectById(sqlite::database &db, int order_id) {
  Order obj;
  db << "SELECT order_id, user_id, total_amount, status FROM orders WHERE "
        "order_id = ?;"

     << order_id

      >>
      [&](int order_id, int user_id, double total_amount, std::string status) {
        obj.order_id = order_id;
        obj.user_id = user_id;
        obj.total_amount = total_amount;
        obj.status = status;
      };
  return obj;
}

// Order Update function
inline void update(sqlite::database &db, const Order &obj) {
  db << "UPDATE orders SET order_id =?, user_id =?, total_amount =?, status = "
        "? WHERE order_id = ?;"

     << obj.order_id << obj.user_id << obj.total_amount << obj.status

     << obj.order_id;
}

// Order Delete function
inline void remove(sqlite::database &db, int order_id) {
  db << "DELETE FROM orders WHERE order_id = ?;"

     << order_id;
}

// End of Order table operations
// ============================================================================

// ============================================================================
// UserSummary VIEW Operations
// Target: user_summary
// ============================================================================

// UserSummary Select All function
inline std::vector<UserSummary> selectAll(sqlite::database &db) {
  std::vector<UserSummary> results;
  db << "SELECT user_id, username, order_count, total_spent FROM "
        "user_summary;" >>
      [&](int user_id, std::string username, int order_count,
          double total_spent) { results.emplace_back(); };
  return results;
}

// End of UserSummary view operations
// ============================================================================

} // namespace data
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
