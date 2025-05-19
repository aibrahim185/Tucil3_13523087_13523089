#include "main_scene.hpp"

#include "../search/bfs/bfs.hpp"
#include "../search/ucs/ucs.hpp"
#include "../search/astar/astar.hpp"
#include "../search/ids/ids.hpp"

MainScene::MainScene() {}
MainScene::~MainScene() {}

void MainScene::_bind_methods() {
    ClassDB::bind_method(D_METHOD("_on_solve_button_pressed"), &MainScene::_on_solve_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_reset_button_pressed"), &MainScene::_on_reset_button_pressed);
    ClassDB::bind_method(D_METHOD("_on_algo_button_selected"), &MainScene::_on_algo_button_selected);
    ClassDB::bind_method(D_METHOD("_on_load_button_pressed"), &MainScene::_on_load_button_pressed);

    ClassDB::bind_method(D_METHOD("_on_load_file_selected", "path"), &MainScene::_on_load_file_selected);
    ClassDB::bind_method(D_METHOD("_on_load_dialog_canceled"), &MainScene::_on_load_dialog_canceled);

    ClassDB::bind_method(D_METHOD("_on_move_animation_finished"), &MainScene::_on_move_animation_finished);
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
            if (solve_button){
                solve_button->connect("pressed", Callable(this, "_on_solve_button_pressed"));
                solve_button->set_disabled(true);
            } 

            reset_button = get_node<Button>("UI/ResetButton");
            if (reset_button) {
                reset_button->connect("pressed", Callable(this, "_on_reset_button_pressed"));
                reset_button->set_disabled(true);
            } 

            load_button = get_node<Button>("UI/LoadButton");
            if (load_button) load_button->connect("pressed", Callable(this, "_on_load_button_pressed"));

            algo_button = get_node<OptionButton>("UI/AlgoButton");
            if (algo_button) {
                algo_button->connect("item_selected", Callable(this, "_on_algo_button_selected"));
                algo_button->add_item("Greedy Best First Search");
                algo_button->add_item("Uniform Cost Search");
                algo_button->add_item("A* Search");
                algo_button->add_item("Iterative Deepening Search");
                algo_button->select(0);
            } 

            time_label = get_node<Label>("UI/TimeLabel");
            if (time_label) time_label->set_text("Time: 0.0s");

            floor = get_node<MeshInstance3D>("StaticBody3D/Floor");
            if (floor) {
                for (int i = floor->get_child_count() - 1; i >= 0; i--) {
                    Node *child = floor->get_child(i);
                    if (child) {
                        child->queue_free();
                    }
                }
            }

            camera_node = get_node<Camera3D>("Camera3D");
            
            ResourceLoader* rl = ResourceLoader::get_singleton();
            car2_template = rl->load("res://scenes/car2.tscn");
            car3_template = rl->load("res://scenes/car3.tscn");
            primary_piece_material = rl->load("res://scenes/primary_material.tres");
            non_primary_piece_material = rl->load("res://scenes/non_primary_material.tres");

            break;
        }
    }
}

