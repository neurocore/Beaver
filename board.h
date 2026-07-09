#pragma once
#include <string>
#include "piece.h"
#include "square.h"
#include "moves.h"
#include "magics.h"
#include "consts.h"
#include "utils.h"

namespace beaver {

struct Board
{
  Color color;
  u32 piece[PT_N][Color_N];
  u64 hash;

public:
  Board() { clear(); }  
  Board(const Board & board);

  void clear();

  bool is_draw() const;
  bool is_repetition() const;
  Piece square(SQ sq) const;

  template<Color Col>
  bool is_opp(SQ sq) const;

  u64 calc_hash() const;

  bool set(std::string fen = Pos::Init);
  std::string to_fen();
  std::string to_string() const;

  bool make(Move move);
  void unmake(Move move);

  void make_null();
  void unmake_null();

  INLINE u32 occupied() const;

  INLINE void generate_all(MoveList & ml) const;
  void generate_legal(MoveList & ml);

  template<Color Col>
  INLINE int extend_attack(Move move, MoveList & extensions) const;

  template<Color Col>
  void generate_quiets(MoveList & ml) const;

  template<Color Col>
  void generate_attacks(MoveList & ml) const;

private:
  inline void place(SQ sq, Piece p);
  inline void remove(SQ sq, Piece p);
};

template<Color Col>
inline bool Board::is_opp(SQ sq) const
{
  return valid(sq) && col(square[sq]) == ~Col;
}

template<Color Col>
inline void Board::generate_quiets(MoveList & ml) const
{
  constexpr u32 promoted = Col ? PromW : PromB;
  const u32 occ = occupied();

  // --------------------------------
  //  Pawns
  // --------------------------------

  for (u32 bb = piece[Pawn][Col]; bb; bb = rlsb(bb))
  {
    SQ sq = bitscan(bb);
    
    for (u32 att = pmov[Col][sq] & ~occ & ~promoted; att; att = rlsb(att))
    {
      SQ j = bitscan(att);
      ml.add(mv_quiet(sq, j));
    }
  }

  // --------------------------------
  //  Kings
  // --------------------------------

  for (u32 bb = piece[King][Col]; bb; bb = rlsb(bb))
  {
    SQ sq = bitscan(bb);
    
    for (u32 att = k_att(occ, sq) & ~occ; att; att = rlsb(att))
    {
      SQ j = bitscan(att);
      ml.add(mv_quiet(sq, j));
    }
  }
}

template<Color Col>
INLINE int Board::extend_attack(Move move, MoveList & steps) const
{
  const u32 occ = occupied();

  switch (move.flags)
  {
    case Quiet: case Cap: // Pawn atts
    {

      break;
    }

    case Prom: case CapProm: // King atts
    {
      u32 att = k_att(occ, move.from);
      u32 blockers = att & occ;
      u32 att2 = k_att(occ & ~blockers, move.from);
      break;
    }
  }
}

template<Color Col>
inline void Board::generate_attacks(MoveList & ml) const
{
  MoveList candidates;

  // --------------------------------
  //  1. Generate candidates
  // --------------------------------

  for (u32 bb = piece[Pawn][Col]; bb; bb = rlsb(bb))
  {
    SQ sq = bitscan(bb);
    extend_attack(mv_quiet(sq, sq), candidates);
  }

  // --------------------------------
  //  2. Iterate and try to extend
  // --------------------------------

  while (!candidates.empty())
  {
    Move move = candidates.pop_back();

    if (!extend_attack<Col>(move, candidates))
    {
      ml.add_unique(move);
    }
  }
}

}
