#ifndef UCS_HPP
#define UCS_HPP

#include "../../main_scene/main_scene.hpp"
#include "../../utils/utils.hpp"

class ucs {
public:
    static Solution search_ucs(const Board& initial_board, const std::vector<Piece>& initial_pieces);
};

#endif