#include <limits>
#include <random>
#include <vector>
#include <tuple>
#include "magics.h"

using namespace std;

namespace beaver {

struct AttSet
{
  vector<u32> atts;
  int offset;
};

struct PackResult
{
  SQ sq;
  int offset; // for attset to fit
  int len;   // total len after fit
  int fit;  // degree of packaging
};

AttSet attsets[32];

// Suppose, first fit found:
// 
// attacks | xxxxxxxx............... ( 8 elements)
// attset  | ......yyyyyy........... ( 6 elements)
// result  | zzzzzzzzzzzz........... (12 elements)
//                 ^^
// [ offset = 6
// [ len = 12
// [ fit = 8 + 6 - 12 = 2

PackResult try_pack_attset(SQ sq, u32 * attacks, int len)
{
  const int sz = attsets[sq].atts.size();

  // Sliding window to fit current attset
  for (int offset = 0; offset <= len; offset++)
  {
    bool found = true;
    for (int i = 0; i < sz; i++)
    {
      if (attacks[i + offset]
      &&  attsets[sq].atts[i])
      {
        found = false;
        break;
      }
    }

    if (found)
    {
      const int new_len = std::max(len, offset + sz);
      return { sq, offset, new_len, len + sz - new_len };
    }
  }

  return { sq, len, len + sz, 0 };
}

void put_attset(SQ sq, u32 * attacks, int offset)
{
  for (int i = 0; i < attsets[sq].atts.size(); i++)
  {
    if (attsets[sq].atts[i])
    {
      assert(!attacks[i + offset]);
      attacks[i + offset] = attsets[sq].atts[i];
    }
  }
}

void build_magics()
{
  int total_sz = 0;
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<u32> dist(0, Full);

  log("Building magics\n\n");

  // +-------------------------------------------------
  //   1. Searching for optimal magics
  // +-------------------------------------------------

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    log("\n{} - ", sq);
    u32 mask = get_mask(sq);

    u32 best = 0u;
    int best_score = numeric_limits<int>::max();
    u32 magic = helpers[sq].magic;
    helpers[sq].offset = 0;

    for (i64 i = 0; i < (best ? 100'000ll : 1'000'000'000ll); i++)
    {
      if (i) switch (i & 3)
      {
        case 0: magic = dist(gen); break;
        case 1: magic = dist(gen) & dist(gen); break;
        case 2: magic = dist(gen) & dist(gen) & dist(gen); break;
        case 3: magic = dist(gen) & dist(gen) & dist(gen); break;
      }

      int score = score_magic(sq, magic, mask, !i);

      if (score && score < best_score)
      {
        best_score = score;
        best = magic;

        log("{} ", score);
      }
    }

    total_sz += best_score;

    if (!best) log("failed!");
    else helpers[sq].magic = best;
  }

  log("\n\ntotal size: {} * 16 bytes\n", total_sz);
  print_results();

  // +-------------------------------------------------
  //   2. Packing attsets from every square
  // +-------------------------------------------------

  u32 attacks[10'000]{};

  // 2.1. Shrinking attsets

  for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    // rebuild into attset[sq]
    score_magic(sq, helpers[sq].magic, get_mask(sq));

    int start = 0;
    for (start = 0; start < Atts; start++)
      if (attset[sq][start]) break;

    int end = start;
    for (int i = start; i < Atts; i++)
      if (attset[sq][i]) end = i;

    attsets[sq].offset = start;
    for (int i = start; i <= end; i++)
      attsets[sq].atts.push_back(attset[sq][i]);
  }

  // 2.2. Explore how pairs of attsets fits

  int len = 0;
  u32 used = 0u;
  for (int iter = 0; iter < SQ_N; iter++)
  {
    PackResult best;
    int score = -1;
    for (SQ sq = A1; sq < SQ_N; ++sq)
    {
      if (bit(sq) & used) continue;

      auto result = try_pack_attset(sq, attacks, len);

      if (result.fit > score)
      {
        score = result.fit;
        best = result;
      }
    }

    log("{} {} {} {}\n", best.sq, best.offset, best.len, best.fit);

    helpers[best.sq].offset = best.offset - attsets[best.sq].offset;
    len = best.len;
    used |= bit(best.sq);

    put_attset(best.sq, attacks, best.offset);
  }

  log("Total elements: {}\n", len);

  print_results();
}

}
