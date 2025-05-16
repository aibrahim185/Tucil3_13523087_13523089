extends Node2D

var padding = 4
var cell_size = 64
var start_pos = Vector2(256, 128)

#const GRID_COLUMNS = 6
#const GRID_ROWS = 6

const EXIT_ROW = 2

# ... (konstanta lama Anda) ...
const LOGICAL_GRID_COLUMNS = 6 # Nama baru untuk kejelasan
const LOGICAL_GRID_ROWS = 6	# Nama baru untuk kejelasan

const PADDING_SIZE = 1 # Tebal dinding/padding
const PADDED_GRID_COLUMNS = LOGICAL_GRID_COLUMNS + 2 * PADDING_SIZE
const PADDED_GRID_ROWS = LOGICAL_GRID_ROWS + 2 * PADDING_SIZE

const WALL_IDENTIFIER = "WALL"
const EXIT_IDENTIFIER = "EXIT" # Opsional, untuk kejelasan sel keluar

var cars_config = [
	{
		"id": "target_R", "pos": Vector2i(1, EXIT_ROW), "length": 2,
		"is_vertical": false, "is_target": true, "color": Color.RED
	},
	{
		"id": "blue_A", "pos": Vector2i(0, 0), "length": 2,
		"is_vertical": false, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_B", "pos": Vector2i(3, 0), "length": 3,
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_C", "pos": Vector2i(0, 1), "length": 3, # Sebelumnya 0,2
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_D", "pos": Vector2i(4, 1), "length": 3,
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_F", "pos": Vector2i(5, 0), "length": 2,
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_G", "pos": Vector2i(5, 2), "length": 2,
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_H", "pos": Vector2i(1, 3), "length": 3,
		"is_vertical": false, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_I", "pos": Vector2i(0, 4), "length": 2,
		"is_vertical": true, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_J", "pos": Vector2i(4, 4), "length": 2,
		"is_vertical": false, "is_target": false, "color": Color.BLUE
	},
	{
		"id": "blue_K", "pos": Vector2i(4, 5), "length": 2,
		"is_vertical": false, "is_target": false, "color": Color.BLUE
	},
]


var car_nodes_map = {}
var grid_occupancy = []

var selected_car_id = null
var drag_start_grid_pos = Vector2i.ZERO
var drag_start_mouse_offset = Vector2.ZERO

# --- Variabel untuk Solver BFS ---
var solver_thread: Thread = null
signal solver_finished(solution_path)


func _ready() -> void:
	_initialize_grid_occupancy()
	_create_car_nodes()
	# Tambahkan tombol untuk memicu solver jika belum ada, misalnya:
	var solve_button = Button.new()
	solve_button.text = "Solve BFS"
	solve_button.position = Vector2(10, 10)
	solve_button.pressed.connect(_on_SolveButton_pressed)
	add_child(solve_button)


func _initialize_grid_occupancy() -> void:
	grid_occupancy.resize(PADDED_GRID_ROWS)
	for r_padded in range(PADDED_GRID_ROWS):
		grid_occupancy[r_padded] = []
		grid_occupancy[r_padded].resize(PADDED_GRID_COLUMNS)
		for c_padded in range(PADDED_GRID_COLUMNS):
			# Cek apakah ini sel di tepi (dinding)
			if r_padded < PADDING_SIZE or r_padded >= PADDED_GRID_ROWS - PADDING_SIZE or \
				c_padded < PADDING_SIZE or c_padded >= PADDED_GRID_COLUMNS - PADDING_SIZE:
				grid_occupancy[r_padded][c_padded] = WALL_IDENTIFIER
			else:
				grid_occupancy[r_padded][c_padded] = null # Area bermain

	# Buat lubang keluar (asumsi keluar ke kanan)
	# EXIT_ROW adalah logis, jadi tambahkan PADDING_SIZE untuk mendapatkan indeks baris di grid padded
	var exit_padded_row = EXIT_ROW + PADDING_SIZE
	var exit_padded_col = PADDED_GRID_COLUMNS - PADDING_SIZE # Ini adalah kolom *setelah* area bermain
															# atau kolom dinding paling kanan.
															# Jika padding = 1, ini adalah PADDED_GRID_COLUMNS - 1

	if exit_padded_row >= PADDING_SIZE and exit_padded_row < PADDED_GRID_ROWS - PADDING_SIZE: # Pastikan EXIT_ROW valid
		# Sel keluar berada di kolom dinding paling kanan
		grid_occupancy[exit_padded_row][PADDED_GRID_COLUMNS - PADDING_SIZE -1 ] = EXIT_IDENTIFIER # Diubah agar EXIT berada di kolom terakhir grid padded
		# Untuk Rush Hour, mobil target keluar *melewati* batas. Jadi, sel di dinding itu sendiri
		# yang menjadi pintu keluar.
		# Misal, grid logis 0-5. Mobil keluar jika mencapai kolom logis 6.
		# Kolom logis 5 ada di kolom padded 5 + PADDING_SIZE.
		# Kolom logis 6 (pintu keluar) ada di kolom padded 6 + PADDING_SIZE.
		# grid_occupancy[exit_padded_row][LOGICAL_GRID_COLUMNS + PADDING_SIZE] = EXIT_IDENTIFIER
		# Ini adalah sel di dinding kanan yang kita ubah jadi EXIT
		grid_occupancy[exit_padded_row][PADDED_GRID_COLUMNS - 1] = EXIT_IDENTIFIER


	# Debug print untuk melihat grid awal (opsional)
	# for r in range(PADDED_GRID_ROWS):
	#     print(grid_occupancy[r])
