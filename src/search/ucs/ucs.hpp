#ifndef UCS_HPP
#define UCS_HPP

#include <vector>
#include <queue>
#include <string>
#include <set>
#include <algorithm>
#include <chrono>

#include "../../main_scene/main_scene.hpp"

class ucs {
public:
    static Solution search_ucs(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif