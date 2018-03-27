#include "TableEntry.h"

#include <functional>
#include <cassert>
#include <iostream>
#include <utility>
#include <exception>

using std::string;
using std::greater;
using std::less;
using std::equal_to;
using std::not_equal_to;
using std::move;
using std::ostream;
using std::terminate;

// elt constructors
// const char* method, because otherwise it matches the bool one
TableEntry::TableEntry(const char* val) : tag{Tag::String}, data_string{val} {}
TableEntry::TableEntry(const string& val) : tag{Tag::String}, data_string{val} {}
TableEntry::TableEntry(string&& val) : tag{Tag::String}, data_string{move(val)} {}
TableEntry::TableEntry(double val) : tag{Tag::Double}, data_double{val} {}
TableEntry::TableEntry(int val) : tag{Tag::Int}, data_int{val} {}
TableEntry::TableEntry(bool val) : tag{Tag::Bool}, data_bool{val} {}

// copy/move ctors
TableEntry::TableEntry(const TableEntry& other) : tag{other.tag} {
  construct_from(other);
}
TableEntry::TableEntry(TableEntry&& other) noexcept : tag{other.tag} {
  construct_from(move(other));
}

// dtor
TableEntry::~TableEntry() noexcept {
  if (tag == Tag::String)
    data_string.~string();
}

// comparisons
bool TableEntry::operator<(const TableEntry& other) const noexcept {
  return compare<less>(other);
}
bool TableEntry::operator>(const TableEntry& other) const noexcept {
  return compare<greater>(other);
}
bool TableEntry::operator==(const TableEntry& other) const noexcept {
  return compare<equal_to>(other);
}
bool TableEntry::operator!=(const TableEntry& other) const noexcept {
  return compare<not_equal_to>(other);
}

// specializations on the as<> template for conversion
template <> const string& TableEntry::as<string>() const noexcept {
  assert(tag == Tag::String &&
         "tried to use TableEntry as a string when it didn't contain a string");
  return data_string;
}
template <> double TableEntry::as<double>() const noexcept {
  assert(tag == Tag::Double &&
         "tried to use TableEntry as a double when it didn't contain a double");
  return data_double;
}
template <> int TableEntry::as<int>() const noexcept {
  assert(tag == Tag::Int &&
         "tried to use TableEntry as an int when it didn't contain an int");
  return data_int;
}
template <> bool TableEntry::as<bool>() const noexcept {
  assert(tag == Tag::Bool &&
         "tried to use TableEntry as a bool when it didn't contain a bool");
  return data_bool;
}


namespace std {
// overload to std::hash
size_t hash<TableEntry>::operator()(const TableEntry& tt) const noexcept {
  switch (tt.tag) {
  case TableEntry::Tag::String:
    return hash<string>{}(tt.data_string);
  case TableEntry::Tag::Double:
    return hash<double>{}(tt.data_double);
  case TableEntry::Tag::Int:
    return hash<int>{}(tt.data_int);
  case TableEntry::Tag::Bool:
    return hash<bool>{}(tt.data_bool);
  }
  terminate();
}
}

ostream& operator<<(ostream& os, const TableEntry& tt) {
  switch (tt.tag) {
  case TableEntry::Tag::String:
    return os << tt.data_string;
  case TableEntry::Tag::Double:
    return os << tt.data_double;
  case TableEntry::Tag::Int:
    return os << tt.data_int;
  case TableEntry::Tag::Bool:
    return os << tt.data_bool;
  }
  terminate();
}