// Copyright (c) Tyler Veness

#include "utils.hpp"

#include <cmath>

unsigned int nextPowerTwo(unsigned int num) {
  return std::exp2(std::ceil(std::log2(static_cast<double>(num))));
}
