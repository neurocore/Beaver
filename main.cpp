#include <iostream>
#include "utils.h"
#include "square.h"
#include "board.h"

using namespace std;
using namespace beaver;

int main()
{
  if (Input.is_console())
  {
    say<1>("Beaver v0.01 - russian checkers playing engine\n");
  }
  return 0;
}
