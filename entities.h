#pragma once
#include "orm.h"
#include <string>

namespace data
{

enum class UserStatus
{
  ACTIVE = 1,
  INACTIVE = 0,
  BANNED = -1
};

struct BaseTableEntity
{
  PK AUTOINC int id;
};

struct BaseViewEntity
{
};

struct Person
{
  std::string first_name;
  std::string last_name;
  int age;
};
struct TABLE(users) User : Person, BaseTableEntity
{
  std::string username;
  std::string email;
  UserStatus status;
  long created_at;
};

struct TABLE(products) Product
{
  WIDTH(12) PK int product_id;
  std::string name;
  std::string description;
  double price;
  int stock_quantity;
  bool is_available;
};

struct TABLE(orders) Order
{
  WIDTH(10) PK AUTOINC int order_id;
  int user_id;  // FK to users.id
  double total_amount;
  std::string status;
};

struct VIEW(user_summary) UserSummary
{
  READONLY int user_id;
  READONLY std::string username;
  READONLY int order_count;
  READONLY double total_spent;
};

// This struct will be ignored
struct IGNORE InternalConfig
{
  std::string config_key;
  std::string config_value;
};
}  // namespace data
