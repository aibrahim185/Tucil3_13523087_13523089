#include "bfs.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <iostream>
using namespace std;

vector<SearchNode> bfs::generate_next_bfs(const Board& initial_board, const SearchNode& current_node) {
    vector<SearchNode> successors;
    const vector<Piece>& current_pieces = current_node.pieces;

    for (size_t piece_idx = 0; piece_idx < current_pieces.size(); ++piece_idx) {
        const Piece& piece_to_move = current_pieces[piece_idx];
        if (piece_to_move.id == 'K') continue; // jangan gerakin exit
        
        // direction: -1 = kiri/atas, 1 = kanan/bawah
        for (int direction = -1; direction <= 1; direction += 2) {
            if (direction == 0) continue;
            for (int steps = 1; ; ++steps) {
                vector<Piece> next_pieces_state = current_pieces;
                Piece& piece_moved = next_pieces_state[piece_idx];

                Coordinates new_top_left_coordinate = piece_to_move.coordinates;
                bool move = true;

                if (piece_to_move.is_vertical) {
                    for (int s = 1; s <= steps; ++s) {
                        int y;
                        if (direction > 0) { // bergerak ke bawah
                            y = piece_to_move.coordinates.y + piece_to_move.size - 1 + s;
                        }
                        else { // bergerak ke atas
                            y = piece_to_move.coordinates.y - s;
                        }
                        if (!Utils::is_cell_clear(initial_board, y, piece_to_move.coordinates.x, current_pieces, piece_to_move.id)) {
                            move = false;
                            break;
                        }
                    }
                    if (move) {
                         piece_moved.coordinates.y += (direction * steps);
                    }
                }
                else { // bergerak horizontal
                    for (int s = 1; s <= steps; ++s) {
                        int x;
                        if (direction > 0) { // bergerak ke kanan
                            x = piece_to_move.coordinates.x + piece_to_move.size - 1 + s;
                        }
                        else { // bergerak ke kiri
                            x = piece_to_move.coordinates.x - s;
                        }
                        if (!Utils::is_cell_clear(initial_board, piece_to_move.coordinates.y, x, current_pieces, piece_to_move.id)) {
                            move = false;
                            break;
                        }
                    }
                     if (move) {
                        piece_moved.coordinates.x += (direction * steps);
                    }
                }

                if (!move) {
                    break;
                }

                vector<PieceMove> next_path = current_node.path;
                PieceMove current_pm;
                current_pm.old_coordinates = piece_to_move.coordinates;
                current_pm.new_coordinates = piece_moved.coordinates;
                next_path.push_back(current_pm);

                int val = Utils::calculate(initial_board, next_pieces_state);

                successors.emplace_back(next_pieces_state, initial_board, next_path, val, piece_to_move.id, piece_to_move.coordinates);
            }
        }
    }
    return successors;
}

Solution bfs::search_bfs(const Board& initial_board, const std::vector<Piece>& initial_pieces) {
    auto time_start = chrono::high_resolution_clock::now();

    // inisialisasi
    Solution result;
    result.is_solved = false;
    result.node = 0;

    priority_queue<SearchNode, vector<SearchNode>, greater<SearchNode>> pq;
    set<string> visited;
    int initial_h = Utils::calculate(initial_board, initial_pieces);
    SearchNode initial_node(initial_pieces, initial_board, {}, initial_h);
    pq.push(initial_node);
    
    godot::UtilityFunctions::print("GBFS: Initial heuristic: ", initial_h);

    while (!pq.empty()) {
        SearchNode current_node = pq.top();
        pq.pop();

        result.node++;

        string current_state_str = Utils::state_to_string(current_node.pieces);
        if (visited.count(current_state_str)) {
            continue;
        }
        visited.insert(current_state_str);
        
        godot::UtilityFunctions::print("GBFS: Exploring node. Heuristic: ", current_node.val, ". Path length: ", current_node.path.size());
        if (current_node.piece_moved != ' ') {
            godot::UtilityFunctions::print("Moved piece: ", String::utf8(&current_node.piece_moved, 1) , " from (", current_node.original_pos_moved_piece.x, ",", current_node.original_pos_moved_piece.y, ") to (", current_node.pieces[0].coordinates.x, ",", current_node.pieces[0].coordinates.y, ")");
        }


        if (Utils::is_exit(initial_board, current_node.pieces)) {
            result.is_solved = true;
            result.moves = current_node.path;
            godot::UtilityFunctions::print("GBFS: Solution Found! Moves: ", result.moves.size(), " Nodes visited: ", result.node);
            break;
        }

        vector<SearchNode> next = generate_next_bfs(initial_board, current_node);
        for (const auto& next_node : next) {
            string next_state_str = Utils::state_to_string(next_node.pieces);
            if (!visited.count(next_state_str)) {
                pq.push(next_node);
            }
        }
    }

    auto time_end = chrono::high_resolution_clock::now();
    result.duration = chrono::duration<double, milli>(time_end - time_start);
    
    if (!result.is_solved) {
        godot::UtilityFunctions::print("GBFS: No solution found. Nodes visited: ", result.node);
    }

    return result;
}