#pragma once
#include <format>
#include <string>
#include <iostream>
#include "types.h"

namespace beaver {

enum SQ : u8
{
  A1, C1, E1, G1,
  B2, D2, F2, H2,
  A3, C3, E3, G3,
  B4, D4, F4, H4,
  A5, C5, E5, G5,
  B6, D6, F6, H6,
  A7, C7, E7, G7,
  B8, D8, F8, H8,
  
  SQ_N,  SQ_None = SQ_N,
  SQ_NN, SQ_Null = SQ_NN
};

INLINE int rank(SQ x) { return x >> 2; }
INLINE int file(SQ x) { return 2 * (x & 3) + (rank(x) & 1); }

INLINE bool out(SQ x) { return x > H8; }
INLINE bool valid(SQ x) { return !out(x); }

INLINE SQ to_sq(int f, int r)
{
  return static_cast<SQ>((r << 2) + (f >> 1));
}

INLINE SQ to_sq(std::string s)
{
  return s.length() > 1 ? to_sq(s[0] - 'a', s[1] - '1') : SQ_N;
}

INLINE bool invalid(int f, int r)
{
  return (f + r) & 1;
}

INLINE SQ opp(SQ sq)
{
  return static_cast<SQ>(sq ^ 28); // flip vert
}

INLINE std::string to_string(SQ sq)
{
  char fileChar = 'a' + file(sq);
  char rankChar = '1' + rank(sq);
  return (std::string() + fileChar) + rankChar;
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
