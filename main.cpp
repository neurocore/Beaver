#include <iostream>
#include "utils.h"
#include "square.h"
#include "board.h"
#include "consts.h"
#include "tables.h"
#include "bitboard.h"
#include "magics.h"

using namespace std;
using namespace beaver;

int main()
{
  if (Input.is_console())
  {
    say<1>("{} v{} - russian checkers engine by {}\n", Name, Vers, Auth);
  }

  //build_magics();
  
  Board B;
  B.set();
  log("{}", B.to_string());

  /*u32 bb = Full;
  print32(bb);
  print32(shift2<UL>(bb));
  print32(bb);
  print32(shift2<UR>(bb));
  print32(bb);
  print32(shift2<DL>(bb));
  print32(bb);
  print32(shift2<DR>(bb));*/

  /*for (SQ sq = A1; sq < SQ_N; ++sq)
  {
    log("{}\n", sq);
    print32(k_att(0u, sq));
  }*/

  /*print32(between[A1][D4]);
  print32(between[D4][A1]);
  print32(between[G1][D4]);
  print32(between[B4][D4]);*/

  return 0;
}
