#include "utils.hpp"

// --- Utils ---
void Utils::print_board(Board& board, std::vector<Piece>& pieces) {
    UtilityFunctions::print("Board:");
    vector<vector<char>> board_representation(board.rows + 2 * board.piece_padding, vector<char>(board.cols + 2 * board.piece_padding, ' '));
    board_representation[board.exit_coordinates.y][board.exit_coordinates.x] = 'K';
    for (Piece& piece : pieces) {
        for (int i = 0; i < piece.size; i++) {
            int x = piece.coordinates.x;
            int y = piece.coordinates.y;
            for (int j = 0; j < piece.size; j++) {
                if (piece.is_vertical) {
                    board_representation[y + j][x] = piece.id;
                } else {
                    board_representation[y][x + j] = piece.id;
                }
            }
        }
    }
    for (int i = 0; i < board_representation.size(); i++) {
        string line = "";
        for (int j = 0; j < board_representation[i].size(); j++) {
            if (board_representation[i][j] == ' ' && i >= board.piece_padding && i < board_representation.size() - board.piece_padding && j >= board.piece_padding && j < board_representation[i].size() - board.piece_padding) {
                line += '.';
            } else {
                line += board_representation[i][j];
            }
        }
        UtilityFunctions::print(stringToGodotString(line));
    }
}

godot::String Utils::stringToGodotString(const std::string& stdString) {
    return godot::String(stdString.c_str());
}

std::string Utils::godotStringToString(const godot::String& godotString) {
    return std::string(godotString.utf8().get_data());
}

int Utils::calculate(const Board& initial_board, const vector<Piece>& current_pieces) {
    const Piece* primary_piece = get_primary_piece(current_pieces);

    if (!primary_piece) {
        godot::UtilityFunctions::printerr("GBFS Error: Primary piece not found in heuristic calculation.");
        return -1; // kalau ga ada primary piece
    }

    int distance = 0;
    if (!primary_piece->is_vertical) { // bidak horizontal
        // jika pintu keluar ada di kanan bidak utama
        if (initial_board.exit_coordinates.x > primary_piece->coordinates.x) {
            distance = initial_board.exit_coordinates.x - (primary_piece->coordinates.x + primary_piece->size - 1);
        }
        else { // jika pintu keluar ada di kiri bidak utama
            distance = primary_piece->coordinates.x - initial_board.exit_coordinates.x;
        }
    }
    else { // bidak vertikal
        // jika pintu keluar ada di bawah bidak utama
        if (initial_board.exit_coordinates.y > primary_piece->coordinates.y) {
            distance = initial_board.exit_coordinates.y - (primary_piece->coordinates.y + primary_piece->size - 1);
        } 
        else { // jika pintu keluar ada di atas bidak utama
            distance = primary_piece->coordinates.y - initial_board.exit_coordinates.y;
        }
    }
    return abs(distance);
}

bool Utils::is_exit(const Board& initial_board, const vector<Piece>& current_pieces) {
    const Piece* primary_piece = get_primary_piece(current_pieces);
    if (!primary_piece) {
        godot::UtilityFunctions::printerr("GBFS Error: Primary piece not found in goal state check.");
        return false;
    }

    if (!primary_piece->is_vertical) { // horizontal
        if (primary_piece->coordinates.y != initial_board.exit_coordinates.y) return false;
        
        // jika pintu keluar di kanan
        if (initial_board.exit_coordinates.x >= (primary_piece->coordinates.x + primary_piece->size -1) ) {
             return (primary_piece->coordinates.x + primary_piece->size - 1) == initial_board.exit_coordinates.x;
        } 
        else { // pintu keluar di kiri
             return primary_piece->coordinates.x == initial_board.exit_coordinates.x;
        }
    }
    else { // vertikal
        if (primary_piece->coordinates.x != initial_board.exit_coordinates.x) return false;

        // jika pintu keluar di bawah
        if (initial_board.exit_coordinates.y >= (primary_piece->coordinates.y + primary_piece->size -1)) {
            return (primary_piece->coordinates.y + primary_piece->size - 1) == initial_board.exit_coordinates.y;
        }
        else { // pintu keluar di atas
            return primary_piece->coordinates.y == initial_board.exit_coordinates.y;
        }
    }
    return false;
}


