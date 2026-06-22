#pragma once
#include "utils.h"

namespace beaver {

enum Color : int { Black, White, Color_N };
enum Piece : u8  { BP, WP, BK, WK, Piece_N, NOP = Piece_N };
enum PT    : u8  { Pawn, King, PT_N };

INLINE Color operator ~ (Color c) { return static_cast<Color>(!c); }
INLINE Color operator ^ (Color c, int i) { return static_cast<Color>(+c ^ i); }

INLINE char to_char(Color c) { return "bw."[c]; }
INLINE char to_char(Piece p) { return "pPkK."[p]; }
INLINE char to_char(PT p)    { return "pk."[p]; }

INLINE Color to_color(const char c)
{
  size_t i = index_of("bw", c);
  if (i < 0) i = Color_N;
  return static_cast<Color>(i);
}

INLINE Piece to_piece(const char c)
{
  auto i = index_of("pPkK", c);
  if (i < 0) return NOP;
  return static_cast<Piece>(i);
}

INLINE PT to_pt(const char c)
{
  auto i = index_of("pk", c);
  if (i < 0) return PT_N;
  return static_cast<PT>(i);
}

INLINE Piece to_piece(PT pt, Color c) { return static_cast<Piece>(2 * pt + c); }
INLINE Color col(Piece p)             { return static_cast<Color>(p & 1); }
INLINE Piece opp(Piece p)             { return static_cast<Piece>(p ^ 1); }
INLINE PT pt(Piece p)                 { return static_cast<PT>(p >> 1); }
INLINE PT & operator ++ (PT & a)      { a = static_cast<PT>(a + 1); return a; }

INLINE Piece & operator ++ (Piece & a) { a = static_cast<Piece>(a + 1); return a; }
INLINE Piece & operator -- (Piece & a) { a = static_cast<Piece>(a - 1); return a; }

INLINE bool is_pawn(Piece p) { return p < BK; }
INLINE bool is_king(Piece p) { return p == BK || p == WK; }

}

template<>
struct std::formatter<beaver::Piece> : std::formatter<std::string>
{
  auto format(const beaver::Piece & p, std::format_context & ctx) const
  {
    std::string str = std::string() + to_char(p);
    return std::formatter<std::string>::format(str, ctx);
  }
};
