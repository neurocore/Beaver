#pragma once
#include <string>

namespace beaver {

using Str = const std::string;

const std::string Name = "Beaver";
const std::string Vers = "0.01";
const std::string Auth = "Nick Kurgin";

enum Time
{
  Def = 60000,
  Inc =  1000,
};

enum Limits
{
  Plies = 128,
  M0ves = 256,
};

namespace Pos
{
  Str Init = "pppp/pppp/pppp/4/4/PPPP/PPPP/PPPP w 0";
}

}
