#ifndef RDICT_H
#define RDICT_H

#include <RName.h>
#include <unordered_map>
#include <any>

namespace Redopera {

template<typename  Value = std::any>
using RDict = std::unordered_map<RName, Value>;

} // ns Redopera

#endif // RDICT_H
