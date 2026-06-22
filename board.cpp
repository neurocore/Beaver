#include "board.h"

using namespace std;

namespace beaver {

Board::Board(const Board & board)
{
  for (SQ sq = A1; sq < SQ_N; ++sq) square[sq] = board.square[sq];

  piece[0][0] = board.piece[0][0];
  piece[0][1] = board.piece[0][1];
  piece[1][0] = board.piece[1][0];
  piece[1][1] = board.piece[1][1];

  color = board.color;
  hash = board.hash;
}

void Board::clear()
{
  for (SQ sq = A1; sq < SQ_N; ++sq) square[sq] = NOP;

  piece[0][0].clear();
  piece[0][1].clear();
  piece[1][0].clear();
  piece[1][1].clear();

  color = White;
  hash = 0ull;
}

bool Board::is_draw() const
{
  if (is_repetition()) return true;

  // TODO: more conditions

  return false;
}

bool Board::is_repetition() const
{
  const u64 key = hash;

  // TODO: implement threefold[]

  return false;
}

bool Board::is_empty(SQ sq) const
{
  return valid(sq) && square[sq] == NOP;
}

bool Board::set(string fen)
{
  // TODO: remove non-playable white squares

  SQ sq = A8;
  clear();

  string fen_board = cut(fen); // parsing main part
  for (char ch : fen_board)
  {
    if (isdigit(ch)) sq += ch - '0';
    else
    {
      Piece p = to_piece(ch);
      if (p == NOP) continue;

      place(sq, p);
      ++sq;
    }

    if (!(sq & 7)) // row wrap
    {
      sq -= 16;
      if (sq < 0) break;
    }
  }

  string fen_color = cut(fen); // parsing color
  for (char ch : fen_color)
    color = to_color(ch);

  //string fen_cnt = cut(fen); // full move counter
  //moves_cnt = parse_int(fen_cnt);

  //hash ^= color ? 0ull : Zobrist::turn;

  return true;
}

void Board::print() const
{
  for (int y = 7; y >= 0; --y)
  {
    cout << ' ' << (y + 1) << " | ";

    for (int x = 0; x < 8; ++x)
    {
      SQ sq = to_sq(x, y);
      Piece p = square[sq];
      
      cout << to_char(p) << ' ';
    }
    cout << "\n";
  }
  cout << "   +----------------  ";
  cout << (color ? "<W>" : "<B>") << " \n";
  cout << "     a b c d e f g h   \n\n";
}

void Board::place(SQ sq, Piece p)
{
  square[sq] = p;

  auto & pieces = piece[pt(p)][color];
  pieces.add(sq);

  //hash ^= Zobrist::key[p][sq];
}

void Board::remove(SQ sq)
{
  Piece p = square[sq];
  assert(p < Piece_N);

  square[sq] = NOP;

  auto & pieces = piece[pt(p)][color];
  size_t i = pieces.find(sq);
  assert(i < pieces.capacity());

  pieces.erase(i);

  //hash ^= Zobrist::key[p][sq];
}

}
