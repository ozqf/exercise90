#pragma once
// common module compilation unit

// Implementation of structs/code found in:

#include <stdio.h>

#include "com_module.h"

// implementations
#include "com_types.cpp"
#include "com_error.cpp"
#include "com_string_utils.cpp"
#include "com_memory_utils.cpp"
#include "com_maths.cpp"
#include "com_transform.cpp"

#include "com_parse.cpp"
#include "com_byte_buffer.cpp"
#include "com_text_command_buffer.cpp"
#include "com_random.cpp"
#include "com_bitpack.cpp"
#include "com_net_types.cpp"

#include "com_collision.cpp"
#include "com_heap/com_heap.cpp"
#include "com_assets.cpp"
#include "com_assets/com_perlin.cpp"
#include "com_assets/com_textures.cpp"
#include "com_debug.cpp"
#include "com_variables.cpp"
#include "com_render/com_render_types.cpp"
#include "com_render/com_render_utils.cpp"
#include "com_debug.cpp"
