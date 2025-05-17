#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/csg_primitive3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include <fstream>
#include <vector>

#include "../utils/utils.hpp"

using namespace godot;
using namespace std;

class MainScene : public Node3D {
    GDCLASS(MainScene, Node3D)

private:
    vector<Piece> pieces;
    Board board;
    vector<PieceMove> moves;

    bool is_searching = false;
    bool is_solved = false;
    AlgoType algo_type = BFS;

    Label* time_label = nullptr;

    Button* solve_button = nullptr;
    Button* reset_button = nullptr;
    Button* load_button = nullptr;
    OptionButton* algo_button = nullptr;

    Ref<PackedScene> car2_template;
    Ref<PackedScene> car3_template;
    Ref<StandardMaterial3D> primary_piece_material;
    Ref<StandardMaterial3D> non_primary_piece_material;
    MeshInstance3D* floor = nullptr;
    std::vector<Node3D*> spawned_car_nodes;
    Vector3 floor_center_offset = Vector3(0.0f, 0.0f, 0.0f);

    void _on_solve_button_pressed();
    void _on_reset_button_pressed();
    void _on_algo_button_selected(int index);
    
    FileDialog* _load_file_dialog = nullptr;
    void _on_load_button_pressed();
    void _on_load_file_selected(const String& path);
    void _on_load_dialog_canceled();

    bool load_input(String path, vector<Piece>& pieces, Board& board);

    void _clear_all_cars();
    void _spawn_piece_as_car(const Piece& piece_data);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    MainScene();
    ~MainScene();

    void _process(double delta) override;
};