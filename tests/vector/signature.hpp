#pragma once

#include <vector.hpp>

namespace signature
{

template <typename T>
using VectorT = containers::Vector<T>;

using VectorInt = VectorT<int>;
using VectorBool = VectorT<bool>;

} // namespace signature