func _create_car_nodes() -> void:
	for car_data_original in cars_config:
		var car_data = car_data_original.duplicate(true)
		var car_node = ColorRect.new()
		car_node.color = car_data.color
		car_node.mouse_filter = Control.MOUSE_FILTER_STOP
		car_node.set_meta("car_id", car_data.id)
		car_node.gui_input.connect(_on_car_gui_input.bind(car_node))
		car_data["node"] = car_node
		car_nodes_map[car_data.id] = car_data
		add_child(car_node)
		_update_car_visuals_and_occupancy(car_data.id, car_data.pos)

func _update_car_visuals_and_occupancy(car_id: String, new_logical_grid_pos: Vector2i, old_logical_grid_pos_optional: Variant = null) -> void:
	var car_data = car_nodes_map[car_id]
	var car_node = car_data.node # ini adalah ColorRect

	var old_pos_to_clear = old_logical_grid_pos_optional if old_logical_grid_pos_optional != null else car_data.pos
	# Panggil dengan posisi logis
	_set_car_occupancy(car_id, old_pos_to_clear, car_data.length, car_data.is_vertical, null)

	car_data.pos = new_logical_grid_pos # Simpan posisi logis baru

	# Atur ukuran visual (tidak berubah)
	if car_data.is_vertical:
		car_node.size = Vector2(1 * cell_size - 2 * padding, car_data.length * cell_size - 2 * padding)
	else:
		car_node.size = Vector2(car_data.length * cell_size - 2 * padding, 1 * cell_size - 2 * padding)

	# Atur posisi visual berdasarkan posisi logis (tidak berubah)
	var visual_x = start_pos.x + new_logical_grid_pos.x * cell_size + padding
	var visual_y = start_pos.y + new_logical_grid_pos.y * cell_size + padding
	car_node.position = Vector2(visual_x, visual_y)

	# Panggil dengan posisi logis baru
	_set_car_occupancy(car_id, new_logical_grid_pos, car_data.length, car_data.is_vertical, car_id)

func _set_car_occupancy(car_id_to_set: String, logical_grid_pos: Vector2i, length: int, is_vertical: bool, value_to_set: Variant) -> void:
	for i in range(length):
		var lr = logical_grid_pos.y # logical row
		var lc = logical_grid_pos.x # logical col
		if is_vertical: lr += i
		else: lc += i

		# Mobil harusnya selalu berada dalam batas logis jika _is_move_valid sudah benar
		# atau ini adalah penempatan awal.
		# Kita hanya peduli dengan menempatkan/menghapus dari area yang bukan WALL permanen.

		var padded_pos = logical_to_padded_pos(Vector2i(lc, lr))
		var pr = padded_pos.y
		var pc = padded_pos.x

		# Pastikan kita berada dalam batas array grid_occupancy (yang sudah dipadding)
		if pr >= 0 and pr < PADDED_GRID_ROWS and pc >= 0 and pc < PADDED_GRID_COLUMNS:
			# Hanya set jika bukan dinding permanen, ATAU jika kita menghapus (value_to_set == null),
			# ATAU jika ini adalah sel EXIT (mobil bisa ada di sel EXIT).
			if grid_occupancy[pr][pc] != WALL_IDENTIFIER or \
			   grid_occupancy[pr][pc] == EXIT_IDENTIFIER or \
			   value_to_set == null:
				grid_occupancy[pr][pc] = value_to_set
			elif value_to_set != null: # Mencoba menempatkan sesuatu di atas WALL_IDENTIFIER
				printerr("Error: Mobil '", value_to_set, "' mencoba menempati WALL di padded (", pc, ",", pr, ") dari logis (", lc, ",", lr, ")")
		# else: (Tidak perlu error di sini jika di luar padded grid, karena validasi harusnya sudah menangani)
			# printerr("Error: Koordinat padded (", pc, ",", pr, ") di luar batas saat _set_car_occupancy untuk mobil '", value_to_set, "'")
			
			
