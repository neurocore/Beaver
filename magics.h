#pragma once
#include "square.h"
#include "bitboard.h"
#include "tables.h"

namespace beaver {

const int Bits = 9;
const int Atts = 1 << Bits;

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
    if (index & (1u << i))
      result |= (1u << j);
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
  int offset;
  u32 magic;
};

static std::array<Helper, 32> helpers
{
  Helper(  -268, 0x507f0042 ), // a1
  Helper(   103, 0x00404008 ), // c1
  Helper(    77, 0x50803cc2 ), // e1
  Helper(    30, 0xa8078010 ), // g1
  Helper(  -151, 0x4a007801 ), // b2
  Helper(   576, 0x32004040 ), // d2
  Helper(   776, 0x7a407fd2 ), // f2
  Helper(  -238, 0x09600807 ), // h2
  Helper(   162, 0x101007c1 ), // a3
  Helper(  1051, 0x00200841 ), // c3
  Helper(   409, 0x17e00300 ), // e3
  Helper(  1179, 0x14040804 ), // g3
  Helper(   182, 0x00040081 ), // b4
  Helper(  1779, 0x5e004101 ), // d4
  Helper(   680, 0x00404108 ), // f4
  Helper(   999, 0x88202041 ), // h4
  Helper(   -12, 0x10804081 ), // a5
  Helper(  1300, 0x10808009 ), // c5
  Helper(  2291, 0x17a2001c ), // e5
  Helper(   530, 0xa0404001 ), // g5
  Helper(   101, 0x64008283 ), // b6
  Helper(  2491, 0x05600c03 ), // d6
  Helper(   873, 0x01014041 ), // f6
  Helper(    60, 0x800021a2 ), // h6
  Helper(   491, 0x00008045 ), // a7
  Helper(  1181, 0x82604028 ), // c7
  Helper(   460, 0x10308028 ), // e7
  Helper(    63, 0x8000440d ), // g7
  Helper(  -121, 0x04a0400a ), // b8
  Helper(   204, 0x7df91f83 ), // d8
  Helper(   400, 0xafdfff81 ), // f8
  Helper(  -228, 0x60070022 ), // h8
}; // total size: 3887 -> 2993 (+ 122)

const std::array<u32, 3115> attacks = []
{
  std::array<u32, 3115> result{};

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    const Helper & H = helpers[sq];
    u32 mask = get_mask(sq);

    for (int i = 0; i < Atts; ++i)
    {
      u32 blocks = index_to_u32(i, Bits, mask);
      int index  = transform(blocks, H.magic, Bits);
      result[H.offset + index] = get_att(sq, blocks);
    }
  }
  return result;
}();

const std::array<Magic, 32> magics = []
{
  std::array<Magic, 32> result{};

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    const Helper & H = helpers[sq];
    u32 mask = get_mask(sq);
    u32 * ptr = (u32 *)(&attacks) + H.offset;

    result[sq] = Magic(ptr, ~mask, H.magic);
  }
  return result;
}();

INLINE u32 k_att(u32 occ, SQ sq)
{
  auto ptr = magics[sq].ptr;
  occ     |= magics[sq].notmask;
  occ     *= magics[sq].blackmagic;
  occ    >>= 32 - 9;
  return ptr[occ];
}


static u32 attset[SQ_N][Atts]{};

INLINE void print_results()
{
  log("\n\nbest results:\n\n");
  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    log("  Helper( {:5d}, 0x{:08x} ), // {}\n",
      helpers[sq].offset, helpers[sq].magic, sq);
  }
}

// Magic must map blockers set into smallest attset
INLINE int score_magic(SQ sq, u32 magic, u32 mask, bool verbose = false)
{
  int lowest = Atts;
  int highest = 0;
  int cnt = 0;

  for (int i = 0; i < Atts; i++)
    attset[sq][i] = 0u; // reset!

  for (int i = 0; i < Atts; i++)
  {
    u32 blocks = index_to_u32(i, Bits, mask);
    int index  = transform(blocks, magic, Bits);
    u32 attack = get_att(sq, blocks);

    lowest  = (std::min)(lowest,  index);
    highest = (std::max)(highest, index);

    //if (!attset[sq][index]) cnt++; // new different attack

    if (attset[sq][index]
    &&  attset[sq][index] != attack) return 0; // collision

    attset[sq][index] = attack;
  }
  return highest - lowest + 1;
}

extern void build_magics();

}
