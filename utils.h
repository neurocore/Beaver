#pragma once
#include <windows.h>
#include <conio.h>
#include <cassert>
#include <chrono>
#include <iostream>
#include <format>
#include <string_view>
#include <string>
#include <vector>
#include <charconv>
#include <algorithm>
#include "types.h"

namespace beaver {

using Clock = std::chrono::high_resolution_clock;
using Milli = std::chrono::milliseconds;
using Timestamp = Clock::time_point;

inline MS elapsed(Timestamp start)
{
  return std::chrono::duration_cast<Milli>(Clock::now() - start).count();
}

template<typename T> INLINE int sgn(T val)
{
  return (T(0) < val) - (val < T(0));
}

template<typename T> INLINE T abs(T val)
{
  return val < T(0) ? -val : val;
}

template<typename T, typename R>
R compare(T a, T b, R less, R equal, R more)
{
  return (a < b ? less : (a > b ? more : equal));
}

// returns -1 if not found
INLINE ptrdiff_t index_of(std::string_view str, char ch)
{
  auto it = std::find(str.begin(), str.end(), ch);
  return it != str.end() ? it - str.begin() : -1;
}

INLINE std::string cut(std::string & str, std::string delim = " ")
{
  auto pos = str.find(delim);
  std::string part = str.substr(0, pos);
  str = pos == std::string::npos ? std::string()
      : str.substr(pos + delim.length(), str.length());
  return part;
}

INLINE void dry(std::string & str, std::string_view chars = "\n")
{
  size_t pos = str.length();
  for (char ch : chars)
  {
    while ((pos = str.rfind(ch, pos)) != std::string::npos)
    {
      str.erase(pos, 1);
    }
  }
}

INLINE bool cut_start(std::string & str, char & ch)
{
  if (str.length() <= 0) return false;
  ch = str[0];
  str = str.substr(1);
  return true;
}

INLINE bool cut_end(std::string & str, char & ch)
{
  if (str.length() <= 0) return false;
  ch = str[str.length() - 1];
  str = str.substr(0, str.length() - 1);
  return true;
}

static Strings split(const std::string & str, std::string delim = " ")
{
  Strings result;
  if (delim.empty()) return result;

  size_t start = 0, end;
  while ((end = str.find(delim, start)) != std::string::npos)
  {
    if (end != start)
      result.push_back(str.substr(start, end - start));
    start = end + delim.size();
  }
  result.push_back(str.substr(start));
  return result;
}

static void replace_all(std::string & str, const std::string & search, const std::string & replace)
{
  size_t pos = 0;
  while ((pos = str.find(search, pos)) != std::string::npos)
  {
    str.replace(pos, search.length(), replace);
    pos += replace.length();
  }
}

inline std::string trim(std::string & str, char ch = ' ')
{
  str.erase(str.find_last_not_of(ch) + 1);
  str.erase(0, str.find_first_not_of(ch));
  return str;
}

inline std::string trim(std::string & str, char left, char right)
{
  str.erase(str.find_last_not_of(left) + 1);
  str.erase(0, str.find_first_not_of(right));
  return str;
}

inline int parse_int(const std::string_view str, int def = 0, int base = 10)
{
  i64 result = def; // it can't correctly read hex int (8 chars)
  std::from_chars(str.data(), str.data() + str.size(), result, base);
  return static_cast<int>(result);
}

inline double parse_double(const std::string_view str, double def = 0.)
{
  double result = def;
  std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

class InputHandler
{
  DWORD mode;
  HANDLE handle = GetStdHandle(STD_INPUT_HANDLE);
  BOOL console = GetConsoleMode(handle, &mode);

public:
  inline bool available()
  {
    if (!console)
    {
      DWORD bytes_avail;
      if (!PeekNamedPipe(handle, 0, 0, 0, &bytes_avail, 0)) return true;
      return bytes_avail > 0;
    }
    else return _kbhit();
  }

  inline bool is_console() const
  {
    return console;
  }
};

static InputHandler Input;

template<bool flush = false, typename... Args>
INLINE void say(std::format_string<Args...> fmt, Args&&... args)
{
  std::cout << std::format(fmt, std::forward<Args>(args)...);
  if constexpr (flush) std::cout.flush();
}

template<typename... Args>
INLINE void con(std::format_string<Args...> fmt, Args&&... args)
{
  if (Input.is_console())
    std::cout << std::format(fmt, std::forward<Args>(args)...);
}

template<typename... Args>
INLINE void log(std::format_string<Args...> fmt, Args&&... args)
{
  std::cerr << std::format(fmt, std::forward<Args>(args)...);
}

// static vector
template<typename T, size_t Capacity>
struct Arr
{
  static_assert(std::is_trivially_copyable_v<T>, "T must be trivially copyable");
  T arr[Capacity];
  size_t cnt = 0;

  Arr() {}
  Arr(const Arr & a) : cnt(a.cnt)
  {
    for (size_t i = 0; i < Capacity; i++)
      arr[i] = a.arr[i];
  }

  constexpr size_t capacity() const { return Capacity; }
  size_t size() const { return cnt; }
  bool empty() const { return !cnt; }
  void clear() { cnt = 0; }

  T & operator[] (size_t i) { return arr[i]; }
  const T & operator[] (size_t i) const { return arr[i]; }
  T * begin() { return arr; }
  T * end() { return arr + cnt; }
  const T * begin() const { return arr; }
  const T * end() const { return arr + cnt; }

  void add(const T & val)
  {
    assert(cnt < Capacity - 1);
    arr[cnt++] = val;
  }

  void pop_back()
  {
    assert(cnt > 0);
    --cnt;
  }

  void erase(size_t i)
  {
    assert(i < cnt);
    std::swap(arr[i], arr[cnt - 1]);
    --cnt;
  }

  size_t find(const T & val) const
  {
    for (size_t i = 0; i < cnt; ++i)
      if (arr[i] == val) return i;
    return Capacity;
  }
};

}