const Piece* Utils::get_primary_piece(const vector<Piece>& pieces_list) {
    for (const auto& p : pieces_list) {
        if (p.is_primary) {
            return &p;
        }
    }
    return nullptr;
}

string Utils::state_to_string(const vector<Piece>& current_pieces) {
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

bool Utils::is_cell_clear(const Board& initial_board, int r, int c, const vector<Piece>& pieces_state, char moving_piece_id) {
    if (r < initial_board.piece_padding || r >= initial_board.rows + initial_board.piece_padding || c < initial_board.piece_padding || c >= initial_board.cols + initial_board.piece_padding) {
        const Piece* p_moving = nullptr;
        for(const auto& p_iter : pieces_state) {
            if(p_iter.id == moving_piece_id) {
                p_moving = &p_iter;
                break;
            }
        }
        if (p_moving && p_moving->is_primary && 
            r == initial_board.exit_coordinates.y && 
            c == initial_board.exit_coordinates.x) {
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

vector<SearchNode> Utils::generate_next(const SearchNode& current_node, ValueCalculator calculate_node_value) {
    vector<SearchNode> successors;
    const vector<Piece>& current_pieces = current_node.pieces;
    const Board& board_node = current_node.board;

    for (size_t piece_idx = 0; piece_idx < current_pieces.size(); ++piece_idx) {
        const Piece& piece_to_move = current_pieces[piece_idx];
        if (piece_to_move.id == 'K') continue; // jangan gerakin exit
        
        // direction: -1 = kiri/atas, 1 = kanan/bawah
        for (int direction = -1; direction <= 1; direction += 2) {
            if (direction == 0) continue;
            for (int steps = 1; ; ++steps) {
                vector<Piece> next_pieces_state = current_pieces;
                Piece& piece_moved = next_pieces_state[piece_idx];
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
                        if (!is_cell_clear(board_node, y, piece_to_move.coordinates.x, current_pieces, piece_to_move.id)) {
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
                        if (!is_cell_clear(board_node, piece_to_move.coordinates.y, x, current_pieces, piece_to_move.id)) {
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

                int val = calculate_node_value(board_node, next_pieces_state, current_node);
                successors.emplace_back(next_pieces_state, board_node, next_path, val, piece_to_move.id, piece_to_move.coordinates);
            }
        }
    }
    return successors;
}

Solution Utils::search(const Board& initial_board, const vector<Piece>& initial_pieces, const SearchParams& params) {
    auto time_start = chrono::high_resolution_clock::now();
    Solution result;
    result.is_solved = false;
    result.node = 0;

    priority_queue<SearchNode, vector<SearchNode>, greater<SearchNode>> pq;
    set<string> visited;

    int initial_val = params.calculate_initial_val(initial_board, initial_pieces);
    SearchNode initial_node(initial_pieces, initial_board, {}, initial_val);
    pq.push(initial_node);

    while (!pq.empty()) {
        SearchNode current_node = pq.top();
        pq.pop();
        result.node++;

        string current_state_str = state_to_string(current_node.pieces);
        if (visited.count(current_state_str)) {
            continue;
        }
        visited.insert(current_state_str);

        if (is_exit(current_node.board, current_node.pieces)) {
            result.is_solved = true;
            result.moves = current_node.path;
            break;
        }

        vector<SearchNode> successors = generate_next(
            current_node,
            params.successor_val
        );

        for (const auto& successor_node : successors) {
            string next_state_str = state_to_string(successor_node.pieces);
            if (!visited.count(next_state_str)) {
                pq.push(successor_node);
            }
        }
    }

    auto time_end = chrono::high_resolution_clock::now();
    result.duration = chrono::duration<double, milli>(time_end - time_start);

    if (!result.is_solved) {
        godot::UtilityFunctions::print(godot::String::utf8(params.algorithm_name.c_str()) + ": No solution found. Nodes visited: " + godot::String::num_int64(static_cast<int64_t>(result.node)));
    }
    return result;
}