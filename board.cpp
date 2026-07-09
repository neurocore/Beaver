#include <format>
#include "board.h"
#include "bitboard.h"

using namespace std;

namespace beaver {

Board::Board(const Board & board)
{
  piece[0][0] = board.piece[0][0];
  piece[0][1] = board.piece[0][1];
  piece[1][0] = board.piece[1][0];
  piece[1][1] = board.piece[1][1];

  color = board.color;
  hash = board.hash;
}

void Board::clear()
{
  piece[0][0] = 0u;
  piece[0][1] = 0u;
  piece[1][0] = 0u;
  piece[1][1] = 0u;

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

Piece Board::square(SQ sq) const
{
  const u32 bb = bit(sq);
  if (bb & piece[0][0]) return BP;
  if (bb & piece[0][1]) return WP;
  if (bb & piece[1][0]) return BK;
  if (bb & piece[1][1]) return WK;
  return NOP;
}

bool Board::set(string fen)
{
  // TODO: remove non-playable white squares

  SQ sq = B8;
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

    if (!(sq & 3)) // row wrap
    {
      sq -= 8;
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

string Board::to_string() const
{
  string str;
  for (int y = 7; y >= 0; --y)
  {
    str += format(" {} | ", y + 1);

    for (int x = 0; x < 8; ++x)
    {
      if (invalid(x, y)) str += "  ";
      else
      {
        SQ sq = to_sq(x, y);
        Piece p = square(sq);
      
        str += format("{} ", "xoX0."[p]);
      }
    }
    str += "\n";
  }
  str += "   +----------------  ";
  str += format("<{}> \n", color ? "W" : "B");
  str += "     a b c d e f g h   \n\n";
  return str;
}

void Board::place(SQ sq, Piece p)
{
  piece[pt(p)][col(p)] |= bit(sq);

  //hash ^= Zobrist::key[p][sq];
}

void Board::remove(SQ sq, Piece p)
{
  piece[pt(p)][col(p)] &= ~bit(sq);

  //hash ^= Zobrist::key[p][sq];
}

INLINE u32 Board::occupied() const
{
  return piece[0][0] | piece[0][1] | piece[1][0] | piece[1][1];
}

}