void MainScene::_input(const Ref<InputEvent>& event) {
    if (!camera_node) {
        return;
    }

    Ref<InputEventMouseButton> mb_event = event;
    if (mb_event.is_valid()) {
        if (mb_event->is_pressed()) {
            Vector3 current_position = camera_node->get_global_position();
            float current_distance_from_origin = current_position.length();

            if (mb_event->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_UP) {
                if (current_distance_from_origin > min_zoom_distance) {
                    camera_node->translate_object_local(Vector3(0, 0, -zoom_speed));
                }
                // UtilityFunctions::print("Zoom In");
            } else if (mb_event->get_button_index() == MouseButton::MOUSE_BUTTON_WHEEL_DOWN) {
                if (current_distance_from_origin < max_zoom_distance) {
                    camera_node->translate_object_local(Vector3(0, 0, zoom_speed));
                }
                // UtilityFunctions::print("Zoom Out");
            }
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
    Solution solution;

    switch (algo_type) {
        case BFS: {
            solution = bfs::search_bfs(board, pieces);
            is_searching = true;

            if (is_solved = solution.is_solved) {
                UtilityFunctions::print("BFS found a solution!");
            } else {
                UtilityFunctions::print("BFS could not find a solution.");
            }
            is_searching = false;
            break;
        }
        case UCS: {
            solution = ucs::search_ucs(board, pieces);
            is_searching = true;

            if (is_solved = solution.is_solved) {
                UtilityFunctions::print("UCS found a solution!");
            } else {
                UtilityFunctions::print("UCS could not find a solution.");
            }
            is_searching = false;
            break;
        }
        case ASTAR: {
            solution = astar::search_astar(board, pieces);
            is_searching = true;

            if (is_solved = solution.is_solved) {
                UtilityFunctions::print("A* found a solution!");
            } else {
                UtilityFunctions::print("A* could not find a solution.");
            }
            is_searching = false;
            break;
        }
        case IDS: {
            solution = ids::search_ids(board, pieces);
            is_searching = true;

            if (is_solved = solution.is_solved) {
                UtilityFunctions::print("IDS found a solution!");
            } else {
                UtilityFunctions::print("IDS could not find a solution with depth: ",  String::num_int64(ids::MAX_DEPTH_LIMIT));
            }
            is_searching = false;
            break;
        }
        default:
            UtilityFunctions::printerr("Unknown algorithm selected");
            break;
    }

    if (solution.is_solved) {
        is_solved = true;
        UtilityFunctions::print("Moves: ", solution.moves.size());
        UtilityFunctions::print("Time taken: ", solution.duration.count(), " ms");
        UtilityFunctions::print("Nodes visited: ", solution.node);

        time_label->set_text("Time: " + String::num(solution.duration.count()) + " ms");
        this->current_solution = solution;
        this->current_move_index = 0;
        this->is_animating_solution = true;
        if(solve_button) solve_button->set_disabled(true);
        if(reset_button) reset_button->set_disabled(true);
        if(load_button) load_button->set_disabled(true);
        if(algo_button) algo_button->set_disabled(true);

        _animate_next_move();
    }
}

void MainScene::_animate_next_move() {
    if (!is_animating_solution || current_move_index < 0 || current_move_index >= current_solution.moves.size()) {
        is_animating_solution = false;
        if(solve_button) solve_button->set_disabled(false);
        if(reset_button) reset_button->set_disabled(false);
        if(load_button) load_button->set_disabled(false);
        if(algo_button) algo_button->set_disabled(false);
        UtilityFunctions::print("Animation finished or cannot proceed.");
        return;
    }

    const PieceMove& move_step = current_solution.moves[current_move_index];

    Node3D* car_to_animate_node = nullptr;
    auto it_map = coord_to_car_node_map.find(move_step.old_coordinates);
    if (it_map != coord_to_car_node_map.end()) {
        car_to_animate_node = it_map->second;
    }

    if (!car_to_animate_node) {
        UtilityFunctions::printerr("Anim Error: Car node not found at old_coordinates (",
                                 move_step.old_coordinates.x, ",", move_step.old_coordinates.y, ")");
        current_move_index++;
        _animate_next_move();
        return;
    }

    const Piece* moved_piece_data = nullptr;
    for (const auto& p : this->pieces) {
        if (p.coordinates.x == move_step.old_coordinates.x && p.coordinates.y == move_step.old_coordinates.y) {
            moved_piece_data = &p;
            break;
        }
    }

    if (!moved_piece_data) {
        UtilityFunctions::printerr("Anim Error: Piece data not found in 'this->pieces' at old_coordinates (",
                                 move_step.old_coordinates.x, ",", move_step.old_coordinates.y, ")");
        current_move_index++;
        _animate_next_move();
        return;
    }

    Vector3 target_3d_pos = _get_3d_position_for_piece_coords(
        move_step.new_coordinates,
        moved_piece_data->size,
        moved_piece_data->is_vertical
    );

    Ref<Tween> tween = get_tree()->create_tween();
    tween->tween_property(car_to_animate_node, "position", target_3d_pos, ANIMATION_DURATION);
    tween->connect("finished", Callable(this, "_on_move_animation_finished"));
}

void MainScene::_on_move_animation_finished() {
    if (current_move_index < 0 || current_move_index >= current_solution.moves.size()) {
        is_animating_solution = false;
        if(solve_button) solve_button->set_disabled(false);
        if(reset_button) reset_button->set_disabled(false);
        if(load_button) load_button->set_disabled(false);
        if(algo_button) algo_button->set_disabled(false);
        return;
    }

    const PieceMove& completed_move = current_solution.moves[current_move_index];

    // 1. Update posisi di `this->pieces`
    bool piece_list_updated = false;
    for (auto& p : this->pieces) {
        if (p.coordinates.x == completed_move.old_coordinates.x && p.coordinates.y == completed_move.old_coordinates.y) {
            p.coordinates = completed_move.new_coordinates;
            piece_list_updated = true;
            break;
        }
    }
    if (!piece_list_updated) {
        UtilityFunctions::printerr("Anim Finished Error: Could not find piece in 'this->pieces' to update its logical coords from (", completed_move.old_coordinates.x, ",", completed_move.old_coordinates.y, ").");
    }


    // 2. Update `coord_to_car_node_map`
    Node3D* animated_node = nullptr;
    auto it_map_old = coord_to_car_node_map.find(completed_move.old_coordinates);
    if (it_map_old != coord_to_car_node_map.end()) {
        animated_node = it_map_old->second;
        coord_to_car_node_map.erase(it_map_old);
        if (animated_node) {
           coord_to_car_node_map[completed_move.new_coordinates] = animated_node;
        }
    } else {
        UtilityFunctions::printerr("Anim Finished Error: Could not find car in map at old_coordinates (", completed_move.old_coordinates.x, ",", completed_move.old_coordinates.y, ") to update map.");
    }

    // 3. Lanjutkan ke langkah berikutnya
    current_move_index++;
    if (current_move_index < current_solution.moves.size()) {
        _animate_next_move();
    } else {
        is_animating_solution = false;
        UtilityFunctions::print("Solution animation complete!");
        if(solve_button) solve_button->set_disabled(false);
        if(reset_button) reset_button->set_disabled(false);
        if(load_button) load_button->set_disabled(false);
        if(algo_button) algo_button->set_disabled(false);
    }
}

Vector3 MainScene::_get_3d_position_for_piece_coords(const Coordinates& piece_top_left_coords, int piece_size, bool is_vertical_piece) {
    int actual_col = piece_top_left_coords.x - board.piece_padding;
    int actual_row = piece_top_left_coords.y - board.piece_padding;

    float target_local_x, target_local_z;
    float car_center_y_on_floor = 0.5f; 

    if (is_vertical_piece) {
        target_local_x = static_cast<float>(actual_col) - static_cast<float>(board.cols) / 2.0f + 0.5f;
        if (piece_size >= 2) {
             target_local_z = static_cast<float>(actual_row) - (static_cast<float>(board.rows) - (static_cast<float>(piece_size - 2.0f))) / 2.0f + 1.0f;
        }
    } else {
        if (piece_size >= 2) {
            target_local_x = static_cast<float>(actual_col) - (static_cast<float>(board.cols) - (static_cast<float>(piece_size - 2.0f))) / 2.0f + 1.0f;
        }
        target_local_z = static_cast<float>(actual_row) - static_cast<float>(board.rows) / 2.0f + 0.5f;
    }
    return Vector3(target_local_x, car_center_y_on_floor, target_local_z);
}

void MainScene::_on_reset_button_pressed() {
    UtilityFunctions::print("Reset button pressed!");
    _clear_all_cars();
    time_label->set_text("Time: 0.0s");
    current_solution.moves.clear();
    current_move_index = -1;
    is_animating_solution = false;
    is_searching = false;
    is_solved = false;
    if(solve_button) solve_button->set_disabled(true);
    if(reset_button) reset_button->set_disabled(true);
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
        case IDS:
            UtilityFunctions::print("IDS selected");
            algo_type = IDS;
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

        solve_button->set_disabled(false);
        reset_button->set_disabled(false);
        
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

    vector<string> potential_lines;
    string s_line;
    for (int i = 0; i < board.rows + 2; ++i) {
        if (std::getline(file, s_line)) {
            potential_lines.push_back(s_line);
        } else {
            break;
        }
    }
    file.close();
    
    // 3. Baca konfigurasi papan
    pieces.clear();
    int padded_height = board.rows + 2 * board.piece_padding;
    int padded_width = board.cols + 2 * board.piece_padding;
    board.grid.assign(padded_height, vector<char>(padded_width, ' '));

    bool local_is_keluar_found = false;
    bool local_is_primary_found = false;
    bool k_found_on_pre_grid_line = false;

    // 4. Pass 1: Cari 'K'
    if (!potential_lines.empty()) {
        const string& first_line_in_buffer = potential_lines[0];
        for (int col = 0; col < first_line_in_buffer.length(); ++col) {
            if (first_line_in_buffer[col] == 'K') {
                board.exit_coordinates.x = col + board.piece_padding;
                board.exit_coordinates.y = -1 + board.piece_padding;
                local_is_keluar_found = true;
                k_found_on_pre_grid_line = true;
                if (board.exit_coordinates.y >= 0 && board.exit_coordinates.y < padded_height &&
                    board.exit_coordinates.x >= 0 && board.exit_coordinates.x < padded_width) {
                    board.grid[board.exit_coordinates.y][board.exit_coordinates.x] = 'K';
                }
                break;
            }
        }
    }

    int grid_lines_start_idx_in_buffer = k_found_on_pre_grid_line ? 1 : 0;
    int actual_grid_lines_processed = 0;

    // 5. Pass 2: Proses Piece dari baris grid utama (dan cari K jika belum ketemu)
    for (int i = 0; i < board.rows; ++i) {
        int current_line_idx_in_buffer = grid_lines_start_idx_in_buffer + i;

        if (current_line_idx_in_buffer >= potential_lines.size()) {
            break;
        }
        actual_grid_lines_processed++;
        const string& current_grid_line_content = potential_lines[current_line_idx_in_buffer];

        for (int col = 0; col < current_grid_line_content.length(); ++col) {
            char c = current_grid_line_content[col];
            if (c == ' ' || c == '\n' || c == '\r') {
                continue;
            }

            int char_padded_x = col + board.piece_padding;
            int char_padded_y = i + board.piece_padding;

            if (char_padded_y >= 0 && char_padded_y < padded_height &&
                char_padded_x >= 0 && char_padded_x < padded_width) {
                board.grid[char_padded_y][char_padded_x] = c;
            }

            if (c == 'K') {
                if (!local_is_keluar_found) {
                    board.exit_coordinates.x = char_padded_x;
                    board.exit_coordinates.y = char_padded_y;
                    local_is_keluar_found = true;
                }
                continue;
            }

            if (c == '.') {
                continue;
            }

            if (col < board.cols) {
                bool piece_found = false;
                for (Piece& piece : pieces) {
                    if (piece.id == c) {
                        piece.size++;
                        if (piece.coordinates.x == char_padded_x) {
                            piece.is_vertical = true;
                        } else if (piece.coordinates.y == char_padded_y) {
                            piece.is_vertical = false;
                        }
                        piece_found = true;
                        break;
                    }
                }

                if (!piece_found) {
                    Piece new_piece;
                    new_piece.id = c;
                    new_piece.size = 1;
                    new_piece.is_vertical = false;
                    new_piece.is_primary = (c == 'P');
                    new_piece.coordinates.x = char_padded_x;
                    new_piece.coordinates.y = char_padded_y;

                    if (c == 'P') {
                        local_is_primary_found = true;
                    }
                    pieces.push_back(new_piece);
                }
            }
        }
    }

    // 6. Pass 3: Proses Piece dari baris pasca-grid (dan cari K jika belum ketemu)
    int line_after_grid_idx_in_buffer = grid_lines_start_idx_in_buffer + actual_grid_lines_processed;
    if (!local_is_keluar_found && line_after_grid_idx_in_buffer < potential_lines.size()) {
        const string& after_grid_line_content = potential_lines[line_after_grid_idx_in_buffer];
        for (int col = 0; col < after_grid_line_content.length(); ++col) {
            if (after_grid_line_content[col] == 'K') {
                board.exit_coordinates.x = col + board.piece_padding;
                board.exit_coordinates.y = board.rows + board.piece_padding;
                local_is_keluar_found = true;
                if (board.exit_coordinates.y >= 0 && board.exit_coordinates.y < padded_height &&
                    board.exit_coordinates.x >= 0 && board.exit_coordinates.x < padded_width) {
                    board.grid[board.exit_coordinates.y][board.exit_coordinates.x] = 'K';
                }
                break;
            }
        }
    }

    if (pieces.size() != board.other_pieces_count + 1) {
        UtilityFunctions::printerr("Error: Pieces count mismatch. Expected: ", board.other_pieces_count + 1, ", Found: ", pieces.size());
        return false;
    }

    if (!local_is_keluar_found) {
        UtilityFunctions::printerr("Error: K not found in the grid.");
        return false;
    }

    Ref<ShaderMaterial> shader_material_ref = floor->get_material_override();
    if (shader_material_ref.is_valid()) {
        shader_material_ref->set_shader_parameter("mesh_actual_size", Vector2(static_cast<float>(board.cols), static_cast<float>(board.rows)));
    }

    if (board.rows > 0 && board.cols > 0) {
        CollisionShape3D* collision_shape_node = get_node<CollisionShape3D>("StaticBody3D/CollisionShape3D");

        if (floor && collision_shape_node) {
            // 1. Atur ukuran visual PlaneMesh
            Ref<PlaneMesh> plane_mesh = floor->get_mesh();
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
                float half_width = static_cast<float>(board.cols - 0.1f) / 2.0f;
                float half_depth = static_cast<float>(board.rows - 0.1f) / 2.0f;

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
            if (!floor) UtilityFunctions::printerr("Failed to find Floor node at StaticBody3D/Floor.");
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
    coord_to_car_node_map.clear();
}

void MainScene::_spawn_piece_as_car(const Piece& piece_data) {
    if (!floor) {
        UtilityFunctions::printerr("cars_container is null. Cannot spawn car.");
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

    floor->add_child(car_node_3d);
    coord_to_car_node_map[piece_data.coordinates] = car_node_3d;
    spawned_car_nodes.push_back(car_node_3d);

    int actual_col = piece_data.coordinates.x - board.piece_padding;
    int actual_row = piece_data.coordinates.y - board.piece_padding;

    float target_local_x, target_local_z;
    float car_center_y_on_floor = 7.0f; 

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
