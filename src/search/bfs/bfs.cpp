#include "bfs.hpp"
#include <godot_cpp/variant/utility_functions.hpp>
#include <iostream>
using namespace std;

bfs::bfs(const Board& initial_board, const vector<Piece>& initial_pieces) : initial_board_config(initial_board), initial_pieces_config(initial_pieces) {}
bfs::~bfs() {}

const Piece* bfs::get_primary_piece(const vector<Piece>& pieces_list) const {
    for (const auto& p : pieces_list) {
        if (p.is_primary) {
            return &p;
        }
    }
    return nullptr;
}

int bfs::calculate(const vector<Piece>& current_pieces) {
    const Piece* primary_piece = get_primary_piece(current_pieces);

    if (!primary_piece) {
        godot::UtilityFunctions::printerr("GBFS Error: Primary piece not found in heuristic calculation.");
        return -1; // kalau ga ada primary piece
    }

    int distance = 0;
    if (!primary_piece->is_vertical) { // bidak horizontal
        // jika pintu keluar ada di kanan bidak utama
        if (initial_board_config.exit_coordinates.x > primary_piece->coordinates.x) {
            distance = initial_board_config.exit_coordinates.x - (primary_piece->coordinates.x + primary_piece->size - 1);
        }
        else { // jika pintu keluar ada di kiri bidak utama
            distance = primary_piece->coordinates.x - initial_board_config.exit_coordinates.x;
        }
    }
    else { // bidak vertikal
        // jika pintu keluar ada di bawah bidak utama
        if (initial_board_config.exit_coordinates.y > primary_piece->coordinates.y) {
            distance = initial_board_config.exit_coordinates.y - (primary_piece->coordinates.y + primary_piece->size - 1);
        } 
        else { // jika pintu keluar ada di atas bidak utama
            distance = primary_piece->coordinates.y - initial_board_config.exit_coordinates.y;
        }
    }
    return abs(distance);
}

bool bfs::is_exit(const vector<Piece>& current_pieces) {
    const Piece* primary_piece = get_primary_piece(current_pieces);
    if (!primary_piece) {
        godot::UtilityFunctions::printerr("GBFS Error: Primary piece not found in goal state check.");
        return false;
    }

    if (!primary_piece->is_vertical) { // horizontal
        if (primary_piece->coordinates.y != initial_board_config.exit_coordinates.y) return false;
        
        // jika pintu keluar di kanan
        if (initial_board_config.exit_coordinates.x >= (primary_piece->coordinates.x + primary_piece->size -1) ) {
             return (primary_piece->coordinates.x + primary_piece->size - 1) == initial_board_config.exit_coordinates.x;
        } 
        else { // pintu keluar di kiri
             return primary_piece->coordinates.x == initial_board_config.exit_coordinates.x;
        }
    }
    else { // vertikal
        if (primary_piece->coordinates.x != initial_board_config.exit_coordinates.x) return false;

        // jika pintu keluar di bawah
        if (initial_board_config.exit_coordinates.y >= (primary_piece->coordinates.y + primary_piece->size -1)) {
            return (primary_piece->coordinates.y + primary_piece->size - 1) == initial_board_config.exit_coordinates.y;
        }
        else { // pintu keluar di atas
            return primary_piece->coordinates.y == initial_board_config.exit_coordinates.y;
        }
    }
    return false;
}

string bfs::state_to_string(const vector<Piece>& current_pieces) {
    string s = "";
    vector<Piece> sorted_pieces = current_pieces;

    // urutin berdasarkan id
    sort(sorted_pieces.begin(), sorted_pieces.end(), [](const Piece& a, const Piece& b) {
        return a.id < b.id;
    });

    // buat string gabungan per piece yang berisi info penting terkait koordinat dan arah
    for (const auto& piece : sorted_pieces) {
        if (piece.id == 'K') continue;
        s += piece.id;
        s += ':';
        s += to_string(piece.coordinates.x);
        s += ',';
        s += to_string(piece.coordinates.y);
        s += (piece.is_vertical ? "V" : "H");
        s += ";";
    }
    return s;
}

bool bfs::is_cell_clear(int r, int c, const vector<Piece>& pieces_state, char moving_piece_id) const {
    if (r < initial_board_config.piece_padding || r >= initial_board_config.rows + initial_board_config.piece_padding || c < initial_board_config.piece_padding || c >= initial_board_config.cols + initial_board_config.piece_padding) {
        const Piece* p_moving = nullptr;
        for(const auto& p_iter : pieces_state) {
            if(p_iter.id == moving_piece_id) {
                p_moving = &p_iter;
                break;
            }
        }
        if (p_moving && p_moving->is_primary && 
            r == initial_board_config.exit_coordinates.y && 
            c == initial_board_config.exit_coordinates.x) {
            return true;
        }
        return false;
    }

    for (const auto& piece : pieces_state) {
        if (piece.id == moving_piece_id || piece.id == 'K') { // jangan cek exit
            continue;
        }
        if (piece.is_vertical) {
            if (c == piece.coordinates.x && r >= piece.coordinates.y && r < piece.coordinates.y + piece.size) {
                return false;
            }
        } 
        else {
            if (r == piece.coordinates.y && c >= piece.coordinates.x && c < piece.coordinates.x + piece.size) {
                return false; // ditempati piece lain
            }
        }
    }
    return true;
}

vector<bfs::SearchNode> bfs::generate_next(const bfs::SearchNode& current_node) {
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
                        if (!is_cell_clear(y, piece_to_move.coordinates.x, current_pieces, piece_to_move.id)) {
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
                        if (!is_cell_clear(piece_to_move.coordinates.y, x, current_pieces, piece_to_move.id)) {
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

                int val = calculate(next_pieces_state);

                successors.emplace_back(next_pieces_state, initial_board_config, next_path, val, piece_to_move.id, piece_to_move.coordinates);
            }
        }
    }
    return successors;
}

Solution bfs::search() {
    auto time_start = chrono::high_resolution_clock::now();

    // inisialisasi
    Solution result;
    result.is_solved = false;
    result.node = 0;

    priority_queue<SearchNode, vector<SearchNode>, greater<SearchNode>> pq;
    set<string> visited;
    int initial_h = calculate(initial_pieces_config);
    SearchNode initial_node(initial_pieces_config, initial_board_config, {}, initial_h);
    pq.push(initial_node);
    
    godot::UtilityFunctions::print("GBFS: Initial heuristic: ", initial_h);

    while (!pq.empty()) {
        SearchNode current_node = pq.top();
        pq.pop();

        result.node++;

        string current_state_str = state_to_string(current_node.pieces);
        if (visited.count(current_state_str)) {
            continue;
        }
        visited.insert(current_state_str);
        
        godot::UtilityFunctions::print("GBFS: Exploring node. Heuristic: ", current_node.val, ". Path length: ", current_node.path.size());
        if (current_node.piece_moved != ' ') {
            godot::UtilityFunctions::print("Moved piece: ", String::utf8(&current_node.piece_moved, 1) , " from (", current_node.original_pos_moved_piece.x, ",", current_node.original_pos_moved_piece.y, ") to (", current_node.pieces[0].coordinates.x, ",", current_node.pieces[0].coordinates.y, ")");
        }


        if (is_exit(current_node.pieces)) {
            result.is_solved = true;
            result.moves = current_node.path;
            godot::UtilityFunctions::print("GBFS: Solution Found! Moves: ", result.moves.size(), " Nodes visited: ", result.node);
            break;
        }

        vector<SearchNode> next = generate_next(current_node);
        for (const auto& next_node : next) {
            string next_state_str = state_to_string(next_node.pieces);
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