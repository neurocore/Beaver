#pragma once
#include <bit>
#include <array>
#include "square.h"
#include "utils.h"

namespace beaver {

const u32 Empty = 0x00000000u;
const u32 Full  = 0xFFFFFFFFu;
const u32 Bit   = 0x00000001u;

INLINE u32 bit(SQ sq) { return Bit << static_cast<int>(sq); }
INLINE u32 lsb(u32 bb) { return bb & (Empty - bb); }
INLINE u32 rlsb(u32 bb) { return bb & (bb - Bit); }

INLINE bool only_one(u32 bb) { return bb && !rlsb(bb); }
INLINE bool several(u32 bb)  { return !!rlsb(bb); }

template<std::integral T>
INLINE T msb(T bb)
{
  unsigned n = 0;

  while (bb >>= 1) n++;
  return static_cast<T>(1) << n;
}

INLINE int popcnt(u32 bb)
{
  return std::popcount(bb);
}

INLINE SQ bitscan(u32 bb)
{
  return static_cast<SQ>(std::countr_zero(bb));
}

static void print32(u32 bb)
{
  for (int y = 7; y >= 0; --y)
  {
    log(" {} | ", y + 1);
    for (int x = 0; x < 8; ++x)
    {
      if (invalid(x, y)) log("  ");
      else
      {
        SQ sq = to_sq(x, y);
        u32 bit = Bit << sq;
        char ch = (bb & bit) ? 'o' : '.';
        log("{} ", ch);
      }
    }
    log("\n");
  }
  log("   +----------------  \n");
  log("     a b c d e f g h  \n\n");
}

}
