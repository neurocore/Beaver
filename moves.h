#pragma once
#include <bit>
#include "square.h"
#include "piece.h"

namespace beaver {

enum Flags : u8
{
  Quiet,  // PC (Promotion, Capture)
  Cap,    // 01
  Prom,   // 10
  CapProm // 11
};

// Since it's Plain Old Data, it is trivially copyable,
//  so compiler should optimize all copy operations
//  into single 64-bit load instruction (>= O2)

struct Move // 8 bytes (POD)
{
  SQ from, to; // 2
  Flags flags; // 1
  u8 captures; // 1
  u32 bb_caps; // 4
};

static const Move None = { SQ_None, SQ_None, Quiet, 0, 0u };
static const Move Null = { SQ_Null, SQ_Null, Quiet, 0, 0u };

INLINE bool is_empty(const Move & move) { return out(move.from); }
INLINE bool is_null(const Move & move) { return move.from == SQ_Null; }

INLINE bool is_cap(Flags fl) { return !!(fl & Cap); }
INLINE bool is_prom(Flags fl) { return !!(fl & Prom); }
INLINE bool is_capprom(Flags fl) { return !!(fl & CapProm); }
INLINE bool is_attack(Flags fl) { return !!fl; }

INLINE bool is_cap(const Move & move) { return !!(move.flags & Cap); }
INLINE bool is_prom(const Move & move) { return !!(move.flags & Prom); }
INLINE bool is_capprom(const Move & move) { return !!(move.flags & CapProm); }
INLINE bool is_attack(const Move & move) { return !!(move.flags); }

INLINE std::string to_string(const Move & move)
{
  const char delim = move.captures ? 'x' : '-';
  return to_string(move.from) + delim
       + to_string(move.to);
}

// We hint to compiler that an effective way to compare this
//  structs is to represent them as a single 64-bit word
// Bit cast operation is zero cost (>= O2)

INLINE bool operator == (const Move & a, const Move & b)
{
  return std::bit_cast<u64>(a) == std::bit_cast<u64>(b);
}

// When reading input we expect that only three fields are
//  actually determining a move:
// - 'from', 'to' squares
// - bitboard of captured pieces
// The captures counter may not be initialized

INLINE bool same(const Move & a, const Move & b)
{
  if (a.from != b.from) return false;
  if (a.flags != b.flags) return false;
  if (a.bb_caps != b.bb_caps) return false;

  return true;
}

INLINE Move mv_quiet(SQ from, SQ to)
{
  return { from, to, Quiet, 0, 0u };
}

using History = int[Color_N][SQ_N][SQ_N];
using MoveList = Arr<Move, 256>;

}

template<>
struct std::formatter<beaver::Move> : std::formatter<std::string>
{
  auto format(const beaver::Move & move, std::format_context & ctx) const
  {
    std::string str = to_string(move);
    return std::formatter<std::string>::format(str, ctx);
  }
};
