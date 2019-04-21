#pragma once

/////////////////////////////////////////////////////////
// Interface
/////////////////////////////////////////////////////////
#include "../../common/common.h"
#include "game.h"
#include "../app_module.h"

/////////////////////////////////////////////////////////
// Globals
/////////////////////////////////////////////////////////
#include "game_types.h"
#include "game_types.cpp"
#include "game_command_types.h"
#include "game_command_types.cpp"
#include "game_globals.cpp"

/////////////////////////////////////////////////////////
// Implementations
/////////////////////////////////////////////////////////

#include "game_utils.cpp"
#include "game_teams.cpp"
#include "game_local_entities.cpp"
#include "game_entities.cpp"
#include "EntityComponents/ec_types.cpp"
#include "EntityComponents/ent_attacks.cpp"
#include "EntityComponents/ec_actor.cpp"
#include "EntityComponents/ec_aiController.cpp"
#include "EntityComponents/ec_collider.cpp"
#include "EntityComponents/ec_volume.cpp"
#include "EntityComponents/ec_misc.cpp"
#include "EntityComponents/ec_thinker.cpp"
#include "EntityComponents/ec_apply_state.cpp"
#include "EntityComponents/ec_factory.cpp"
#include "EntityComponents/ent_enemy_templates.cpp"
#include "EntityComponents/ent_templates.cpp"
#include "game_draw.cpp"

#include "game_entityFactory.cpp"
#include "game_physics.cpp"
#include "game_state.cpp"
#include "game_input.cpp"
#include "game_server.cpp"
#include "game_embedded_scenes.cpp"
#include "game_ui.cpp"
#include "game_replication.cpp"
#include "game_exec.cpp"
#include "game_main.cpp"


