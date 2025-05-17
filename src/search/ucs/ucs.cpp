#include "ucs.hpp"
#include "../../utils/utils.hpp"

Solution ucs::search_ucs(const Board& initial_board, const std::vector<Piece>& initial_pieces) {
    const int COST_PER_MOVE = 1;

    Utils::SearchParams ucs_params;
    ucs_params.algorithm_name = "UCS";

    ucs_params.calculate_initial_val = [](const Board&, const vector<Piece>&) {
        return 0;
    };

    ucs_params.successor_val = [COST_PER_MOVE](const Board&, const vector<Piece>&, const SearchNode& parent_node) {
        return parent_node.val + COST_PER_MOVE;
    };

    ucs_params.get_node_exploration = [](const SearchNode& node) {
        return godot::String("Cost: ") + godot::String::num_int64(node.val);
    };

    ucs_params.get_solution_details = [](const SearchNode& node, const Solution&) {
        return godot::String("Optimal steps: ") + godot::String::num_int64(node.val);
    };
    return Utils::search(initial_board, initial_pieces, ucs_params);
}