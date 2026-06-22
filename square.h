#pragma once
#include <format>
#include <string>
#include <iostream>
#include "types.h"

namespace beaver {

enum SQ : u8 // 0x88 repr
{
  A1 = 0x00, B1, C1, D1, E1, F1, G1, H1,
  A2 = 0x10, B2, C2, D2, E2, F2, G2, H2,
  A3 = 0x20, B3, C3, D3, E3, F3, G3, H3,
  A4 = 0x30, B4, C4, D4, E4, F4, G4, H4,
  A5 = 0x40, B5, C5, D5, E5, F5, G5, H5,
  A6 = 0x50, B6, C6, D6, E6, F6, G6, H6,
  A7 = 0x60, B7, C7, D7, E7, F7, G7, H7,
  A8 = 0x70, B8, C8, D8, E8, F8, G8, H8, SQ_N, SQ_None, SQ_Null
};

INLINE int rank(SQ x) { return x >> 4; }
INLINE int file(SQ x) { return x & 15; }

INLINE bool out(SQ x) { return x & 0x88; }
INLINE bool valid(SQ x) { return !out(x); }

INLINE SQ to_sq(int f, int r)
{
  return static_cast<SQ>((r << 4) + f);
}

INLINE SQ to_sq(std::string s)
{
  return s.length() > 1 ? to_sq(s[0] - 'a', s[1] - '1') : SQ_N;
}

INLINE SQ opp(SQ sq)
{
  return static_cast<SQ>(sq ^ 0x70); // flip vertically
}

INLINE std::string to_string(SQ sq)
{
  char fileChar = 'a' + file(sq);
  char rankChar = '1' + rank(sq);
  return (std::string() + fileChar) + rankChar;
}

inline std::ostream & operator << (std::ostream & os, const SQ & sq)
{
  os << static_cast<char>('a' + file(sq))
     << static_cast<char>('1' + rank(sq)) << std::endl;
  return os;
}

INLINE SQ operator + (SQ a, int i) { return static_cast<SQ>(+a + i); }
INLINE SQ operator - (SQ a, int i) { return static_cast<SQ>(+a - i); }

INLINE SQ & operator += (SQ & a, int i) { a = a + i; return a; }
INLINE SQ & operator -= (SQ & a, int i) { a = a - i; return a; }

INLINE SQ & operator ++ (SQ & a) { a = static_cast<SQ>(a + 1); return a; }
INLINE SQ & operator -- (SQ & a) { a = static_cast<SQ>(a - 1); return a; }

}

template<>
struct std::formatter<beaver::SQ> : std::formatter<std::string>
{
  auto format(const beaver::SQ & sq, std::format_context & ctx) const
  {
    std::string str = to_string(sq);
    return std::formatter<std::string>::format(str, ctx);
  }
};
