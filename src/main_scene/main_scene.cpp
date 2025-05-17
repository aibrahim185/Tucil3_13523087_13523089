#include "main_scene.hpp"

#include <godot_cpp/classes/rigid_body3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/csg_primitive3d.hpp>
#include <godot_cpp/classes/plane_mesh.hpp>
#include <godot_cpp/classes/collision_shape3d.hpp>
#include <godot_cpp/classes/concave_polygon_shape3d.hpp>
#include <godot_cpp/classes/engine.hpp>
#include "../search/bfs/bfs.hpp"

MainScene::MainScene() {}
MainScene::~MainScene() {}

void MainScene::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_solve_button_pressed"), &MainScene::_on_solve_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_reset_button_pressed"), &MainScene::_on_reset_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_algo_button_selected"), &MainScene::_on_algo_button_selected);
    ClassDB::bind_method(D_METHOD("_on_load_button_pressed"), &MainScene::_on_load_button_pressed);

    ClassDB::bind_method(D_METHOD("_on_load_file_selected", "path"), &MainScene::_on_load_file_selected);
    ClassDB::bind_method(D_METHOD("_on_load_dialog_canceled"), &MainScene::_on_load_dialog_canceled);
}

void MainScene::_notification(int p_what) {
    switch (p_what) {
        case NOTIFICATION_READY: {
            UtilityFunctions::print("MainScene node created!");
            if (Engine::get_singleton()->is_editor_hint()) {
                UtilityFunctions::print("MainScene is in editor mode.");
                return;
            }

            solve_button = get_node<Button>("UI/SolveButton");
            if (solve_button) solve_button->connect("pressed", Callable(this, "_on_solve_button_pressed"));

            reset_button = get_node<Button>("UI/ResetButton");
            if (reset_button) reset_button->connect("pressed", Callable(this, "_on_reset_button_pressed"));

            load_button = get_node<Button>("UI/LoadButton");
            if (load_button) load_button->connect("pressed", Callable(this, "_on_load_button_pressed"));

            algo_button = get_node<OptionButton>("UI/AlgoButton");
            if (algo_button) algo_button->connect("item_selected", Callable(this, "_on_algo_button_selected"));

            time_label = get_node<Label>("UI/TimeLabel");
            if (time_label) time_label->set_text("Time: 0.0s");

            cars_container = get_node<Node>("StaticBody3D/Floor");
            if (!cars_container) {
                UtilityFunctions::printerr("ERROR: Node 'StaticBody3D/Floor' tidak ditemukan. Mobil tidak akan di-spawn.");
            }

            RigidBody3D* car2 = get_node<RigidBody3D>("StaticBody3D/Floor/Car2");
            if (car2) car2->queue_free();
            RigidBody3D* car3 = get_node<RigidBody3D>("StaticBody3D/Floor/Car3");
            if (car3) car3->queue_free();
            
            ResourceLoader* rl = ResourceLoader::get_singleton();
            car2_template = rl->load("res://scenes/car2.tscn");
            car3_template = rl->load("res://scenes/car3.tscn");
            primary_piece_material = rl->load("res://scenes/primary_material.tres");
            non_primary_piece_material = rl->load("res://scenes/non_primary_material.tres");

            break;
        }
    }
}

void MainScene::_process(double delta) {
    if (is_searching) {
        // time->set_text("Time: " + String::num(get_process_time()));
    }
}

void MainScene::_on_solve_button_pressed() {
    UtilityFunctions::print("Solve button pressed!");

    switch (algo_type) {
        case BFS: {
            Solution solution = bfs::search_bfs(board, pieces);
            is_searching = true;

            if (is_solved = solution.is_solved) {
                UtilityFunctions::print("BFS found a solution!");
                UtilityFunctions::print("Moves: ", solution.moves.size());
            } else {
                UtilityFunctions::print("BFS could not find a solution.");
            }
            UtilityFunctions::print("Time taken: ", solution.duration.count(), " ms");
            UtilityFunctions::print("Nodes visited: ", solution.node);
            is_searching = false;
            break;
        }
        case UCS:
            UtilityFunctions::print("UCS selected");
            break;
        case ASTAR:
            UtilityFunctions::print("A* selected");
            break;
        default:
            UtilityFunctions::printerr("Unknown algorithm selected");
            break;
    }
}

void MainScene::_on_reset_button_pressed() {
    UtilityFunctions::print("Reset button pressed!");
}

void MainScene::_on_algo_button_selected(int index) {
    UtilityFunctions::print("Algorithm selected: ", algo_button->get_item_text(index));
    switch (index) {
        case BFS:
            UtilityFunctions::print("BFS selected");
            algo_type = BFS;
            break;
        case UCS:
            UtilityFunctions::print("UCS selected");
            algo_type = UCS;
            break;
        case ASTAR:
            UtilityFunctions::print("A* selected");
            algo_type = ASTAR;
            break;
        default:
            UtilityFunctions::printerr("Unknown algorithm selected");
            break;
    }
}

