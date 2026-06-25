#pragma once
#include <array>
#include "piece.h"
#include "square.h"

namespace beaver {

enum Dir { UL, UR, DL, DR, Dir_N };

using SQ_BB  = std::array<u32, SQ_NN>;
using SQ_SQ  = std::array<SQ,  SQ_NN>;
using SQ_Val = std::array<int, SQ_NN>;

extern const std::array<SQ_BB, Color_N> pmov, patt;
extern const std::array<SQ_BB, SQ_NN> between;
extern const std::array<SQ_BB, Dir_N> step;

}
