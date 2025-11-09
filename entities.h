#pragma once
#include "orm.h"
#include <string>
namespace data {

using StringT = std::string; // will be changed to wxSrting or CsString ...
using DateTimeT = time_t;
using DateT = std::string;
using TimeT = time_t;
using IntegerT = long int;
using RealT = double;
enum class Gender { Men, Women };
enum class AccountType { User, Admin };
enum class ContractType { CDI, CDD, CTT, CUI };
enum class PaymentMode { Cash, CCP, RIP, RIB, Balance };
enum class JobbingPaymentMode { Cash, AddToSalary };

enum class SalaryMode {
  MonthBased,
  DayBased,
  DayBasedAdapted,
  HourBased,
  HourBasedAdapted
};

struct TABLE(employees) Employee {
  AUTOINC PK IntegerT id;
  bool active;
  StringT first_name;
  StringT last_name;
  StringT first_name_ar;
  StringT last_mame_ar;
  StringT mother_first_name;
  StringT mother_last_name;
  StringT mother_first_name_ar;
  StringT mother_last_name_ar;

  // Father last name
  StringT father_last_name;
  StringT father_last_name_ar;

  // Birth information
  DateT birth_date;
  StringT birth_location;
  StringT birth_location_ar;

  // Contact information
  StringT address;
  StringT code_postal;
  StringT mobile_phone;
  StringT email;
  IntegerT commune_id; // commune

  StringT ssn_number;
  StringT national_id;

  IntegerT job_id;
  FK DateT join_date;
  READONLY StringT full_name;
  READONLY StringT full_name_ar;
};

struct VIEW(v_active_employees) EmployeeCompactFR {
  PK IntegerT id;
  StringT job_title;
  StringT full_name;
  StringT commune;
  StringT wilaya;
};

struct VIEW(v_active_employees) EmployeeCompactAR {
  PK IntegerT id;
  StringT job_title;
  StringT full_name;
  StringT commune;
  StringT wilaya;
};

struct TABLE(attendances) AttendanceDB {
  AUTOINC PK IntegerT id;
  bool active;
  DateT clock_in;
  DateT clock_out;
  StringT notes;
  READONLY RealT worked_hours;
  FK IntegerT employee_id;
};

struct TABLE(wilayas) WilayaDB {
  AUTOINC PK IntegerT id;
  StringT name;
  StringT name_ar;
};

struct TABLE(communes) CommuneDB {
  AUTOINC PK IntegerT id;
  StringT name;
  StringT name_ar;
  IntegerT wilaya_id;
};

struct VIEW(v_all_employees) EmployeeFR {
  AUTOINC PK IntegerT id;
  StringT full_name;
  bool active;
};

struct TABLE(jobs) JobDB {
  AUTOINC PK IntegerT id;
  StringT title;
  StringT title_ar;
  StringT short_title;
  StringT description;
  StringT description_ar;
  RealT hours_per_day;
  RealT days_per_month;
  RealT declared_salary;
  RealT day_salary;
  RealT month_salary;
  RealT extra_hours_salary;
  SalaryMode salary_mode;
};

struct TABLE(attendances) AttendenceDB {
  AUTOINC PK IntegerT id;
  FK IntegerT employee_id;
  TimeT clock_in;
  TimeT clock_out;
  StringT motes;
  DateT work_date;
  RealT worked_hours;
};

} // namespace data
