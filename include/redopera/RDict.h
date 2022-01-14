#ifndef RDICT_H
#define RDICT_H

#include <RName.h>
#include <variant>
#include <unordered_map>

namespace Redopera {

template<typename Value>
using RDict = std::unordered_map<RName, Value>;

} // ns Redopera

#endif // RDICT_H
