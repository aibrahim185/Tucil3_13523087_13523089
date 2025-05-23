#pragma once

#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/camera3d.hpp>
#include <godot_cpp/classes/world3d.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/physics_direct_space_state3d.hpp>
#include <godot_cpp/classes/physics_ray_query_parameters3d.hpp>
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/option_button.hpp>
#include <godot_cpp/classes/label.hpp>
#include <godot_cpp/classes/file_dialog.hpp>
#include <godot_cpp/classes/packed_scene.hpp>
#include <godot_cpp/classes/standard_material3d.hpp>
#include <godot_cpp/classes/shader_material.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/csg_primitive3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/scene_tree_timer.hpp>
#include <godot_cpp/classes/tween.hpp>
#include <godot_cpp/classes/property_tweener.hpp>
#include <godot_cpp/classes/engine.hpp>

#include <godot_cpp/variant/utility_functions.hpp>

#include <fstream>
#include <vector>
#include <map>
#include <cmath> 

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

    Label* node_label = nullptr;
    Label* time_label = nullptr;
    Label* move_label = nullptr;

    Button* solve_button = nullptr;
    Button* reset_button = nullptr;
    Button* load_button = nullptr;
    OptionButton* algo_button = nullptr;

    Ref<PackedScene> car2_template;
    Ref<PackedScene> car3_template;
    Ref<PackedScene> gate_template;
    Ref<StandardMaterial3D> primary_piece_material;
    Ref<StandardMaterial3D> non_primary_piece_material;
    MeshInstance3D* floor = nullptr;
    vector<Node3D*> spawned_car_nodes;
    Vector3 floor_center_offset = Vector3(0.0f, 0.0f, 0.0f);

    map<Coordinates, Node3D*> coord_to_car_node_map;

    Solution current_solution;
    int current_move_index = -1;
    bool is_animating_solution = false;
    const float ANIMATION_DURATION = 0.25f;

    Camera3D* camera_node;
    float zoom_speed = 0.5f;
    float min_zoom_distance = 2.0f;
    float max_zoom_distance = 20.0f;

    int notification_count = 0;
	VBoxContainer* notification_container = nullptr;
	Label* notification_label_template = nullptr;

    Node3D* dragged_car_node = nullptr;
    Piece* dragged_piece_data = nullptr;
    Vector3 drag_start_mouse_world_pos;
    Vector3 drag_start_car_3d_pos;
    Coordinates drag_start_piece_coords;
    bool is_dragging_piece = false;
    int current_manual_moves = 0;

    void _animate_next_move();
    void _on_move_animation_finished();
    Vector3 _get_3d_position_for_piece_coords(const Coordinates& piece_top_left_coords, int piece_size, bool is_vertical_piece);

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

    void add_notification(const String& p_message);

    Coordinates _get_grid_coords_from_3d_position(const Vector3& pos_3d, int piece_size, bool is_vertical_piece);
    bool _is_move_valid(const Coordinates& from_coords, const Coordinates& to_coords, char piece_id_moving, bool is_vertical);

protected:
    static void _bind_methods();
    void _notification(int p_what);

public:
    MainScene();
    ~MainScene();

    void _process(double delta) override;
    void _input(const Ref<InputEvent>& event) override;
};