#pragma once
// ===================================================
// Auto-generated SOCI mapping and CRUD operations
// Generated on: 2025-11-16 08:58:13
// Source: entities.h
// Tables: User, Product, Order
// Views: UserSummary
// Enums: UserStatus
// ===================================================

#include "entities.h"
#include <iomanip>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>

// SOCI Type mapping
namespace soci {
using namespace data;

// -----------------------------------------------------------

// Mapping for bool Fields
// bool attributes will be mapped to int columns in database
template <> struct type_conversion<bool> {
  typedef int base_type;

  static void from_base(int const &v, indicator i /* ind */, bool &e) { e = v; }

  static void to_base(const bool &e, int &v, indicator &ind) {
    v = e;
    ind = i_ok;
  }
};

// Mapping for string Fields
// NULL database columns will be mapped to empty strings
template <> struct type_conversion<std::string> {
  typedef std::string base_type;

  static void from_base(std::string const &v, indicator i /* ind */,
                        std::string &e) {
    e = i == i_null ? "" : v;
  }

  static void to_base(const std::string &e, std::string &v, indicator &ind) {
    ind = v.empty() ? i_null : i_ok;
    v = e;
  }
};

// Entity : User  <==>  Database
template <> struct type_conversion<User> {
  typedef values base_type;

  static void from_base(values const &v, indicator /* ind */, User &e) {

    e.id = v.get<int>("id");

    e.first_name = v.get<std::string>("first_name");

    e.last_name = v.get<std::string>("last_name");

    e.age = v.get<int>("age");

    e.username = v.get<std::string>("username");

    e.email = v.get<std::string>("email");

    e.status = static_cast<UserStatus>(v.get<int>("status"));

    e.created_at = v.get<long>("created_at");
  }

  static void to_base(const User &e, values &v, indicator &ind) {

    v.set("id", e.id);

    v.set("first_name", e.first_name);

    v.set("last_name", e.last_name);

    v.set("age", e.age);

    v.set("username", e.username);

    v.set("email", e.email);
    v.set("status", static_cast<int>(e.status));

    v.set("created_at", e.created_at);
    ind = i_ok;
  }
};

// Entity : Product  <==>  Database
template <> struct type_conversion<Product> {
  typedef values base_type;

  static void from_base(values const &v, indicator /* ind */, Product &e) {

    e.product_id = v.get<int>("product_id");

    e.name = v.get<std::string>("name");

    e.description = v.get<std::string>("description");

    e.price = v.get<double>("price");

    e.stock_quantity = v.get<int>("stock_quantity");

    e.is_available = v.get<bool>("is_available");
  }

  static void to_base(const Product &e, values &v, indicator &ind) {

    v.set("product_id", e.product_id);

    v.set("name", e.name);

    v.set("description", e.description);

    v.set("price", e.price);

    v.set("stock_quantity", e.stock_quantity);

    v.set("is_available", e.is_available);
    ind = i_ok;
  }
};

// Entity : Order  <==>  Database
template <> struct type_conversion<Order> {
  typedef values base_type;

  static void from_base(values const &v, indicator /* ind */, Order &e) {

    e.order_id = v.get<int>("order_id");

    e.user_id = v.get<int>("user_id");

    e.total_amount = v.get<double>("total_amount");

    e.status = v.get<std::string>("status");
  }

  static void to_base(const Order &e, values &v, indicator &ind) {

    v.set("order_id", e.order_id);

    v.set("user_id", e.user_id);

    v.set("total_amount", e.total_amount);

    v.set("status", e.status);
    ind = i_ok;
  }
};

// Entity : UserSummary  <==>  Database
template <> struct type_conversion<UserSummary> {
  typedef values base_type;

  static void from_base(values const &v, indicator /* ind */, UserSummary &e) {

    e.user_id = v.get<int>("user_id");

    e.username = v.get<std::string>("username");

    e.order_count = v.get<int>("order_count");

    e.total_spent = v.get<double>("total_spent");
  }

  static void to_base(const UserSummary &e, values &v, indicator &ind) {

    v.set("user_id", e.user_id);

    v.set("username", e.username);

    v.set("order_count", e.order_count);

    v.set("total_spent", e.total_spent);
    ind = i_ok;
  }
};

} // namespace soci

namespace data {

//-----------------------------------------------------------
template <typename T, typename... Args>
std::vector<T> getMultipleQuery(soci::session &sql, const std::string &query,
                                Args... args) {
  T r;
  std::vector<T> objs;
  soci::statement st(sql);
  // Expand and bind all arguments
  (st.exchange(soci::use(args)), ...);
  st.alloc();
  st.prepare(query);
  st.define_and_bind();
  // after define_and_bind and before execute
  st.exchange_for_rowset(soci::into(r));
  st.execute(false);
  // Convert to rowset and use directly
  soci::rowset_iterator<T> it(st, r);
  soci::rowset_iterator<T> end;
  for (; it != end; ++it) {
    objs.push_back(*it);
  }
  return objs;
}

template <typename T, typename... Args>
T getSingleleQuery(soci::session &sql, const std::string &query, Args... args) {
  T r;
  soci::statement st(sql);
  (st.exchange(soci::use(args)), ...);
  st.exchange(soci::into(r));
  st.alloc();
  st.prepare(query);
  st.define_and_bind();
  st.execute(true);
  return r;
}

template <typename... Args>
bool singleObjectOperation(soci::session &sql, const std::string &query,
                           Args &&...args) {
  soci::statement st(sql);
  (st.exchange(soci::use(args)), ...);
  st.alloc();
  st.prepare(query);
  st.define_and_bind();
  st.execute(true);
  return true;
}

//-----------------------------------------------------------

template <typename T, size_t N, bool IsView = false> struct EntityMetadataBase {
  using type = T;
  static constexpr bool is_view = IsView;
  static constexpr size_t field_count = N;
};

template <typename T> struct EntityMetadata;

template <typename T> std::ostream &print_header(std::ostream &os) {
  std::apply(
      [&](auto... m) {
        ((os << std::left << std::setw(std::get<2>(m)) << std::get<0>(m)), ...);
      },
      EntityMetadata<T>::members);
  return os;
}

template <typename T> std::ostream &print_row(std::ostream &os, const T &obj) {
  std::apply(
      [&](auto... m) {
        ((os << std::left << std::setw(std::get<2>(m))
             << obj.*(std::get<1>(m))),
         ...);
      },
      EntityMetadata<T>::members);
  return os;
}
template <typename T> void printHeader();

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) {
  print_header<T>(os) << "\n";
  for (const auto &item : vec)
    print_row(os, item) << '\n';
  return os;
}