void MainScene::_on_load_button_pressed() {
    UtilityFunctions::print("Load button pressed!");

    if (!_load_file_dialog) {
		_load_file_dialog = memnew(FileDialog);
		_load_file_dialog->set_name("LoadFileDialog");
		add_child(_load_file_dialog);

		_load_file_dialog->connect("file_selected", Callable(this, "_on_load_file_selected"));
		_load_file_dialog->connect("canceled", Callable(this, "_on_load_dialog_canceled"));

		UtilityFunctions::print("Created and added LoadfileDialog node.");
	}

	if (_load_file_dialog) {
		_load_file_dialog->set_access(FileDialog::ACCESS_FILESYSTEM);
		_load_file_dialog->set_file_mode(FileDialog::FileMode::FILE_MODE_OPEN_FILE);
		_load_file_dialog->set_use_native_dialog(true);
        _load_file_dialog->popup_centered();
		_load_file_dialog->set_title("Select File to Load");

		UtilityFunctions::print("LoadfileDialog popped up.");
	}
}

void MainScene::_on_load_file_selected(const String& path) {
    UtilityFunctions::print("File selected: ", path);
    if (load_input(path, pieces, board)) {
        Utils::print_board(board, pieces);

        _clear_all_cars();
        for (const auto& piece_data : this->pieces) {
            _spawn_piece_as_car(piece_data);
        }
    } else {
        UtilityFunctions::printerr("Failed to load input file.");
    }
}

void MainScene::_on_load_dialog_canceled() {
    UtilityFunctions::print("Load dialog canceled.");
}

