#pragma once

#include "types.h"
#include "drawable.hpp"

#include <memory>

struct CDrawableDictionary {
    shared_vector<CDrawable> Drawables;
};
