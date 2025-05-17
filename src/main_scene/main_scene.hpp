#pragma once

#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/file_dialog.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include <fstream>
#include <vector>

#include "../utils/utils.hpp"

using namespace godot;
using namespace std;

class MainScene : public Control {
    GDCLASS(MainScene, Control)

private:
    vector<Piece> pieces;
    Board board;
    vector<PieceMove> moves;

    bool is_searching = false;
    bool is_solved = false;
    AlgoType algo_type = BFS;

    Label* time = nullptr;

    Button* solve_button = nullptr;
    Button* reset_button = nullptr;
    Button* load_button = nullptr;
    OptionButton* algo_button = nullptr;

    void _on_solve_button_pressed();
    void _on_reset_button_pressed();
    void _on_algo_button_selected(int index);
    
    FileDialog* _load_file_dialog = nullptr;
    void _on_load_button_pressed();
    void _on_load_file_selected(const String& path);
    void _on_load_dialog_canceled();

    bool load_input(String path, vector<Piece>& pieces, Board& board);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    MainScene();
    ~MainScene();

    void _process(double delta) override;
};