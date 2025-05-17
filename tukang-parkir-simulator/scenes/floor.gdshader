// Shader Godot untuk Grid 1 Meter
shader_type spatial;
// render_mode unshaded; // Tidak terpengaruh pencahayaan, garis akan selalu terlihat jelas
// render_mode world_vertex_coords; // Membuat VERTEX di fragment shader menjadi koordinat dunia
// render_mode cull_disabled; // Nonaktifkan culling jika Anda ingin grid terlihat dari kedua sisi (misal pada PlaneMesh)

// Jika Anda ingin garis/warna dasar memiliki transparansi dan berbaur dengan benar:
// render_mode blend_mix, depth_draw_opaque, cull_disabled;
// Atau jika hanya perlu alpha clip (bagian transparan tidak digambar sama sekali):
// render_mode depth_draw_opaque, cull_disabled; // ALPHA_SCISSOR_THRESHOLD akan digunakan

uniform float grid_spacing : hint_range(0.1, 10.0, 0.01) = 1.0; // Ukuran setiap sel grid (meter)
uniform float line_thickness : hint_range(0.001, 0.5, 0.001) = 0.1; // Ketebalan garis (meter)
uniform vec4 line_color : source_color = vec4(0.0, 0.0, 0.0, 1.0);   // Warna garis (hitam default)
uniform vec4 base_color : source_color = vec4(0.8, 0.8, 0.8, 1.0);   // Warna dasar (abu-abu default)

// Pilih sumbu mana yang akan digunakan untuk menggambar grid
// Ini berguna untuk menentukan orientasi grid (misalnya, lantai XZ, dinding XY, dll.)
// 0: Bidang XY (garis sepanjang sumbu X dan Y dunia)
// 1: Bidang XZ (garis sepanjang sumbu X dan Z dunia - umum untuk lantai/langit-langit)
// 2: Bidang YZ (garis sepanjang sumbu Y dan Z dunia)
uniform int grid_plane : hint_enum("Bidang XY (Dinding/Sprite)", "Bidang XZ (Lantai/Langit-langit)", "Bidang YZ (Dinding)") = 1;

// Varying untuk meneruskan posisi dunia dari vertex ke fragment shader
// (Tidak dibutuhkan jika menggunakan render_mode world_vertex_coords)
varying vec3 world_pos;

void vertex() {
    // Transformasi VERTEX (koordinat model) ke koordinat dunia
    world_pos = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
    // Jika menggunakan Godot 4 dan ingin lebih ringkas (namun world_vertex_coords lebih disarankan untuk ini):
    // world_pos = VERTEX_WORLD;
}

void fragment() {
    vec3 current_pos_in_world;

    // Jika menggunakan render_mode world_vertex_coords, VERTEX sudah dalam koordinat dunia
    // current_pos_in_world = VERTEX;
    // Jika tidak, gunakan varying yang sudah dihitung di vertex shader
    current_pos_in_world = world_pos;

    float coord1, coord2;

    if (grid_plane == 0) { // Bidang XY
        coord1 = current_pos_in_world.x;
        coord2 = current_pos_in_world.y;
    } else if (grid_plane == 1) { // Bidang XZ
        coord1 = current_pos_in_world.x;
        coord2 = current_pos_in_world.z;
    } else { // Bidang YZ
        coord1 = current_pos_in_world.y;
        coord2 = current_pos_in_world.z;
    }

    float half_line_w = line_thickness / 2.0;

    // Hitung jarak ke garis grid terdekat untuk koordinat pertama
    // abs(pos - round(pos / spacing) * spacing) memberikan jarak dari 'pos' ke kelipatan 'spacing' terdekat
    bool on_line1 = abs(coord1 - round(coord1 / grid_spacing) * grid_spacing) < half_line_w;

    // Hitung jarak ke garis grid terdekat untuk koordinat kedua
    bool on_line2 = abs(coord2 - round(coord2 / grid_spacing) * grid_spacing) < half_line_w;

    if (on_line1 || on_line2) {
        ALBEDO = line_color.rgb;
        ALPHA = line_color.a;
    } else {
        ALBEDO = base_color.rgb;
        ALPHA = base_color.a;
    }

    // Jika Anda menggunakan nilai ALPHA < 1.0 dan ingin alpha blending:
    // Pastikan Material di Inspector memiliki properti Transparency diatur ke "Alpha".
    // Dan tambahkan `render_mode blend_mix;` di atas.
    // Jika Anda ingin alpha scissor (transparansi keras berdasarkan threshold):
    // Atur Material Transparency ke "Alpha Scissor" dan ALPHA_SCISSOR_THRESHOLD di shader jika perlu.
    // ALPHA_SCISSOR_THRESHOLD = 0.5; // Contoh
}