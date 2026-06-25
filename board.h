#pragma once
#include <string>
#include "piece.h"
#include "square.h"
#include "moves.h"
#include "consts.h"

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

  INLINE void generate_all(MoveList & ml) const;
  void generate_legal(MoveList & ml);

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
  if constexpr (Col)
  {
    for (auto sq : piece[Pawn][Col])
    {
      if (is_empty<Col>(sq + 7)) ml.add(mv_quiet(sq, sq + 7));
      if (is_empty<Col>(sq + 9)) ml.add(mv_quiet(sq, sq + 9));
    }
  }
  else
  {
    for (auto sq : piece[Pawn][Col])
    {
      if (is_empty<Col>(sq - 7)) ml.add(mv_quiet(sq, sq - 7));
      if (is_empty<Col>(sq - 9)) ml.add(mv_quiet(sq, sq - 9));
    }
  }

  for (int shift : {-7, -9, 7, 9})
  {
    for (auto sq : piece[King][Col])
    {
      for (SQ j = sq + shift; is_empty(j); j += shift)
      {
        ml.add(mv_quiet(sq, j));
      }
    }
  }
}

template<Color Col>
inline void Board::generate_attacks(MoveList & ml) const
{
  // TODO: 
}

}
