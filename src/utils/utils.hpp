#pragma once

#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/variant/string.hpp>

#include <algorithm>
#include <chrono>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <functional>
#include <stack>

using namespace std;
using namespace godot;

enum AlgoType {
    BFS,
    UCS,
    ASTAR,
    IDS,
};

struct Coordinates {
    int x;
    int y;

    bool operator<(const Coordinates& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }

    bool operator==(const Coordinates& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Coordinates& other) const {
        return !(*this == other);
    }
};

struct Piece {
    char id;
    Coordinates coordinates;
    int size;
    bool is_vertical;
    bool is_primary;
};

struct Board {
    int rows;
    int cols;
    int other_pieces_count;
    int pixel_size;
    int pixel_padding;
    Coordinates exit_coordinates;
    
    vector<vector<char>> grid;
    int piece_padding = 1;
};

struct PieceMove {
    Coordinates old_coordinates;
    Coordinates new_coordinates;
};

struct Solution {
    vector<PieceMove> moves;
    chrono::duration<double, milli> duration;
    int node;
    bool is_solved;
    Solution() : node(0), is_solved(false) {}
};

struct SearchNode {
    vector<Piece> pieces;
    Board board;
    vector<PieceMove> path;
    int val;
    
    char piece_moved; 
    Coordinates original_position;

    SearchNode(const vector<Piece>& p, const Board& b, const vector<PieceMove>& path, int v, char pid = ' ', Coordinates opos = {-1,-1}) : pieces(p), board(b), path(path), val(v), piece_moved(pid), original_position(opos) {}

    bool operator>(const SearchNode& other) const {
        return val > other.val;
    }
};

class Utils {
public:
    static void print_board(Board& board,  vector<Piece>& pieces);
    static godot::String stringToGodotString(const string& stdString);
    static string godotStringToString(const godot::String& godotString);
    
    static int calculate(const Board& initial_board, const vector<Piece>& current_pieces);
    static bool is_exit(const Board& initial_board, const vector<Piece>& current_pieces);
    
    // helper function
    static const Piece* get_primary_piece(const vector<Piece>& pieces_list);
    static string state_to_string(const vector<Piece>& current_pieces);
    static bool is_cell_clear(const Board& initial_board, int r_check, int c_check, const vector<Piece>& pieces_in_state, char moving_piece_id);
    using ValueCalculator = function<int(const Board& board_state, const vector<Piece>& next_pieces_state, const SearchNode& parent_node)>;
    static vector<SearchNode> generate_next(const SearchNode& current_node, ValueCalculator calculate_node_value);

    struct SearchParams {
        string algorithm_name;
        function<int(const Board&, const vector<Piece>&)> calculate_initial_val;
        ValueCalculator successor_val;
        function<godot::String(const SearchNode&)> get_node_exploration;
        function<godot::String(const SearchNode&, const Solution&)> get_solution_details;
    };

    static Solution search(const Board& initial_board, const vector<Piece>& initial_pieces, const SearchParams& params);
};