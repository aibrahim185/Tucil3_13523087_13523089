extends Sprite2D

var dragging = false
var of = Vector2(0,0)

var snap = 64
var x_start = 320
var y_start = 192
var x_end = 576
var y_end = 448

func _process(delta: float) -> void:
	if dragging:
		var new_pos = get_global_mouse_position() - of
		
		if new_pos.x >= x_start and new_pos.y >= y_start and new_pos.x <= x_end and new_pos.y <= y_end:
			position = Vector2(snapped(new_pos.x, snap), snapped(new_pos.y, snap))
		else:
			position = new_pos
		#print(position)

func _on_button_button_down() -> void:
	dragging = true
	of = get_global_mouse_position() - global_position


func _on_button_button_up() -> void:
	dragging = false
