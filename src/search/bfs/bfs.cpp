#include "bfs.hpp"
#include "../../utils/utils.hpp"

Solution bfs::search_bfs(const Board& initial_board, const std::vector<Piece>& initial_pieces) {
    Utils::SearchParams bfs_params;
    bfs_params.algorithm_name = "BFS";

    bfs_params.calculate_initial_val = [](const Board& board, const vector<Piece>& pieces) {
        return Utils::calculate(board, pieces);
    };

    bfs_params.successor_val = [](const Board& board_state, const vector<Piece>& next_pieces_state, const SearchNode&) {
        return Utils::calculate(board_state, next_pieces_state);
    };

    bfs_params.get_node_exploration = [](const SearchNode& node) {
        return godot::String("Heuristic: ") + godot::String::num_int64(node.val);
    };

    bfs_params.get_solution_details = [](const SearchNode&, const Solution& sol) {
        return godot::String("Moves: ") + godot::String::num_int64(sol.moves.size());
    };

    return Utils::search(initial_board, initial_pieces, bfs_params);
}