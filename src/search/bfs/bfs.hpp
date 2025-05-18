#ifndef BFS_HPP
#define BFS_HPP

#include "../../main_scene/main_scene.hpp"
#include "../../utils/utils.hpp"

class bfs {
public:
    static Solution search_bfs(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif