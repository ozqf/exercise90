#pragma once

/////////////////////////////////////////////////////////
// Interface
/////////////////////////////////////////////////////////
#include "../../common/com_module.h"
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

#include "game_utils.h"
#include "game_teams.h"
#include "game_local_entities.h"
#include "game_entities.h"
#include "EntityComponents/ec_types.h"
#include "EntityComponents/ent_attacks.h"
#include "EntityComponents/ec_aiController.h"
#include "EntityComponents/ec_collider.h"
#include "EntityComponents/ec_volume.h"
#include "EntityComponents/ec_misc.h"
#include "EntityComponents/ec_apply_state.h"
#include "EntityComponents/ec_factory.h"
#include "EntityComponents/ent_enemy_templates.h"
#include "EntityComponents/ent_templates.h"
#include "game_draw.h"
#include "game_players.h"

#include "game_entityFactory.cpp"
#include "game_physics.h"
#include "game_state.h"
#include "game_input.cpp"
#include "game_server.h"
#include "game_embedded_scenes.cpp"
#include "game_ui.cpp"
#include "game_replication.cpp"
#include "game_exec.h"
#include "game_main.cpp"