func logical_to_padded_pos(logical_pos: Vector2i) -> Vector2i:
	return logical_pos + Vector2i(PADDING_SIZE, PADDING_SIZE)

func padded_to_logical_pos(padded_pos: Vector2i) -> Vector2i:
	return padded_pos - Vector2i(PADDING_SIZE, PADDING_SIZE)
	
func _on_car_gui_input(event: InputEvent, car_node: ColorRect) -> void:
	var car_id = car_node.get_meta("car_id")
	var car_data = car_nodes_map[car_id]
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		if event.pressed:
			selected_car_id = car_id
			drag_start_grid_pos = car_data.pos
			drag_start_mouse_offset = car_node.get_global_mouse_position() - car_node.global_position
		else:
			if selected_car_id == car_id:
				var current_visual_pos = car_node.global_position
				var target_grid_x = round((current_visual_pos.x - start_pos.x) / float(cell_size))
				var target_grid_y = round((current_visual_pos.y - start_pos.y) / float(cell_size))
				var final_grid_pos = _calculate_valid_snap_position(car_id, Vector2i(target_grid_x, target_grid_y))
				_update_car_visuals_and_occupancy(car_id, final_grid_pos, drag_start_grid_pos)
				selected_car_id = null
				_check_for_win(car_id)
	elif event is InputEventMouseMotion and selected_car_id == car_id and Input.is_mouse_button_pressed(MOUSE_BUTTON_LEFT):
		var target_visual_pos = get_global_mouse_position() - drag_start_mouse_offset
		if car_data.is_vertical: target_visual_pos.x = start_pos.x + car_data.pos.x * cell_size + padding
		else: target_visual_pos.y = start_pos.y + car_data.pos.y * cell_size + padding
		car_node.global_position = target_visual_pos

func _calculate_valid_snap_position(car_id: String, desired_grid_pos_approx: Vector2i) -> Vector2i:
	var car_data = car_nodes_map[car_id]
	var original_grid_pos = drag_start_grid_pos
	var target_grid_pos = original_grid_pos
	if car_data.is_vertical:
		target_grid_pos.x = original_grid_pos.x
		var dy = desired_grid_pos_approx.y - original_grid_pos.y
		var direction_y = sign(dy)
		for i in range(1, abs(dy) + 1):
			var potential_pos = Vector2i(original_grid_pos.x, original_grid_pos.y + i * direction_y)
			if _is_move_valid(car_id, potential_pos): target_grid_pos.y = potential_pos.y
			else: break
	else:
		target_grid_pos.y = original_grid_pos.y
		var dx = desired_grid_pos_approx.x - original_grid_pos.x
		var direction_x = sign(dx)
		for i in range(1, abs(dx) + 1):
			var potential_pos = Vector2i(original_grid_pos.x + i * direction_x, original_grid_pos.y)
			if _is_move_valid(car_id, potential_pos): target_grid_pos.x = potential_pos.x
			else: break
	return target_grid_pos

