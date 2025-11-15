#pragma once
// ===================================================
// Auto-generated SOCI mapping and CRUD operations
// Generated on: 2025-11-15 21:27:09
// Source: entities.h
// Tables: Wilaya, Commune, JobFRCompaxt, Job
// Views: EmployeeFR
// Enums: Gender, AccountType, ContractType, PaymentMode, JobbingPaymentMode,
// SalaryMode
// ===================================================

#include "entities.h"
#include <iomanip>
#include <soci/soci.h>
#include <soci/sqlite3/soci-sqlite3.h>
#include <vector>

// SOCI Type mapping
namespace soci
{
using namespace data;

// -----------------------------------------------------------

// Mapping for bool Fields
// bool attributes will be mapped to int columns in database
template <>
struct type_conversion<bool>
{
  typedef int base_type;

  static void from_base(int const& v, indicator i /* ind */, bool& e) { e = v; }

  static void to_base(const bool& e, int& v, indicator& ind)
  {
    v = e;
    ind = i_ok;
  }
};

// Mapping for string Fields
// NULL database columns will be mapped to empty strings
template <>
struct type_conversion<std::string>
{
  typedef std::string base_type;

  static void from_base(std::string const& v, indicator i /* ind */, std::string& e) { e = i == i_null ? "" : v; }

  static void to_base(const std::string& e, std::string& v, indicator& ind)
  {
    ind = v.empty() ? i_null : i_ok;
    v = e;
  }
};

// Entity : Wilaya  <==>  Database
template <>
struct type_conversion<Wilaya>
{
  typedef values base_type;

  static void from_base(values const& v, indicator /* ind */, Wilaya& e)
  {
    e.id = v.get<IntegerT>("id");

    e.name = v.get<StringT>("name");

    e.name_ar = v.get<StringT>("name_ar");
  }

  static void to_base(const Wilaya& e, values& v, indicator& ind)
  {
    v.set("id", e.id);

    v.set("name", e.name);

    v.set("name_ar", e.name_ar);
    ind = i_ok;
  }
};

// Entity : Commune  <==>  Database
template <>
struct type_conversion<Commune>
{
  typedef values base_type;

  static void from_base(values const& v, indicator /* ind */, Commune& e)
  {
    e.id = v.get<IntegerT>("id");

    e.name = v.get<StringT>("name");

    e.name_ar = v.get<StringT>("name_ar");

    e.wilaya_id = v.get<IntegerT>("wilaya_id");
  }

  static void to_base(const Commune& e, values& v, indicator& ind)
  {
    v.set("id", e.id);

    v.set("name", e.name);

    v.set("name_ar", e.name_ar);

    v.set("wilaya_id", e.wilaya_id);
    ind = i_ok;
  }
};

// Entity : JobFRCompaxt  <==>  Database
template <>
struct type_conversion<JobFRCompaxt>
{
  typedef values base_type;

  static void from_base(values const& v, indicator /* ind */, JobFRCompaxt& e)
  {
    e.id = v.get<IntegerT>("id");

    e.short_title = v.get<StringT>("short_title");

    e.title = v.get<StringT>("title");

    e.hours_per_day = v.get<RealT>("hours_per_day");

    e.days_per_month = v.get<RealT>("days_per_month");

    e.declared_salary = v.get<RealT>("declared_salary");

    e.day_salary = v.get<RealT>("day_salary");

    e.month_salary = v.get<RealT>("month_salary");

    e.extra_hours_salary = v.get<RealT>("extra_hours_salary");

    e.salary_mode = static_cast<SalaryMode>(v.get<int>("salary_mode"));
  }

  static void to_base(const JobFRCompaxt& e, values& v, indicator& ind)
  {
    v.set("id", e.id);

    v.set("short_title", e.short_title);

    v.set("title", e.title);

    v.set("hours_per_day", e.hours_per_day);

    v.set("days_per_month", e.days_per_month);

    v.set("declared_salary", e.declared_salary);

    v.set("day_salary", e.day_salary);

    v.set("month_salary", e.month_salary);

    v.set("extra_hours_salary", e.extra_hours_salary);
    v.set("salary_mode", static_cast<int>(e.salary_mode));
    ind = i_ok;
  }
};

// Entity : Job  <==>  Database
template <>
struct type_conversion<Job>
{
  typedef values base_type;

