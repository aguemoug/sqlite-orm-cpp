#pragma once
#include "include/orm.h"
#include <string>

namespace data {

struct

    TABLE(users) User {
  PK AUTOINC int id;
  std::string username;
  std::string email;
  int age;
  long created_at;
};

struct Product TABLE(products) {
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
  READONLY int user_id;
  READONLY std::string username;
  READONLY int order_count;
  READONLY double total_spent;
};

// This struct will be ignored
struct IGNORE InternalConfig {
  std::string config_key;
  std::string config_value;
};
} // namespace data