func _is_move_valid(car_id_moving: String, target_logical_pos: Vector2i) -> bool:
	var car_data = car_nodes_map[car_id_moving]
	var car_length = car_data.length
	var car_is_vertical = car_data.is_vertical

	for i in range(car_length):
		var lc = target_logical_pos.x # logical column untuk segmen ini
		var lr = target_logical_pos.y # logical row untuk segmen ini
		if car_is_vertical: lr += i
		else: lc += i

		# Kasus khusus: Mobil target mencoba keluar
		if car_data.is_target and not car_is_vertical and lr == EXIT_ROW and lc == LOGICAL_GRID_COLUMNS:
			# Mobil target mencoba bergerak ke kolom logis "di luar" grid pada baris keluar.
			# Ini adalah posisi "keluar". Kita cek sel EXIT_IDENTIFIER di padded grid.
			var exit_padded_pos = logical_to_padded_pos(Vector2i(LOGICAL_GRID_COLUMNS, EXIT_ROW))
			# Sebenarnya, exit slot itu ada di PADDED_GRID_COLUMNS-1, bukan LOGICAL_GRID_COLUMNS + PADDING_SIZE
			# exit_padded_pos.x akan menjadi LOGICAL_GRID_COLUMNS + PADDING_SIZE
			# Sel yang kita tandai EXIT_IDENTIFIER adalah grid_occupancy[EXIT_ROW + PADDING_SIZE][PADDED_GRID_COLUMNS -1]
			# Jadi, jika lc == LOGICAL_GRID_COLUMNS, artinya mobil menyentuh dinding kanan.

			var actual_exit_padded_col = PADDED_GRID_COLUMNS - 1 # Kolom dinding kanan
			var actual_exit_padded_row = EXIT_ROW + PADDING_SIZE

			if grid_occupancy[actual_exit_padded_row][actual_exit_padded_col] == EXIT_IDENTIFIER:
				# Boleh bergerak ke sini (sel ini akan ditangani oleh _check_for_win)
				# Jika ada mobil lain di EXIT_IDENTIFIER (tidak mungkin), maka tidak valid.
				var occupant = grid_occupancy[actual_exit_padded_row][actual_exit_padded_col]
				if occupant != EXIT_IDENTIFIER and occupant != null and occupant != car_id_moving: # Seharusnya EXIT_IDENTIFIER atau null
					return false
				continue # Segmen ini valid menuju/di pintu keluar
			else: # Jika bukan EXIT_IDENTIFIER (misal WALL)
				return false # Tidak bisa keluar lewat sini


		# Untuk semua kasus lain (mobil non-target, atau mobil target tidak di pintu keluar)
		# Segmen mobil harus berada dalam batas logis grid permainan.
		if not (lc >= 0 and lc < LOGICAL_GRID_COLUMNS and lr >= 0 and lr < LOGICAL_GRID_ROWS):
			return false # Di luar area bermain logis dan bukan gerakan keluar yang valid

		# Konversi ke koordinat padded untuk cek grid_occupancy
		var padded_pos_segment = logical_to_padded_pos(Vector2i(lc, lr))
		var cell_content = grid_occupancy[padded_pos_segment.y][padded_pos_segment.x]

		if cell_content == WALL_IDENTIFIER:
			return false # Menabrak dinding
		if cell_content != null and cell_content != car_id_moving:
			return false # Menabrak mobil lain

	return true

func _check_for_win(moved_car_id: String) -> void:
	var car_data = car_nodes_map[moved_car_id]
	if car_data.is_target and not car_data.is_vertical and car_data.pos.y == EXIT_ROW:
		# Cek apakah salah satu segmen mobil target menempati sel EXIT
		var exit_padded_pos = Vector2i(PADDED_GRID_COLUMNS - 1, EXIT_ROW + PADDING_SIZE) # Koordinat padded dari sel EXIT

		# Cek apakah mobil target sekarang menempati sel EXIT ini
		# Mobil target menang jika ujung kanannya (pos.x + length - 1 secara logis)
		# Berada di kolom LOGICAL_GRID_COLUMNS - 1 (kolom terakhir yang bisa dimainkan)
		# DAN sel EXIT di sebelahnya (yang ditandai EXIT_IDENTIFIER) ditempati oleh mobil ini.
		# Atau lebih sederhana, jika posisi logis X mobil adalah LOGICAL_GRID_COLUMNS - car_data.length
		# dan mobil bisa bergerak maju satu langkah lagi.
		# Kondisi paling sederhana: ujung kanan mobil (logical_pos.x + length -1)
		# telah mencapai atau melewati kolom terakhir yang valid (LOGICAL_GRID_COLUMNS -1)
		# DAN mobil tersebut berada di EXIT_ROW. Jika mobil bisa bergerak ke (LOGICAL_GRID_COLUMNS, EXIT_ROW)
		# berarti dia menang.
		# Jadi, kita cek apakah mobil target setelah bergerak, ujung kanannya
		# ada di LOGICAL_GRID_COLUMNS-1 (kolom terakhir dalam grid 6x6)
		# DAN bisa bergerak satu langkah lagi ke kanan.
		# Atau, jika mobil target sudah menempati posisi di mana ujung kanannya adalah LOGICAL_GRID_COLUMNS.
		if (car_data.pos.x + car_data.length -1) >= LOGICAL_GRID_COLUMNS : # Ujung kanan mobil sudah di "luar" area bermain logis
			print("SELAMAT! MOBIL TARGET BERHASIL KELUAR! (pos.x=", car_data.pos.x,")")
			# get_tree().reload_current_scene()


