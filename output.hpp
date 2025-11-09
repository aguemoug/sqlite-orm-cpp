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
