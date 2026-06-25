#pragma once
#include <random>
#include "square.h"
#include "bitboard.h"
#include "tables.h"

namespace beaver {

const int Bits = 9;

// Actually this is black magics
// Original idea by Volker Annuss

struct Magic
{
  const u32 * ptr;
  u32 notmask;
  u32 blackmagic;
};

constexpr u32 index_to_u32(int index, int bits, u32 mask)
{
  u32 result = ~mask;
  for (int i = 0; i < bits; ++i)
  {
    int j = bitscan(mask);
    if (index & (Bit << i)) result |= (Bit << j);
    mask = rlsb(mask);
  }
  return result;
}

constexpr u32 get_mask(SQ sq)
{
  u32 result = 0u;
  for (int dir = UL; dir < Dir_N; dir++)
  {
    u32 bit = 1u;
    u32 pre = 0u;

    for (SQ j = sq; bit; j = bitscan(bit))
    {
      bit = step[dir][j];
      if (!bit) break;
      
      result |= pre;
      pre = bit;
    }
  }
  return result;
}

constexpr u32 get_att(SQ sq, u32 blocks)
{
  u32 result = 0u;
  for (int dir = UL; dir < Dir_N; dir++)
  {
    u32 bit = 1u;
    for (SQ j = sq; bit; j = bitscan(bit))
    {
      bit = step[dir][j];
      result |= bit;
      if (!bit || bit & blocks) break;
    }
  }
  return result;
}

INLINE int transform(u32 blocks, u32 magic, int bits)
{
  return static_cast<int>((blocks * magic) >> (32 - bits));
}

// +-------------------------------------------
//   Searching for best magics
// +-------------------------------------------

struct Helper
{
  u32 offset;
  u32 magic;
};

std::array<Helper, 32> helpers
{
  Helper(     0, 0x20088081 ), // a1
  Helper(     0, 0x803fbff9 ), // c1
  Helper(     0, 0x407fc131 ), // e1
  Helper(     0, 0x30078010 ), // g1
  Helper(     0, 0x8a007801 ), // b2
  Helper(     0, 0xe000404c ), // d2
  Helper(     0, 0xdc408030 ), // f2
  Helper(     0, 0x1ee00807 ), // h2
  Helper(     0, 0xf00ff841 ), // a3
  Helper(     0, 0x10200841 ), // c3
  Helper(     0, 0x47d80380 ), // e3
  Helper(     0, 0x1403ff84 ), // g3
  Helper(     0, 0x00040081 ), // b4
  Helper(     0, 0x5e004101 ), // d4
  Helper(     0, 0x08404108 ), // f4
  Helper(     0, 0x0be04004 ), // h4
  Helper(     0, 0x00804081 ), // a5
  Helper(     0, 0x18808009 ), // c5
  Helper(     0, 0x17a2001c ), // e5
  Helper(     0, 0x00401004 ), // g5
  Helper(     0, 0xc3ff807b ), // b6
  Helper(     0, 0xdefff47d ), // d6
  Helper(     0, 0x01014041 ), // f6
  Helper(     0, 0x800021a2 ), // h6
  Helper(     0, 0x24008045 ), // a7
  Helper(     0, 0x54e04028 ), // c7
  Helper(     0, 0xe6cf7fd8 ), // e7
  Helper(     0, 0x1000440d ), // g7
  Helper(     0, 0x8e1fc00a ), // b8
  Helper(     0, 0xe8fb5f83 ), // d8
  Helper(     0, 0xafdfff81 ), // f8
  Helper(     0, 0x20004382 ), // h8
}; // total size: 3839 * 16 bytes

u32 attset[SQ_N][1 << Bits]{};

// Magic must map blockers set into smallest attset
INLINE int score_magic(SQ sq, u32 magic, u32 mask)
{
  const int sz = 1 << Bits;
  int lowest = sz;
  int highest = 0;

  //for (int i = 0; i < sz; i++)
  //  attset[sq][i] = 0u; // reset!

  for (int i = 0; i < sz; i++)
  {
    u32 blocks = index_to_u32(i, Bits, mask);
    int index  = transform(blocks, magic, Bits);
    u32 attack = get_att(sq, blocks);

    lowest  = (std::min)(lowest,  index);
    highest = (std::max)(highest, index);

    if (attset[sq][index]
    &&  attset[sq][index] != attack) // collision
    {
      for (int j = 0; j <= i; j++) // reset!
      {
        u32 occ = index_to_u32(j, Bits, mask);
        int k = transform(occ, magic, Bits);
        attset[sq][k] = 0u;
      }
      return 0;
    }

    attset[sq][index] = attack;
  }
  return highest - lowest;
}

static void build_magics()
{
  print32(get_mask(F4));
  print32(get_att(F4, 0u));

  int total_sz = 0;
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<u32> dist(0, Full);

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    log("\n{} - ", sq);
    u32 mask = get_mask(sq);

    u32 best = 0u;
    int best_sz = 1 << Bits;
    u32 magic = helpers[sq].magic;

    for (i64 i = 0; i < (best ? 10'000'000ll : 10'000'000'000ll); i++)
    {
      if (i) switch (i & 3)
      {
        case 0: magic = dist(gen); break;
        case 1: magic = dist(gen) & dist(gen); break;
        case 2: magic = dist(gen) & dist(gen) & dist(gen); break;
        case 3: magic = dist(gen) & dist(gen) & dist(gen); break;
      }

      int sz = score_magic(sq, magic, mask);

      if (sz && sz < best_sz)
      {
        best_sz = sz;
        best = magic;

        log("{} ", sz);
      }
    }

    total_sz += best_sz;

    if (!best) log("failed!");
    else helpers[sq].magic = best;
  }

  log("\n\ntotal size: {} * 16 bytes\n", total_sz);
  log("\n\nbest results:\n\n");
  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    log("  Helper(     0, 0x{:08x} ), // {}\n", helpers[sq].magic, sq);
  }
}

}