# --- Tombol UI untuk Solver ---
func _on_SolveButton_pressed() -> void:
	if solver_thread != null and solver_thread.is_alive():
		print("Solver BFS sedang berjalan.")
		return
	print("Memulai solver BFS...")
	var initial_cars_state_data = _get_current_cars_state_data_for_solver()
	solver_thread = Thread.new()
	solver_thread.start(_run_bfs_solver_in_thread.bind(initial_cars_state_data))
	solver_finished.connect(_on_solver_finished_received, CONNECT_ONE_SHOT) # CONNECT_ONE_SHOT jika hanya sekali solve

func _on_solver_finished_received(solution_path: Array) -> void:
	if solver_thread != null:
		solver_thread.wait_to_finish()
		solver_thread = null
	if solution_path.is_empty():
		print("Solver BFS: Tidak ada solusi ditemukan atau batas tercapai.")
	else:
		print("Solver BFS: Solusi ditemukan dengan ", solution_path.size(), " langkah.")
		_animate_solution(solution_path)

# --- FUNGSI YANG BERJALAN DI THREAD TERPISAH (BFS) ---
func _run_bfs_solver_in_thread(initial_cars_data: Dictionary) -> void:
	var queue = [] # Antrian untuk BFS: [state_tuple, ...]
	var visited_states = {} # {state_tuple: true} untuk closed_set
	var came_from = {}  # {state_tuple: {"from_state": state_tuple, "move": move_object}}

	var start_state_tuple = _convert_cars_data_to_state_tuple(initial_cars_data)
	queue.append(start_state_tuple)
	visited_states[start_state_tuple] = true

	var iterations = 0
	var MAX_ITERATIONS = 100000 # Tingkatkan jika perlu untuk puzzle kompleks

	while not queue.is_empty() and iterations < MAX_ITERATIONS:
		iterations += 1
		var current_state_tuple = queue.pop_front()

		if _is_goal_state_from_tuple(current_state_tuple, initial_cars_data):
			var path = _reconstruct_path(came_from, current_state_tuple)
			# Perbaikan: Gunakan call_deferred untuk emit sinyal dari main thread
			call_deferred("emit_signal", "solver_finished", path)
			return

		var current_cars_config_from_tuple = _convert_state_tuple_to_cars_data(current_state_tuple, initial_cars_data)

		for car_id_to_move in current_cars_config_from_tuple:
			var car_to_move_data = current_cars_config_from_tuple[car_id_to_move]
			for direction in [-1, 1]:
				for steps in range(1, LOGICAL_GRID_COLUMNS): # Maksimum langkah mungkin (bisa dioptimalkan)
					var new_pos_for_car: Vector2i
					var original_car_pos = car_to_move_data.pos
					if car_to_move_data.is_vertical:
						new_pos_for_car = Vector2i(original_car_pos.x, original_car_pos.y + steps * direction)
					else:
						new_pos_for_car = Vector2i(original_car_pos.x + steps * direction, original_car_pos.y)

					var next_potential_cars_config = _deep_copy_cars_data_for_solver(current_cars_config_from_tuple)
					next_potential_cars_config[car_id_to_move].pos = new_pos_for_car

					if not _is_abstract_move_valid(car_id_to_move, new_pos_for_car, car_to_move_data.length, car_to_move_data.is_vertical, next_potential_cars_config):
						break 

					var neighbor_state_tuple = _convert_cars_data_to_state_tuple(next_potential_cars_config)

					if not visited_states.has(neighbor_state_tuple):
						visited_states[neighbor_state_tuple] = true
						var move_object = {"car_id": car_id_to_move, "from_pos": original_car_pos, "to_pos": new_pos_for_car}
						came_from[neighbor_state_tuple] = {"from_state": current_state_tuple, "move": move_object}
						queue.append(neighbor_state_tuple)
	
	# Jika loop selesai tanpa menemukan solusi
	# Perbaikan: Gunakan call_deferred untuk emit sinyal dari main thread
	call_deferred("emit_signal", "solver_finished", [])