template <typename E> struct enum_traits; // user specializes this per enum
template <typename E>
std::enable_if_t<std::is_enum_v<E>, std::ostream &> operator<<(std::ostream &os,
                                                               E e) {
  if constexpr (requires { enum_traits<E>::names; }) {
    // name table available
    auto val = static_cast<std::size_t>(e);
    if (val < enum_traits<E>::names.size())
      return os << enum_traits<E>::names[val];
    else
      return os << "Unknown(" << val << ")";
  } else {
    // fallback: print as integer
    return os << static_cast<std::underlying_type_t<E>>(e);
  }
}

template <> struct enum_traits<UserStatus> {
  static constexpr std::array<const char *, 3> names = {"ACTIVE", "INACTIVE",
                                                        "BANNED"};
};

//-------------------------------------------------------------

// Insert
template <typename T> bool insert(soci::session &sql, const T &obj);

template <> bool insert<User>(soci::session &sql, const User &obj) {
  return singleObjectOperation(sql, "INSERT INTO users () VALUES (:)", obj);
}

template <> bool insert<Product>(soci::session &sql, const Product &obj) {
  return singleObjectOperation(sql, "INSERT INTO products () VALUES (:)", obj);
}

template <> bool insert<Order>(soci::session &sql, const Order &obj) {
  return singleObjectOperation(sql, "INSERT INTO orders () VALUES (:)", obj);
}

// Select by ID
template <typename T, typename IdType>
T selectById(soci::session &sql, const IdType &id);

// Select all

template <typename T> std::vector<T> selectAll(soci::session &sql);

template <> std::vector<User> selectAll<User>(soci::session &sql) {
  return getMultipleQuery<User>(
      sql, "SELECT id, first_name, last_name, age, username, email, status, "
           "created_at FROM users ");
}

template <> std::vector<Product> selectAll<Product>(soci::session &sql) {
  return getMultipleQuery<Product>(
      sql, "SELECT product_id, name, description, price, stock_quantity, "
           "is_available FROM products ");
}

template <> std::vector<Order> selectAll<Order>(soci::session &sql) {
  return getMultipleQuery<Order>(
      sql, "SELECT order_id, user_id, total_amount, status FROM orders ");
}

template <>
std::vector<UserSummary> selectAll<UserSummary>(soci::session &sql) {
  return getMultipleQuery<UserSummary>(
      sql,
      "SELECT user_id, username, order_count, total_spent FROM user_summary ");
}

//---------------------Metadata Functions---------------------/

template <> struct EntityMetadata<User> : EntityMetadataBase<User, 8, false> {
  static constexpr const char *table_name = "users";
  static constexpr auto members =
      std::make_tuple(std::tuple{"id", &type::id, 20u, ""},
                      std::tuple{"first_name", &type::first_name, 20u, ""},
                      std::tuple{"last_name", &type::last_name, 20u, ""},
                      std::tuple{"age", &type::age, 20u, ""},
                      std::tuple{"username", &type::username, 20u, ""},
                      std::tuple{"email", &type::email, 20u, ""},
                      std::tuple{"status", &type::status, 20u, ""},
                      std::tuple{"created_at", &type::created_at, 20u, ""});
};

template <>
struct EntityMetadata<Product> : EntityMetadataBase<Product, 6, false> {
  static constexpr const char *table_name = "products";
  static constexpr auto members = std::make_tuple(
      std::tuple{"product_id", &type::product_id, 12u, ""},
      std::tuple{"name", &type::name, 20u, ""},
      std::tuple{"description", &type::description, 20u, ""},
      std::tuple{"price", &type::price, 20u, ""},
      std::tuple{"stock_quantity", &type::stock_quantity, 20u, ""},
      std::tuple{"is_available", &type::is_available, 20u, ""});
};

template <> struct EntityMetadata<Order> : EntityMetadataBase<Order, 4, false> {
  static constexpr const char *table_name = "orders";
  static constexpr auto members =
      std::make_tuple(std::tuple{"order_id", &type::order_id, 10u, ""},
                      std::tuple{"user_id", &type::user_id, 20u, ""},
                      std::tuple{"total_amount", &type::total_amount, 20u, ""},
                      std::tuple{"status", &type::status, 20u, ""});
};

template <>
struct EntityMetadata<UserSummary> : EntityMetadataBase<UserSummary, 4, false> {
  static constexpr const char *table_name = "user_summary";
  static constexpr auto members =
      std::make_tuple(std::tuple{"user_id", &type::user_id, 20u, ""},
                      std::tuple{"username", &type::username, 20u, ""},
                      std::tuple{"order_count", &type::order_count, 20u, ""},
                      std::tuple{"total_spent", &type::total_spent, 20u, ""});
};

} // namespace data