  static void from_base(values const& v, indicator /* ind */, Job& e)
  {
    e.id = v.get<IntegerT>("id");

    e.short_title = v.get<StringT>("short_title");

    e.title = v.get<StringT>("title");

    e.hours_per_day = v.get<RealT>("hours_per_day");

    e.days_per_month = v.get<RealT>("days_per_month");

    e.declared_salary = v.get<RealT>("declared_salary");

    e.day_salary = v.get<RealT>("day_salary");

    e.month_salary = v.get<RealT>("month_salary");

    e.extra_hours_salary = v.get<RealT>("extra_hours_salary");

    e.salary_mode = v.get<int>("salary_mode");

    e.title_ar = v.get<StringT>("title_ar");

    e.description = v.get<StringT>("description");

    e.description_ar = v.get<StringT>("description_ar");
  }

  static void to_base(const Job& e, values& v, indicator& ind)
  {
    v.set("id", e.id);

    v.set("short_title", e.short_title);

    v.set("title", e.title);

    v.set("hours_per_day", e.hours_per_day);

    v.set("days_per_month", e.days_per_month);

    v.set("declared_salary", e.declared_salary);

    v.set("day_salary", e.day_salary);

    v.set("month_salary", e.month_salary);

    v.set("extra_hours_salary", e.extra_hours_salary);

    v.set("salary_mode", e.salary_mode);

    v.set("title_ar", e.title_ar);

    v.set("description", e.description);

    v.set("description_ar", e.description_ar);
    ind = i_ok;
  }
};

// Entity : EmployeeFR  <==>  Database
template <>
struct type_conversion<EmployeeFR>
{
  typedef values base_type;

  static void from_base(values const& v, indicator /* ind */, EmployeeFR& e)
  {
    e.id = v.get<IntegerT>("id");

    e.full_name = v.get<StringT>("full_name");

    e.job_title = v.get<StringT>("job_title");

    e.commune = v.get<StringT>("commune");

    e.wilaya = v.get<StringT>("wilaya");

    e.active = v.get<bool>("active");
  }

  static void to_base(const EmployeeFR& e, values& v, indicator& ind)
  {
    v.set("id", e.id);

    v.set("full_name", e.full_name);

    v.set("job_title", e.job_title);

    v.set("commune", e.commune);

    v.set("wilaya", e.wilaya);

    v.set("active", e.active);
    ind = i_ok;
  }
};

}  // namespace soci

