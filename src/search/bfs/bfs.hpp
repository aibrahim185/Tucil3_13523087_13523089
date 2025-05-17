#ifndef BFS_HPP
#define BFS_HPP

#include <vector>
#include <queue>
#include <string>
#include <set>
#include <algorithm>
#include <chrono>

#include "../../main_scene/main_scene.hpp"

class bfs {
public:
    static Solution search_bfs(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif