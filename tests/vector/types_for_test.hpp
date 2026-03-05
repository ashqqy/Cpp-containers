#include <gtest/gtest.h>

#include <cstdint>
#include <deque>
#include <forward_list>
#include <list>

#include "signature.hpp"

using TestTypes = ::testing::Types<bool, int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t,
                                   uint32_t, uint64_t, float, double, signature::VectorInt,
                                   std::deque<int>, std::list<int>, std::forward_list<int>>;
