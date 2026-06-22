#pragma once
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

struct alignas(16) Move
{
  SQ from;
  SQ to[12];
  u8 captures;
  Flags flags;
};

static const Move None = { SQ_None, {}, 0, Quiet };
static const Move Null = { SQ_Null, {}, 0, Quiet };

INLINE bool is_empty(const Move & move) { return out(move.from); }
INLINE bool is_null(const Move & move) { return move.from == SQ_Null; }

INLINE bool is_cap(Flags fl) { return !!(fl & Cap); }
INLINE bool is_prom(Flags fl) { return !!(fl & Prom); }
INLINE bool is_capprom(Flags fl) { return !!(fl & CapProm); }
INLINE bool is_attack(Flags fl) { return is_cap(fl) || is_prom(fl); }

INLINE bool is_cap(const Move & move) { return !!(move.flags & Cap); }
INLINE bool is_prom(const Move & move) { return !!(move.flags & Prom); }
INLINE bool is_capprom(const Move & move) { return !!(move.flags & CapProm); }
INLINE bool is_attack(const Move & move) { return is_attack(move.flags); }

INLINE std::string to_string(const Move & move)
{
  const u8 caps = move.captures;
  const char delim = caps ? 'x' : '-';
  std::string str = to_string(move.from);

  for (int i = 0; i < caps + !caps; i++)
  {
    str += delim + to_string(move.to[i]);
  }
  return str;
}

INLINE bool same(const Move & a, const Move & b)
{
  if (a.from != b.from) return false;
  if (a.flags != b.flags) return false;
  if (a.captures != b.captures) return false;

  // TODO: check that we capture same pieces

  return false;
}

INLINE Move mv_quiet(SQ from, SQ to)
{
  return { from, { to }, 0, Quiet };
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