namespace data
{

//-----------------------------------------------------------
template <typename T, typename... Args>
std::vector<T> getMultipleQuery(soci::session& sql, const std::string& query, Args... args)
{
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
  for (; it != end; ++it)
  {
    objs.push_back(*it);
  }
  return objs;
}

template <typename T, typename... Args>
T getSingleleQuery(soci::session& sql, const std::string& query, Args... args)
{
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
bool singleObjectOperation(soci::session& sql, const std::string& query, Args&&... args)
{
  soci::statement st(sql);
  (st.exchange(soci::use(args)), ...);
  st.alloc();
  st.prepare(query);
  st.define_and_bind();
  st.execute(true);
  return true;
}

//-----------------------------------------------------------

template <typename T, size_t N, bool IsView = false>
struct EntityMetadataBase
{
  using type = T;
  static constexpr bool is_view = IsView;
  static constexpr size_t field_count = N;
};

template <typename T>
struct EntityMetadata;

template <typename T>
std::ostream& print_header(std::ostream& os)
{
  std::apply([&](auto... m) { ((os << std::left << std::setw(std::get<2>(m)) << std::get<0>(m)), ...); },
             EntityMetadata<T>::members);
  return os;
}

template <typename T>
std::ostream& print_row(std::ostream& os, const T& obj)
{
  std::apply([&](auto... m) { ((os << std::left << std::setw(std::get<2>(m)) << obj.*(std::get<1>(m))), ...); },
             EntityMetadata<T>::members);
  return os;
}
template <typename T>
void printHeader();

template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec)
{
  print_header<T>(os) << "\n";
  for (const auto& item : vec) print_row(os, item) << '\n';
  return os;
}

template <typename E>
struct enum_traits;  // user specializes this per enum
template <typename E>
std::enable_if_t<std::is_enum_v<E>, std::ostream&> operator<<(std::ostream& os, E e)
{
  if constexpr (requires { enum_traits<E>::names; })
  {
    // name table available
    auto val = static_cast<std::size_t>(e);
    if (val < enum_traits<E>::names.size())
      return os << enum_traits<E>::names[val];
    else
      return os << "Unknown(" << val << ")";
  }
  else
  {
    // fallback: print as integer
    return os << static_cast<std::underlying_type_t<E>>(e);
  }
}

template <>
struct enum_traits<Gender>
{
  static constexpr std::array<const char*, 2> names = {"Men", "Women"};
};

template <>
struct enum_traits<AccountType>
{
  static constexpr std::array<const char*, 2> names = {"User", "Admin"};
};

template <>
struct enum_traits<ContractType>
{
  static constexpr std::array<const char*, 4> names = {"CDI", "CDD", "CTT", "CUI"};
};

template <>
struct enum_traits<PaymentMode>
{
  static constexpr std::array<const char*, 5> names = {"Cash", "CCP", "RIP", "RIB", "Balance"};
};

template <>
struct enum_traits<JobbingPaymentMode>
{
  static constexpr std::array<const char*, 2> names = {"Cash", "AddToSalary"};
};

template <>
struct enum_traits<SalaryMode>
{
  static constexpr std::array<const char*, 5> names = {"MonthBased", "DayBased", "DayBasedAdapted", "HourBased",
                                                       "HourBasedAdapted"};
};

//-------------------------------------------------------------

// Insert
template <typename T>
bool insert(soci::session& sql, const T& obj);

template <>
bool insert<Wilaya>(soci::session& sql, const Wilaya& obj)
{
  return singleObjectOperation(sql, "INSERT INTO wilayas () VALUES (:)", obj);
}

template <>
bool insert<Commune>(soci::session& sql, const Commune& obj)
{
  return singleObjectOperation(sql, "INSERT INTO communes () VALUES (:)", obj);
}

template <>
bool insert<JobFRCompaxt>(soci::session& sql, const JobFRCompaxt& obj)
{
  return singleObjectOperation(sql, "INSERT INTO jobs () VALUES (:)", obj);
}

template <>
bool insert<Job>(soci::session& sql, const Job& obj)
{
  return singleObjectOperation(sql, "INSERT INTO jobs () VALUES (:)", obj);
}

// Select by ID
template <typename T, typename IdType>
T selectById(soci::session& sql, const IdType& id);

// Select all

template <typename T>
std::vector<T> selectAll(soci::session& sql);

template <>
std::vector<Wilaya> selectAll<Wilaya>(soci::session& sql)
{
  return getMultipleQuery<Wilaya>(sql, "SELECT id, name, name_ar FROM wilayas ");
}

template <>
std::vector<Commune> selectAll<Commune>(soci::session& sql)
{
  return getMultipleQuery<Commune>(sql, "SELECT id, name, name_ar, wilaya_id FROM communes ");
}

template <>
std::vector<JobFRCompaxt> selectAll<JobFRCompaxt>(soci::session& sql)
{
  return getMultipleQuery<JobFRCompaxt>(sql,
                                        "SELECT id, short_title, title, hours_per_day, days_per_month, "
                                        "declared_salary, day_salary, month_salary, extra_hours_salary, "
                                        "salary_mode FROM jobs ");
}

template <>
std::vector<Job> selectAll<Job>(soci::session& sql)
{
  return getMultipleQuery<Job>(sql,
                               "SELECT id, short_title, title, hours_per_day, days_per_month, "
                               "declared_salary, day_salary, month_salary, extra_hours_salary, "
                               "salary_mode, title_ar, description, description_ar FROM jobs ");
}

template <>
std::vector<EmployeeFR> selectAll<EmployeeFR>(soci::session& sql)
{
  return getMultipleQuery<EmployeeFR>(sql,
                                      "SELECT id, full_name, job_title, commune, wilaya, active FROM "
                                      "v_employees ");
}

//---------------------Metadata Functions---------------------/

template <>
struct EntityMetadata<Wilaya> : EntityMetadataBase<Wilaya, 3, false>
{
  static constexpr const char* table_name = "wilayas";
  static constexpr auto members =
    std::make_tuple(std::tuple{"id", &type::id, 5u, ""}, std::tuple{"name", &type::name, 15u, ""},
                    std::tuple{"name_ar", &type::name_ar, 15u, ""});
};

template <>
struct EntityMetadata<Commune> : EntityMetadataBase<Commune, 4, false>
{
  static constexpr const char* table_name = "communes";
  static constexpr auto members =
    std::make_tuple(std::tuple{"id", &type::id, 6u, ""}, std::tuple{"name", &type::name, 20u, ""},
                    std::tuple{"name_ar", &type::name_ar, 30u, ""}, std::tuple{"wilaya_id", &type::wilaya_id, 5u, ""});
};

template <>
struct EntityMetadata<JobFRCompaxt> : EntityMetadataBase<JobFRCompaxt, 10, false>
{
  static constexpr const char* table_name = "jobs";
  static constexpr auto members = std::make_tuple(
    std::tuple{"id", &type::id, 5u, ""}, std::tuple{"short_title", &type::short_title, 5u, ""},
    std::tuple{"title", &type::title, 30u, ""}, std::tuple{"hours_per_day", &type::hours_per_day, 4u, ""},
    std::tuple{"days_per_month", &type::days_per_month, 4u, ""},
    std::tuple{"declared_salary", &type::declared_salary, 10u, ""},
    std::tuple{"day_salary", &type::day_salary, 10u, ""}, std::tuple{"month_salary", &type::month_salary, 10u, ""},
    std::tuple{"extra_hours_salary", &type::extra_hours_salary, 10u, ""},
    std::tuple{"salary_mode", &type::salary_mode, 20u, ""});
};

template <>
struct EntityMetadata<Job> : EntityMetadataBase<Job, 13, false>
{
  static constexpr const char* table_name = "jobs";
  static constexpr auto members = std::make_tuple(
    std::tuple{"id", &type::id, 5u, ""}, std::tuple{"short_title", &type::short_title, 5u, ""},
    std::tuple{"title", &type::title, 30u, ""}, std::tuple{"hours_per_day", &type::hours_per_day, 4u, ""},
    std::tuple{"days_per_month", &type::days_per_month, 4u, ""},
    std::tuple{"declared_salary", &type::declared_salary, 10u, ""},
    std::tuple{"day_salary", &type::day_salary, 10u, ""}, std::tuple{"month_salary", &type::month_salary, 10u, ""},
    std::tuple{"extra_hours_salary", &type::extra_hours_salary, 10u, ""},
    std::tuple{"salary_mode", &type::salary_mode, 20u, ""}, std::tuple{"title_ar", &type::title_ar, 30u, ""},
    std::tuple{"description", &type::description, 6u, ""},
    std::tuple{"description_ar", &type::description_ar, 20u, ""});
};

template <>
struct EntityMetadata<EmployeeFR> : EntityMetadataBase<EmployeeFR, 6, false>
{
  static constexpr const char* table_name = "v_employees";
  static constexpr auto members =
    std::make_tuple(std::tuple{"id", &type::id, 5u, ""}, std::tuple{"full_name", &type::full_name, 25u, ""},
                    std::tuple{"job_title", &type::job_title, 30u, ""}, std::tuple{"commune", &type::commune, 20u, ""},
                    std::tuple{"wilaya", &type::wilaya, 20u, ""}, std::tuple{"active", &type::active, 20u, ""});
};

}  // namespace data
