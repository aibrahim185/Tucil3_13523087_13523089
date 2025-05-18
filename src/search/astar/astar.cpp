#include "astar.hpp"

vector<AStarSearchNode> astar::generate_successors_astar(const AStarSearchNode& current_astar_node, const int COST_PER_MOVE) {
    vector<AStarSearchNode> successors;
    const vector<Piece>& current_pieces = current_astar_node.pieces;
    const Board& board_node = current_astar_node.board;

    for (size_t piece_idx = 0; piece_idx < current_pieces.size(); ++piece_idx) {
        const Piece& piece_to_move = current_pieces[piece_idx];
        if (piece_to_move.id == 'K') continue;

        for (int direction = -1; direction <= 1; direction += 2) {
            for (int steps = 1; ; ++steps) {
                vector<Piece> next_pieces_state = current_pieces;
                Piece& piece_moved_in_state = next_pieces_state[piece_idx]; // ambil referensi ke piece yang akan digerakkan di state baru
                bool move_possible = true;

                Coordinates original_coords_of_moving_piece = piece_to_move.coordinates; // simpan koordinat asli sebelum modifikasi

                if (piece_to_move.is_vertical) {
                    for (int s = 1; s <= steps; ++s) {
                        int y_check;
                        if (direction > 0) {
                            y_check = piece_to_move.coordinates.y + piece_to_move.size - 1 + s;
                        } else {
                            y_check = piece_to_move.coordinates.y - s;
                        }
                        if (!Utils::is_cell_clear(board_node, y_check, piece_to_move.coordinates.x, current_pieces, piece_to_move.id)) {
                            move_possible = false;
                            break;
                        }
                    }
                    if (move_possible) {
                        piece_moved_in_state.coordinates.y += (direction * steps);
                    }
                } else {
                    for (int s = 1; s <= steps; ++s) {
                        int x_check;
                        if (direction > 0) {
                            x_check = piece_to_move.coordinates.x + piece_to_move.size - 1 + s;
                        } else {
                            x_check = piece_to_move.coordinates.x - s;
                        }
                        if (!Utils::is_cell_clear(board_node, piece_to_move.coordinates.y, x_check, current_pieces, piece_to_move.id)) {
                            move_possible = false;
                            break;
                        }
                    }
                    if (move_possible) {
                        piece_moved_in_state.coordinates.x += (direction * steps);
                    }
                }

                if (!move_possible) {
                    break;
                }

                vector<PieceMove> next_path = current_astar_node.path;
                PieceMove current_pm;
                current_pm.old_coordinates = original_coords_of_moving_piece; // gunakan koordinat asli
                current_pm.new_coordinates = piece_moved_in_state.coordinates; // koordinat baru setelah bergerak
                next_path.push_back(current_pm);

                int g_cost_successor = current_astar_node.g_cost + COST_PER_MOVE; // biaya pergerakan umumnya 1
                int h_cost_successor = Utils::calculate(board_node, next_pieces_state);

                successors.emplace_back(next_pieces_state, board_node, next_path, g_cost_successor, h_cost_successor, piece_to_move.id, original_coords_of_moving_piece);
            }
        }
    }
    return successors;
}


Solution astar::search_astar(const Board& initial_board, const vector<Piece>& initial_pieces) {
    auto time_start = chrono::high_resolution_clock::now();
    Solution result;
    result.is_solved = false;
    result.node = 0;

    const int COST_PER_MOVE = 1; // biaya setiap gerakan adalah 1

    priority_queue<AStarSearchNode, vector<AStarSearchNode>, greater<AStarSearchNode>> pq;
    set<string> visited_states;

    // node awal
    int initial_g_cost = 0;
    int initial_h_cost = Utils::calculate(initial_board, initial_pieces);
    AStarSearchNode initial_astar_node(initial_pieces, initial_board, {}, initial_g_cost, initial_h_cost);
    pq.push(initial_astar_node);

    UtilityFunctions::print("ASTAR: Initial g_cost: 0, h_cost: " + godot::String::num_int64(initial_h_cost) + ", f_cost: " + godot::String::num_int64(initial_astar_node.f_cost));

    while (!pq.empty()) {
        AStarSearchNode current_node = pq.top();
        pq.pop();
        result.node++;

        string current_state_str = current_node.get_state_string();
        if (visited_states.count(current_state_str)) {
            continue;
        }
        visited_states.insert(current_state_str);

        UtilityFunctions::print("ASTAR: Exploring node. " +
                                       godot::String("f(n):") + godot::String::num_int64(current_node.f_cost) +
                                       godot::String(", g(n):") + godot::String::num_int64(current_node.g_cost) +
                                       godot::String(", h(n):") + godot::String::num_int64(current_node.h_cost) +
                                       ". Path length: " + godot::String::num_int64(static_cast<int64_t>(current_node.path.size())));
        
        if (current_node.piece_moved != ' ') {
            Coordinates new_pos_for_log = {-1, -1};
            // cari piece yang digerakkan di state saat ini untuk mendapatkan posisi barunya
            for(const auto& p_state : current_node.pieces) {
                if (p_state.id == current_node.piece_moved) {
                    new_pos_for_log = p_state.coordinates;
                    break;
                }
            }
            UtilityFunctions::print("Moved piece: ", godot::String::utf8(&current_node.piece_moved, 1), " from (", current_node.original_position.x, ",", current_node.original_position.y, ") to (", new_pos_for_log.x, ",", new_pos_for_log.y, ")");
        }


        if (Utils::is_exit(current_node.board, current_node.pieces)) {
            result.is_solved = true;
            result.moves = current_node.path;
            UtilityFunctions::print("ASTAR: Solution Found! Optimal steps (g_cost): " + godot::String::num_int64(current_node.g_cost) + ". Total moves: " + godot::String::num_int64(result.moves.size()) + ". Nodes visited: " + godot::String::num_int64(static_cast<int64_t>(result.node)));
            break;
        }

        vector<AStarSearchNode> successors = generate_successors_astar(current_node, COST_PER_MOVE);

        for (const auto& successor_node : successors) {
            string next_state_str = successor_node.get_state_string();
            if (!visited_states.count(next_state_str)) {
                pq.push(successor_node);
            }
        }
    }

    auto time_end = chrono::high_resolution_clock::now();
    result.duration = chrono::duration<double, milli>(time_end - time_start);

    if (!result.is_solved) {
        UtilityFunctions::print("ASTAR: No solution found. Nodes visited: " + godot::String::num_int64(static_cast<int64_t>(result.node)));
    }
    return result;
}