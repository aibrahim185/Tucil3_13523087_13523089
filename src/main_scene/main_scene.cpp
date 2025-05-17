#include "main_scene.hpp"

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

            solve_button = get_node<Button>("SolveButton");
            solve_button->connect("pressed", Callable(this, "_on_solve_button_pressed"));

            reset_button = get_node<Button>("ResetButton");
            reset_button->connect("pressed", Callable(this, "_on_reset_button_pressed"));

            load_button = get_node<Button>("LoadButton");
            load_button->connect("pressed", Callable(this, "_on_load_button_pressed"));

            algo_button = get_node<OptionButton>("AlgoButton");
            algo_button->connect("item_selected", Callable(this, "_on_algo_button_selected"));

            time = get_node<Label>("TimeLabel");
            time->set_text("Time: 0.0s");

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

    return true;
}
