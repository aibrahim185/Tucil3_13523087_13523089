#pragma once

#include <vector>
#include <queue>
#include <set>
#include <string>
#include <algorithm>
#include <chrono>

#include "../../utils/utils.hpp"

struct AStarSearchNode {
    vector<Piece> pieces;
    Board board;
    vector<PieceMove> path;
    int g_cost;
    int h_cost;
    int f_cost;

    char piece_moved;
    Coordinates original_position;

    AStarSearchNode(const vector<Piece>& p, const Board& b, const vector<PieceMove>& path, int g, int h, char pid = ' ', Coordinates opos = {-1,-1})
        : pieces(p), board(b), path(path), g_cost(g), h_cost(h), f_cost(g + h), piece_moved(pid), original_position(opos) {}

    // operator untuk priority queue
    bool operator>(const AStarSearchNode& other) const {
        if (f_cost != other.f_cost) {
            return f_cost > other.f_cost;
        }
        // jika f_cost sama, bisa menggunakan h_cost sebagai tie-breaker
        return h_cost > other.h_cost;
    }

    // fungsi untuk mendapatkan representasi string dari state untuk set 'visited'
    string get_state_string() const {
        string s = "";
        vector<Piece> sorted_pieces = pieces;

        sort(sorted_pieces.begin(), sorted_pieces.end(), [](const Piece& a, const Piece& b) {
            return a.id < b.id;
        });

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
};

class astar {
public:
    static Solution search_astar(const Board& initial_board, const vector<Piece>& initial_pieces);

private:
    static vector<AStarSearchNode> generate_successors_astar(const AStarSearchNode& current_node, const int COST_PER_MOVE);
};