# --- AKHIR FUNGSI THREAD BFS ---

# --- Helper Functions untuk Solver (sebagian besar sama dengan A*) ---
func _get_current_cars_state_data_for_solver() -> Dictionary:
	var current_state = {}
	for car_id in car_nodes_map:
		var car_game_data = car_nodes_map[car_id] # Perbaikan: akses langsung
		current_state[car_id] = {
			"pos": car_game_data.pos,
			"length": car_game_data.length,
			"is_vertical": car_game_data.is_vertical,
			"is_target": car_game_data.is_target,
			"original_id": car_id
		}
	return current_state

func _deep_copy_cars_data_for_solver(original_data: Dictionary) -> Dictionary:
	var new_copy = {}
	for car_id in original_data:
		new_copy[car_id] = original_data[car_id].duplicate(true)
	return new_copy

func _convert_cars_data_to_state_tuple(cars_data_dict: Dictionary) -> Array:
	var state_list = []
	var sorted_car_ids = cars_data_dict.keys()
	sorted_car_ids.sort()
	for car_id in sorted_car_ids:
		var car = cars_data_dict[car_id]
		state_list.append([car_id, car.pos.x, car.pos.y])
	return state_list

func _convert_state_tuple_to_cars_data(state_tuple: Array, template_cars_data: Dictionary) -> Dictionary:
	var cars_data = {}
	for car_info_in_tuple in state_tuple:
		var car_id = car_info_in_tuple[0]
		var original_car_template = template_cars_data[car_id]
		cars_data[car_id] = {
			"pos": Vector2i(car_info_in_tuple[1], car_info_in_tuple[2]),
			"length": original_car_template.length,
			"is_vertical": original_car_template.is_vertical,
			"is_target": original_car_template.is_target,
			"original_id": car_id
		}
	return cars_data

func _get_occupant_in_abstract_state(cell_pos: Vector2i, cars_config_dict: Dictionary, exclude_car_id: String = "") -> String:
	for car_id_iter in cars_config_dict:
		if car_id_iter == exclude_car_id: continue
		var car = cars_config_dict[car_id_iter]
		for i in range(car.length):
			var current_cell_x = car.pos.x
			var current_cell_y = car.pos.y
			if car.is_vertical: current_cell_y += i
			else: current_cell_x += i
			if current_cell_x == cell_pos.x and current_cell_y == cell_pos.y:
				return car_id_iter
	return "" # Atau null jika lebih disukai

func _is_goal_state_from_tuple(state_tuple: Array, template_cars_data: Dictionary) -> bool:
	var cars_config_dict = _convert_state_tuple_to_cars_data(state_tuple, template_cars_data)
	for car_id_iter in cars_config_dict:
		var car = cars_config_dict[car_id_iter]
		if car.is_target:
			if not car.is_vertical and car.pos.y == EXIT_ROW:
				if car.pos.x + car.length >= LOGICAL_GRID_COLUMNS: return true
			return false
	return false

func _is_abstract_move_valid(car_id_moving: String, new_car_pos: Vector2i, car_length: int, car_is_vertical: bool, current_cars_config: Dictionary) -> bool:
	var final_x = new_car_pos.x
	var final_y = new_car_pos.y
	if car_is_vertical: final_y += car_length - 1
	else: final_x += car_length - 1
	if not (new_car_pos.x >= 0 and final_x < LOGICAL_GRID_COLUMNS and new_car_pos.y >= 0 and final_y < LOGICAL_GRID_ROWS):
		return false
	for i in range(car_length):
		var check_cell_x = new_car_pos.x
		var check_cell_y = new_car_pos.y
		if car_is_vertical: check_cell_y += i
		else: check_cell_x += i
		var occupant_id = _get_occupant_in_abstract_state(Vector2i(check_cell_x, check_cell_y), current_cars_config, car_id_moving)
		if occupant_id != null and occupant_id != "": # Perbaikan: Cek string kosong jika itu yang dikembalikan
			return false
	return true

