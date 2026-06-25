#include "tables.h"
#include "bitboard.h"

namespace beaver {

constexpr Dir on_line(SQ i, SQ j)
{
  int dx = file(j) - file(i);
  int dy = rank(j) - rank(i);

  return abs(dx) != abs(dy) ? Dir_N    // on line?
       : sgn(dx) == sgn(dy) ? UR : UL; // diag or anti
}

const std::array<SQ_BB, Dir_N> step = []
{
  std::array<SQ_BB, Dir_N> result{};

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    const int r = rank(sq);
    const int f = file(sq);

    result[UL][sq] = f > 0 && r < 7 ? bit(to_sq(f - 1, r + 1)) : 0u;
    result[DL][sq] = f > 0 && r > 0 ? bit(to_sq(f - 1, r - 1)) : 0u;

    result[UR][sq] = f < 7 && r < 7 ? bit(to_sq(f + 1, r + 1)) : 0u;
    result[DR][sq] = f < 7 && r > 0 ? bit(to_sq(f + 1, r - 1)) : 0u;
  }
  return result;
}();

const std::array<SQ_BB, Color_N> pmov = []
{
  std::array<SQ_BB, Color_N> result{};

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    const int r = rank(sq);

    result[0][sq] = r > 0 ? step[DL][sq] | step[DR][sq] : 0u;
    result[1][sq] = r < 7 ? step[UL][sq] | step[UR][sq] : 0u;
  }
  return result;
}();

const std::array<SQ_BB, Color_N> patt = []
{
  std::array<SQ_BB, Color_N> result{};

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    const int r = rank(sq);

    if (r > 1)
    {
      for (u32 bb = pmov[0][sq]; bb; bb = rlsb(bb))
      {
        SQ j = bitscan(bb);
        result[0][sq] = step[DL][j] | step[DR][j];
      }
    }

    if (r < 6)
    {
      for (u32 bb = pmov[1][sq]; bb; bb = rlsb(bb))
      {
        SQ j = bitscan(bb);
        result[1][sq] = step[UL][j] | step[UR][j];
      }
    }
  }
  return result;
}();

const std::array<SQ_BB, SQ_NN> between = []
{
  std::array<SQ_BB, SQ_NN> result{};

  for (SQ a = A1; a < SQ_N; ++a)
  {
    for (SQ b = a + 1; b < SQ_N; ++b)
    {
      const Dir dir = on_line(a, b);
      if (dir < Dir_N)
      {
        for (SQ j = a; j != b;)
        {
          u32 bb = step[dir][j];
          result[a][b] |= bb;
          j = bitscan(bb);
        }
      }

      result[b][a] = result[a][b];
    }
  }
  return result;
}();

}
