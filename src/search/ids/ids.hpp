#ifndef IDS_HPP
#define IDS_HPP

#include "../../main_scene/main_scene.hpp"
#include "../../utils/utils.hpp"

class ids {
public:
    static const int MAX_DEPTH_LIMIT = 100;
    static Solution search_ids(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif