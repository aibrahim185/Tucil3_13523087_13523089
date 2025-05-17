#ifndef BFS_HPP
#define BFS_HPP

#include <vector>
#include <queue>
#include <string>
#include <set>
#include <algorithm>
#include <chrono>
#include "../search.hpp"
#include "../../main_scene/main_scene.hpp"

class bfs : public Search {
public:
    // ctor: menerima konfigurasi awal papan, mobil, dan pintu keluar
    bfs(const Board& initial_board, const std::vector<Piece>& initial_pieces);
    // dtor
    ~bfs();

    // override fungsi search dari kelas dasar
    Solution search() override;

private:
    struct SearchNode {
        std::vector<Piece> pieces;
        Board board;
        std::vector<PieceMove> path;
        int val; // nilai heuristik dari state ini
        
        char piece_moved; 
        Coordinates original_pos_moved_piece;

        SearchNode(const std::vector<Piece>& p, const Board& b, const std::vector<PieceMove>& path, int h, char pid = ' ', Coordinates opos = {-1,-1}) : pieces(p), board(b), path(path), val(h), piece_moved(pid), original_pos_moved_piece(opos) {}

        bool operator>(const SearchNode& other) const {
            return val > other.val;
        }
    };

    Board initial_board_config;
    std::vector<Piece> initial_pieces_config;

    int calculate(const std::vector<Piece>& current_pieces);
    std::vector<SearchNode> generate_next(const SearchNode& current_node);
    bool is_exit(const std::vector<Piece>& current_pieces);
    
    // helper function
    const Piece* get_primary_piece(const std::vector<Piece>& pieces_list) const;
    std::string state_to_string(const std::vector<Piece>& current_pieces);
    bool is_cell_clear(int r_check, int c_check, const std::vector<Piece>& pieces_in_state, char moving_piece_id) const;
};

#endif