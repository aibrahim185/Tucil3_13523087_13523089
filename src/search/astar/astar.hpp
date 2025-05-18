#pragma once

#include <vector>
#include <queue>
#include <set>
#include <string>
#include <algorithm>
#include <chrono>

#include "../../utils/utils.hpp"

struct AStarSearchNode : public SearchNode {
    int h_cost;
    int actual_g_cost;

    AStarSearchNode(const vector<Piece>& p,
                    const Board& b,
                    const vector<PieceMove>& path_taken,
                    int g,
                    int h,
                    char pid_moved = ' ',
                    Coordinates opos_moved = {-1,-1})
        : SearchNode(p, b, path_taken, g + h, pid_moved, opos_moved),
          h_cost(h),
          actual_g_cost(g) {}

    bool operator>(const AStarSearchNode& other) const {
        if (this->val != other.val) {
            return this->val > other.val;
        }
        return this->h_cost > other.h_cost;
    }

    string get_state_string() const {
        string s = "";
        vector<Piece> sorted_pieces = this->pieces;

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