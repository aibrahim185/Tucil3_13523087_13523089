#pragma once

#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/file_dialog.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include <fstream>

#include <vector>

using namespace godot;
using namespace std;

enum PieceType {
    BFS,
    UCS,
    ASTAR,
};

struct Coordinates {
    int x;
    int y;
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
    
    vector<vector<bool>> is_occupied;
    int piece_padding = 1;
};

struct PieceMove {
    Coordinates old_coordinates;
    Coordinates new_coordinates;
};

class MainScene : public Node2D {
    GDCLASS(MainScene, Node2D)

private:
    vector<Piece> pieces;
    Board board;
    vector<PieceMove> moves;

    bool is_searching = false;
    bool is_solved = false;
    PieceType piece_type = BFS;

    Label* time;

    Button* solve_button;
    Button* reset_button;
    Button* load_button;

    void _on_solve_button_pressed();
    void _on_reset_button_pressed();
    
    FileDialog* _load_folder_dialog;
    void _on_load_button_pressed();
    void _on_load_folder_selected(const String& path);
    void _on_load_dialog_canceled();

    bool load_input(String path, vector<Piece>& pieces, Board& board);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    MainScene();
    ~MainScene();

    void _process(double delta) override;

// --- Utils ---
private:
    void print_board();
    godot::String stringToGodotString(const std::string& stdString);
    std::string godotStringToString(const godot::String& godotString);
};