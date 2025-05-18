shader_type spatial;

uniform vec4 base_color : source_color = vec4(0.8, 0.8, 0.8, 1.0);
uniform vec4 line_color : source_color = vec4(0.0, 0.0, 0.0, 1.0);
uniform float grid_spacing = 1.0;
uniform float line_thickness = 0.1;
uniform vec2 mesh_actual_size;
uniform int grid_plane = 1;

varying vec3 v_local_position;

void vertex() {
    v_local_position = VERTEX;
}

void fragment() {
    vec2 coord_on_plane;

    if (grid_plane == 1) {
        coord_on_plane = v_local_position.xz;
    } else if (grid_plane == 0) {
        coord_on_plane = v_local_position.xy;
    } else {
        coord_on_plane = v_local_position.yz;
    }

    vec2 shifted_coord = coord_on_plane + mesh_actual_size / 2.0;
    float half_thick = line_thickness / 2.0;

    float mod_val_x = mod(shifted_coord.x, grid_spacing);
    bool on_vertical_line = (mod_val_x < half_thick || mod_val_x > grid_spacing - half_thick);

    if (abs(shifted_coord.x - mesh_actual_size.x) < half_thick && mesh_actual_size.x > 0.001) {
        on_vertical_line = true;
    }

    float mod_val_y = mod(shifted_coord.y, grid_spacing);
    bool on_horizontal_line = (mod_val_y < half_thick || mod_val_y > grid_spacing - half_thick);

    if (abs(shifted_coord.y - mesh_actual_size.y) < half_thick && mesh_actual_size.y > 0.001) {
        on_horizontal_line = true;
    }

    if (on_vertical_line || on_horizontal_line) {
        ALBEDO = line_color.rgb;
        ALPHA = line_color.a;
    } else {
        ALBEDO = base_color.rgb;
        ALPHA = base_color.a;
    }
}