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

  build_magics();
  
  Board B;
  B.set();
  log("{}", B.to_string());

  /*print32(between[A1][D4]);
  print32(between[D4][A1]);
  print32(between[G1][D4]);
  print32(between[B4][D4]);*/

  return 0;
}