func _reconstruct_path(came_from_map: Dictionary, current_state_tuple: Array) -> Array:
	var total_path = []
	var temp_current_tuple = current_state_tuple
	while came_from_map.has(temp_current_tuple):
		var entry = came_from_map[temp_current_tuple]
		total_path.append(entry.move)
		temp_current_tuple = entry.from_state
	total_path.reverse()
	return total_path

# Tambahkan di bagian atas skrip Anda, bersama variabel anggota lainnya
var _anim_timer: Timer = null
var _anim_current_move_idx: int = 0
var _anim_solution_path: Array = []

func _animate_solution(solution_path: Array) -> void:
	if solution_path.is_empty():
		print("ANIM: solution_path kosong, tidak ada animasi.")
		return

	print("Menganimasikan solusi BFS...")
	print("ANIM: Jumlah langkah untuk dianimasikan: ", solution_path.size())
	# print("ANIM: Isi solution_path: ", solution_path)

	_anim_solution_path = solution_path # Simpan path ke variabel anggota
	_anim_current_move_idx = 0	   # Reset index

	if _anim_timer != null and is_instance_valid(_anim_timer): # Hentikan dan hapus timer lama jika ada
		_anim_timer.stop()
		_anim_timer.queue_free()

	_anim_timer = Timer.new()
	_anim_timer.name = "BFSSolutionAnimationTimer"
	_anim_timer.wait_time = 0.6
	_anim_timer.one_shot = false

	print("ANIM: Timer dibuat. wait_time: ", _anim_timer.wait_time)

	# Hubungkan ke fungsi anggota, bukan lambda
	_anim_timer.timeout.connect(_on_animation_timer_timeout)

	add_child(_anim_timer)
	_anim_timer.start()
	print("ANIM: Timer dimulai. Menunggu tick pertama...")

# Buat fungsi baru untuk menangani timeout timer
func _on_animation_timer_timeout() -> void:
	print("ANIM: Timer tick! current_move_idx: ", _anim_current_move_idx)

	if _anim_current_move_idx >= _anim_solution_path.size():
		print("ANIM: Semua langkah selesai dianimasikan.")
		if _anim_timer != null and is_instance_valid(_anim_timer):
			_anim_timer.stop()
			_anim_timer.queue_free()
			_anim_timer = null # Bersihkan referensi
		print("Animasi solusi BFS selesai.")
		return

	print("ANIM: Memproses langkah ke-", _anim_current_move_idx)
	var move = _anim_solution_path[_anim_current_move_idx]

	if not move is Dictionary or not move.has("car_id") or not move.has("to_pos"):
		printerr("ANIM ERROR: Format 'move' tidak valid di langkah ", _anim_current_move_idx, ": ", move)
		if _anim_timer != null and is_instance_valid(_anim_timer): _anim_timer.stop(); _anim_timer.queue_free(); _anim_timer = null
		return

	var car_id = move.car_id
	var to_pos = move.to_pos

	print("ANIM: car_id: ", car_id, ", to_pos: ", to_pos)

	if not car_nodes_map.has(car_id):
		printerr("ANIM ERROR: car_id '", car_id, "' tidak ditemukan di car_nodes_map.")
		if _anim_timer != null and is_instance_valid(_anim_timer): _anim_timer.stop(); _anim_timer.queue_free(); _anim_timer = null
		return

	var car_data_anim = car_nodes_map[car_id]
	if not car_data_anim is Dictionary or not car_data_anim.has("pos"):
		printerr("ANIM ERROR: car_data untuk '", car_id,"' tidak valid atau tidak memiliki key 'pos': ", car_data_anim)
		if _anim_timer != null and is_instance_valid(_anim_timer): _anim_timer.stop(); _anim_timer.queue_free(); _anim_timer = null
		return

	var from_pos = car_data_anim.pos 
	print("ANIM: from_pos: ", from_pos)

	print("ANIM: Memanggil _update_car_visuals_and_occupancy untuk ", car_id)
	_update_car_visuals_and_occupancy(car_id, to_pos, from_pos)
	print("ANIM: Selesai _update_car_visuals_and_occupancy untuk langkah ", _anim_current_move_idx)

	_anim_current_move_idx += 1