bool MainScene::load_input(String path, vector<Piece>& pieces, Board& board) {
    UtilityFunctions::print("Loading input from file: ", path);

    std::ifstream file(Utils::godotStringToString(path));
    if (!file.is_open()) {
        UtilityFunctions::printerr("Error: Tidak dapat membuka file: ", path);
        return false;
    }

    // 1. Baca dimensi papan (A B)
    if (!(file >> board.rows >> board.cols)) {
        UtilityFunctions::printerr("Error: Gagal membaca dimensi papan dari file.");
        file.close();
        return false;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 2. Baca banyak piece BUKAN primary piece (N)
    if (!(file >> board.other_pieces_count)) {
        UtilityFunctions::printerr("Error: Gagal membaca jumlah piece non-primary (N) dari file.");
        file.close();
        return false;
    }
    file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // 3. Baca konfigurasi papan
    board.grid.resize(board.rows + 2 * board.piece_padding, vector<char>(board.cols + 2 * board.piece_padding, ' '));
    bool is_keluar_found = false;
    bool is_primary_found = false;
    pieces.clear();
    std::string line;
    for (int i = 0; i < board.rows; i++) {
        if (!std::getline(file, line)) {
            UtilityFunctions::printerr("Error: Gagal membaca baris ", i, " dari file.");
            file.close();
            return false;
        }

        for (int col_idx = 0; col_idx < line.length(); ++col_idx) {
            char c = line[col_idx];

            if (c == ' ' || c == '\n' || c == '\r') {
                continue;
            } 
            
            int x = col_idx + board.piece_padding;
            int y = i + board.piece_padding;
            board.grid[y][x] = c;
            if (c == '.') continue;

            bool is_found = false;
            for (Piece& piece : pieces) {
                if (piece.id == c) {
                    piece.size++;
                    if (piece.coordinates.x == x) 
                        piece.is_vertical = true;
                    is_found = true;
                    break;
                }
            }

            if (!is_found) {
                Piece piece;
                piece.id = c;
                piece.size = 1;
                piece.is_vertical = false;
                piece.is_primary = false;
                piece.coordinates.x = x;
                piece.coordinates.y = y;

                if (c == 'K') {
                    is_keluar_found = true;
                    board.exit_coordinates.x = x;
                    board.exit_coordinates.y = y;
                    continue;
                } else if (c == 'P') {
                    is_primary_found = true;
                    piece.is_primary = true;
                }

                pieces.push_back(piece);
            }
        }
    }

    file.close();

    if (board.rows > 0 && board.cols > 0) {
        MeshInstance3D* floor_node = get_node<MeshInstance3D>("StaticBody3D/Floor");
        CollisionShape3D* collision_shape_node = get_node<CollisionShape3D>("StaticBody3D/CollisionShape3D");

        if (floor_node && collision_shape_node) {
            // 1. Atur ukuran visual PlaneMesh
            Ref<PlaneMesh> plane_mesh = floor_node->get_mesh();
            if (plane_mesh.is_valid()) {
                plane_mesh->set_size(Vector2(static_cast<float>(board.cols), static_cast<float>(board.rows)));
                UtilityFunctions::print("Floor mesh size set to: ", Vector2(static_cast<float>(board.cols), static_cast<float>(board.rows)));
            } else {
                UtilityFunctions::printerr("Failed to get PlaneMesh for Floor.");
            }

            // 2. Atur ukuran fisik CollisionShape3D
            Ref<ConcavePolygonShape3D> collision_shape = collision_shape_node->get_shape();
            if (collision_shape.is_valid()) {
                PackedVector3Array vertices;
                float half_width = static_cast<float>(board.cols) / 2.0f;
                float half_depth = static_cast<float>(board.rows) / 2.0f;

                vertices.push_back(Vector3(half_width, 0.0f, half_depth));
                vertices.push_back(Vector3(-half_width, 0.0f, half_depth));
                vertices.push_back(Vector3(half_width, 0.0f, -half_depth));
                
                vertices.push_back(Vector3(-half_width, 0.0f, half_depth));
                vertices.push_back(Vector3(-half_width, 0.0f, -half_depth));
                vertices.push_back(Vector3(half_width, 0.0f, -half_depth));
                
                collision_shape->set_faces(vertices);
                UtilityFunctions::print("Floor collision shape updated for size: ", Vector2(static_cast<float>(board.cols), static_cast<float>(board.rows)));
            } else {
                UtilityFunctions::printerr("Failed to get ConcavePolygonShape3D for Floor collision.");
            }
        } else {
            if (!floor_node) UtilityFunctions::printerr("Failed to find Floor node at StaticBody3D/Floor.");
            if (!collision_shape_node) UtilityFunctions::printerr("Failed to find CollisionShape3D node at StaticBody3D/CollisionShape3D.");
        }
    } else {
        UtilityFunctions::printerr("Invalid board dimensions for setting floor size: rows=", board.rows, ", cols=", board.cols);
    }

    return true;
}

void MainScene::_clear_all_cars() {
    for (Node3D* car_node : spawned_car_nodes) {
        if (car_node && car_node->get_parent()) {
            car_node->queue_free();
        }
    }
    spawned_car_nodes.clear();
}

void MainScene::_spawn_piece_as_car(const Piece& piece_data) {
    if (!cars_container) {
        UtilityFunctions::printerr("cars_container tidak valid, tidak bisa spawn mobil.");
        return;
    }

    Ref<PackedScene> selected_car_template;
    if (piece_data.size == 2) {
        selected_car_template = car2_template;
    } else if (piece_data.size == 3) {
        selected_car_template = car3_template;
    } else {
        return; 
    }

    if (!selected_car_template.is_valid()) {
        return;
    }

    Node* instance = selected_car_template->instantiate();
    Node3D* car_node_3d = Object::cast_to<Node3D>(instance);

    if (!car_node_3d) {
        if (instance) instance->queue_free();
        return;
    }

    cars_container->add_child(car_node_3d);
    spawned_car_nodes.push_back(car_node_3d);

    int actual_col = piece_data.coordinates.x - board.piece_padding;
    int actual_row = piece_data.coordinates.y - board.piece_padding;

    float target_local_x, target_local_z;
    float car_center_y_on_floor = 0.5f; 

    if (piece_data.is_vertical) {
        target_local_x = static_cast<float>(actual_col) - static_cast<float>(board.cols) / 2.0f + 0.5f;
        target_local_z = static_cast<float>(actual_row) - (static_cast<float>(board.rows) - (static_cast<float>(piece_data.size - 2.0f))) / 2.0f + 1.0f;
        car_node_3d->set_rotation(Vector3(0, Math::deg_to_rad(0.0f), 0));
    } else {
        target_local_x = static_cast<float>(actual_col) - (static_cast<float>(board.cols) - (static_cast<float>(piece_data.size - 2.0f))) / 2.0f + 1.0f;
        target_local_z = static_cast<float>(actual_row) - static_cast<float>(board.rows) / 2.0f + 0.5f;
    }
    car_node_3d->set_position(Vector3(target_local_x, car_center_y_on_floor, target_local_z));

    CSGPrimitive3D* csg_body = car_node_3d->get_node<CSGPrimitive3D>("Body");
    if (piece_data.is_primary && primary_piece_material.is_valid() && csg_body) {
        csg_body->set_material_override(primary_piece_material);
    } else {
        csg_body->set_material_override(non_primary_piece_material);
    }

    UtilityFunctions::print("Spawned car: ", String::chr(piece_data.id), " at (", actual_col, ", ", actual_row, ") size: ", piece_data.size, " is_vertical: ", piece_data.is_vertical, " is_primary: ", piece_data.is_primary);
}
