#ifndef IDS_HPP
#define IDS_HPP

#include "../../main_scene/main_scene.hpp"
#include "../../utils/utils.hpp"

class ids {
public:
    static Solution search_ids(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif