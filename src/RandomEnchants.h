
#include "ScriptMgr.h"
#include "Chat.h"
#include "Player.h"
#include "Configuration/Config.h"

static std::vector<std::vector<std::vector<uint32>>> randomEnchants;
static std::vector<std::vector<std::vector<uint32>>> getRE() { return randomEnchants; }
//void RollPossibleEnchant(Player* player, Item* item) {};
