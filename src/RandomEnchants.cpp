/*
* Converted from the original LUA script to a module for Azerothcore(Sunwell) :D
*/
#include "RandomEnchants.h"


static uint32 rerollItem;
static uint32 rerollItemAmount;
static uint32 rerollGoldCost;
static std::string rerollItemName;
static const bool debug = true;
static int maxEnchants;
static int slot1;
static int slot2;
static int slot3;
static int slot4;
static int slot5;

static const char* GetSlotName(uint8 slot, WorldSession* /*session*/)
{
    switch (slot)
    {
    case EQUIPMENT_SLOT_HEAD: return "Head";
    case EQUIPMENT_SLOT_NECK: return "Neck";
    case EQUIPMENT_SLOT_SHOULDERS: return "Shoulders";
    case EQUIPMENT_SLOT_BODY: return "Shirt";
    case EQUIPMENT_SLOT_CHEST: return "Chest";
    case EQUIPMENT_SLOT_WAIST: return "Waist";
    case EQUIPMENT_SLOT_LEGS: return "Legs";
    case EQUIPMENT_SLOT_FEET: return "Feet";
    case EQUIPMENT_SLOT_WRISTS: return "Wrists";
    case EQUIPMENT_SLOT_HANDS: return "Hands";
    case EQUIPMENT_SLOT_FINGER1: return "Right finger";
    case EQUIPMENT_SLOT_FINGER2: return "Left finger";
    case EQUIPMENT_SLOT_TRINKET1: return "Right trinket";
    case EQUIPMENT_SLOT_TRINKET2: return "Left trinket";
    case EQUIPMENT_SLOT_BACK: return "Back";
    case EQUIPMENT_SLOT_MAINHAND: return "Main hand";
    case EQUIPMENT_SLOT_OFFHAND: return "Off hand";
    case EQUIPMENT_SLOT_TABARD: return "Tabard";
    case EQUIPMENT_SLOT_RANGED: return "Ranged";
    default: return NULL;
    }
}
static uint32 Melt(uint8 i, uint8 j)
{
    return (i << 8) + j;
}
static void Unmelt(uint32 melt, uint8& i, uint8& j)
{
    i = melt >> 8;
    j = melt & 0xFF;
}
static Item* GetEquippedItem(Player* player, uint32 guidlow)
{

    for (uint8 i = EQUIPMENT_SLOT_START; i < INVENTORY_SLOT_ITEM_END; ++i)
        if (Item* pItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i))
            if (pItem->GetGUIDLow() == guidlow || pItem->GetEntry() == guidlow)
                return pItem;
    return NULL;
}
std::string itemQualityColor(uint32 quality) {
    if (quality == 0)
        return "|cff9d9d9d";
    else if (quality == 1)
        return "|cffffffff";
    else if (quality == 2)
        return "|cff1eff00";
    else if (quality == 3)
        return "|cff0070dd";
    else if (quality == 4)
        return "|cffa335ee";
    else if (quality == 5)
        return "|cffff8000";
    else if (quality == 6)
        return "|cff850505";
    else if (quality == 7)
        return "|cffe6cc80";
    else
        return "|cff00ccff";
}
class RandomEnchantsPlayer : public PlayerScript{
public:

    //Tier -> item type -> enchant ID

    RandomEnchantsPlayer() : PlayerScript("RandomEnchantsPlayer") { }

    void OnLogin(Player* player) override {
		if (sConfigMgr->GetBoolDefault("RandomEnchants.AnnounceOnLogin", true))
            ChatHandler(player->GetSession()).SendSysMessage(sConfigMgr->GetStringDefault("RandomEnchants.OnLoginMessage", "This server is running a RandomEnchants Module.").c_str());
    }
	void OnLootItem(Player* player, Item* item, uint32 count, uint64 /*lootguid*/) override
	{
		if (sConfigMgr->GetBoolDefault("RandomEnchants.OnLoot", true))
			RollPossibleEnchant(player, item);
	}
	void OnCreateItem(Player* player, Item* item, uint32 count) override
	{
		if (sConfigMgr->GetBoolDefault("RandomEnchants.OnCreate", true))
			RollPossibleEnchant(player, item);
	}
	void OnQuestRewardItem(Player* player, Item* item, uint32 count) override
	{
		if(sConfigMgr->GetBoolDefault("RandomEnchants.OnQuestReward", true))
			RollPossibleEnchant(player, item);
	}
    static void RollPossibleEnchant(Player* player, Item* item)
    {
        uint32 Quality = item->GetTemplate()->Quality;
        uint32 Class = item->GetTemplate()->Class;
        float ilvl = item->GetTemplate()->ItemLevel;
        __if_exists(Player::forgingMap) {
            if (player->forgingMap.find(item->GetTemplate()->ItemId) != player->forgingMap.end()) {
                Player::ForgingData ForgingData = player->forgingMap.at(item->GetTemplate()->ItemId);
                if (ForgingData.ilvlBoost) {
                    ilvl += ForgingData.ilvlBoost;
                }
            }
        }
        float ILevelMult = 0.5 + (ilvl / 100.0);
        if ((Quality > 6 && Quality < 0)/*eliminates enchanting anything that isn't a recognized quality*/ || (Class != 2 && Class != 4)/*eliminates enchanting anything but weapons/armor*/)
            return;
        int slotRand[5] = { -1, -1, -1 ,-1, -1 };
        uint32 slotEnch[5] = { slot1, slot2, slot3, slot4, slot5 };
        double roll = rand_chance();
        if (roll <= (Quality + 1) * 25 * ILevelMult && maxEnchants >= 1)
        {
            debug ? sLog->outString(">> Generating random enchantment in slot 7 (1), ILevelMult is %f", ILevelMult) : NULL;
            slotRand[0] = getRandEnchantment(item);
            if (slotRand[0] != -1 && maxEnchants >= 2)
            {
                roll = rand_chance();
                if (roll <= (Quality) * 20 * ILevelMult)
                {
                    debug ? sLog->outString(">> Generating random enchantment in slot 8 (2)") : NULL;
                    slotRand[1] = getRandEnchantment(item);
                    if (slotRand[1] != -1 && maxEnchants >= 3)
                    {
                        roll = rand_chance();
                        if (roll <= (Quality - 1) * 15 * ILevelMult)
                        {
                            debug ? sLog->outString(">> Generating random enchantment in slot 9 (3)") : NULL;
                            slotRand[2] = getRandEnchantment(item);
                            if (slotRand[2] != -1 && maxEnchants >= 4)
                            {
                                roll = rand_chance();
                                if (roll <= (Quality - 2) * 10 * ILevelMult) {
                                    debug ? sLog->outString(">> Generating random enchantment in slot 10 (4)") : NULL;
                                    slotRand[3] = getRandEnchantment(item);
                                    if (slotRand[3] != -1 && maxEnchants >= 5)
                                    {
                                        roll = rand_chance();
                                        if (roll <= (Quality - 3) * 5 * ILevelMult) {
                                            debug ? sLog->outString(">> Generating random enchantment in slot 11 (5)") : NULL;
                                            slotRand[4] = getRandEnchantment(item);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        debug ? sLog->outString(">> Generated random enchantments") : NULL;
        for (int i = 0; i < 5; i++)
        {
            debug ? sLog->outString(">> checking slot %i for random enchantment", i) : NULL;
            if (slotRand[i] != -1)
            {
                if (sSpellItemEnchantmentStore.LookupEntry(slotRand[i]))//Make sure enchantment id exists
                {
                    debug ? sLog->outString(">> applying enchantment %i from slot %i to player with false", slotRand[i], i) : NULL;
                    player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), false);
                    debug ? sLog->outString(">> applying enchantment %i from slot %i to item", slotRand[i], i) : NULL;
                    item->SetEnchantment(EnchantmentSlot(slotEnch[i]), slotRand[i], 0, 0);
                    debug ? sLog->outString(">> applying enchantment %i from slot %i to player with true", slotRand[i], i) : NULL;
                    player->ApplyEnchantment(item, EnchantmentSlot(slotEnch[i]), true);
                }
            }
        }
        ChatHandler chathandle = ChatHandler(player->GetSession());
        if (slotRand[4] != -1)
            chathandle.PSendSysMessage("The %s %s |rholds traces of|cff571b7e pure mana|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
        else if (slotRand[3] != -1)
            chathandle.PSendSysMessage("The %s %s |rholds traces of|cff5FFB17 psi|r,|cffADD8E6 magic|r, and|cffDC143C ki|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
        else if (slotRand[2] != -1) {
            double roll = rand_chance();
            if (roll < 34)
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cffADD8E6 magic|r and|cffDC143C ki|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
            else if (roll < 67)
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cff5FFB17 psi|r and|cffDC143C ki|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
            else
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cff5FFB17 psi|r and|cffADD8E6 magic|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
        }
        else if (slotRand[1] != -1) {
            if (roll < 34)
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cff5FFB17 psi|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
            else if (roll < 67)
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cffADD8E6 magic|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
            else
                chathandle.PSendSysMessage("The%s %s |rholds traces of|cffDC143C ki|r within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
        }
		else if(slotRand[0] != -1)
            chathandle.PSendSysMessage("The%s %s |rholds traces of power within itself!", itemQualityColor(item->GetTemplate()->Quality).c_str(), item->GetTemplate()->Name1.c_str());
	}
    static int getRandEnchantment(Item* item)
	{
		uint32 Class = item->GetTemplate()->Class;
		uint32 ilvl = item->GetTemplate()->ItemLevel;
		int rarityRoll = -1;
		switch (item->GetTemplate()->Quality)
		{
		case 0://grey
			rarityRoll = (rand_norm() * 25 * ilvl) / 50;
			break;
		case 1://white
			rarityRoll = (rand_norm() * 50 * ilvl) / 50;
			break;
		case 2://green
			rarityRoll = ((45 + (rand_norm() * 20)) * ilvl)/50;
			break;
		case 3://blue
			rarityRoll = ((65 + (rand_norm() * 15)) * ilvl ) / 50;
			break;
		case 4://purple
			rarityRoll = ((80 + (rand_norm() * 14)) * ilvl ) / 50;
			break;
		case 5://orange
			rarityRoll = 100;
			break;
		}
		if (rarityRoll < 0)
			rarityRoll = (rand_norm() * 100 * ilvl)/50;
		int tier = 0;
		if (rarityRoll <= 40)
			tier = 1;
		else if (rarityRoll <= 60)
			tier = 2;
		else if (rarityRoll <= 80)
			tier = 3;
		else if (rarityRoll <= 95)
			tier = 4;
		else
			tier = 5;

		//QueryResult qr = WorldDatabase.PQuery("SELECT enchantID FROM item_enchantment_random_tiers WHERE tier='%d' AND exclusiveSubClass=NULL AND class='%s' OR exclusiveSubClass='%u' OR class='ANY' ORDER BY RAND() LIMIT 1", tier, ClassQueryString.c_str(), item->GetTemplate()->SubClass);
        debug ? sLog->outString(">> Getting size of the any random list") : NULL;
        uint32 anySize = getRE().at(tier - 1).at(0).size();
        debug ? sLog->outString(">> Selecting random index from the appropriate RE vectors, any vector size is %u", anySize) : NULL;
        uint32 id = (rand_norm()*(anySize + ((Class==2||Class==4) ? getRE().at(tier - 1).at(Class/2).size() : 0)));
        debug ? sLog->outString(">> Turning index %u into enchant ID", id) : NULL;
        if (id < anySize) {
            id = getRE().at(tier - 1).at(0).at(id);
            debug ? sLog->outString(">> Enchant translates to %u from the ANY list", id) : NULL;
        }
        else {
            id -= anySize;
            debug ? sLog->outString(">> appropriate Vector is %u of length %u and id has been adjusted to %u", Class/2, getRE().at(tier - 1).at(Class / 2).size(), id) : NULL;
            id = getRE().at(tier - 1).at(Class / 2).at(id);
            debug ? sLog->outString(">> Enchant translates to %u from the item class specific list", id) : NULL;
        }
        //id = (id < anySize) ? getRE().at(tier - 1).at(0).at(id) : getRE().at(tier - 1).at(Class / 2).at(id - anySize);
        debug ? sLog->outString(">> Generated random enchantment ID: %u", id) : NULL;
        return id;
        //return qr->Fetch()[0].GetUInt32();
	}
};
class RandomEnchantsWorld : public WorldScript
{
public:
    RandomEnchantsWorld() : WorldScript("RandomEnchantsWorld") { }

    void OnStartup() {
        uint32 oldMS = getMSTime();
        int RECount = 0;
        sLog->outString(">> Loading Random Enchantments");
        QueryResult qr = WorldDatabase.PQuery("SELECT * FROM item_enchantment_random_tiers");
        for (int i = 0; i < 5; i++) {
            std::vector<std::vector<uint32>> RERow;
            for (int j = 0; j < 3; j++) {
                debug ? sLog->outString(">> Inserting class %i into tier %i", j, i) : NULL;
                std::vector<uint32> RE;
                RERow.push_back (RE);
            }
            debug ? sLog->outString(">> Inserting tier %i into Random Enchants List", i) : NULL;
            randomEnchants.push_back(RERow);
        }
        uint32 tier = 0;
        uint32 Class = 0;
        uint32 enchant = 0;
        do {
            Field* fields = qr->Fetch();
            tier = fields[1].GetUInt32() - 1;
            Class = fields[2].GetUInt32();
            enchant = fields[0].GetUInt32();
            debug ? sLog->outString(">> Inserting enchant %u of Class %u in tier %u into Random Enchants List", enchant,  Class, tier) : NULL;
            randomEnchants.at(tier).at(Class).push_back(enchant);
            RECount++;
        } while (qr->NextRow());
        sLog->outString(">> Loaded %u random enchantments in %u ms", RECount, GetMSTimeDiffToNow(oldMS));
        {
            rerollItem = sConfigMgr->GetIntDefault("RandomEnchants.rerollItem", 20725);
            rerollItemAmount = sConfigMgr->GetIntDefault("RandomEnchants.rerollItemAmount", 5);
            rerollGoldCost = sConfigMgr->GetIntDefault("RandomEnchants.rerollGoldCost", 10000000);
            maxEnchants = sConfigMgr->GetIntDefault("RandomEnchants.maxEnchants", 5);
            slot1 = sConfigMgr->GetIntDefault("RandomEnchants.slot1", 7);
            slot2 = sConfigMgr->GetIntDefault("RandomEnchants.slot2", 8);
            slot3 = sConfigMgr->GetIntDefault("RandomEnchants.slot3", 9);
            slot4 = sConfigMgr->GetIntDefault("RandomEnchants.slot4", 10);
            slot5 = sConfigMgr->GetIntDefault("RandomEnchants.slot5", 11);
            if (sObjectMgr->GetItemTemplateStore()->find(rerollItem) != sObjectMgr->GetItemTemplateStore()->end())
                rerollItemName = sObjectMgr->GetItemTemplateStore()->find(rerollItem)->second.Name1;
            else
                rerollItemName = "Item Name Not Found";
        }
    }

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload) {
            std::string conf_path = _CONF_DIR;
            std::string cfg_file = conf_path + "/RandomEnchants.conf";
#ifdef WIN32
				cfg_file = "RandomEnchants.conf";
#endif
			std::string cfg_def_file = cfg_file +".dist";
            sConfigMgr->LoadMore(cfg_def_file.c_str());

            sConfigMgr->LoadMore(cfg_file.c_str());
        }
    }
};
class Random_Enchanter_NPC : public CreatureScript
{
public:
    Random_Enchanter_NPC() : CreatureScript("Random_Enchanter_NPC") { }


    bool OnGossipHello(Player* player, Creature* creature)
    {

        AddGossipItemFor(player, GOSSIP_ICON_BATTLE, "Select the item to reroll:", GOSSIP_SENDER_MAIN, Melt(MAIN_MENU, 0));
        for (uint8 slot = EQUIPMENT_SLOT_START; slot < EQUIPMENT_SLOT_END; ++slot)
        {
            if (Item* invItem = player->GetItemByPos(INVENTORY_SLOT_BAG_0, slot)) {
                if (const char* slotname = GetSlotName(slot, player->GetSession())) {
                    std::string rerollQuestion = std::string("Are you sure you want to reroll the random enchants on \n\n").append(itemQualityColor(invItem->GetTemplate()->Quality)).
                        append(invItem->GetTemplate()->Name1).append("|r").append(std::string("\n This will cost you \n"));
                    bool needsAnd = false;
                    if (rerollItem) {
                        if (rerollItemAmount) {
                            rerollQuestion = rerollQuestion.append(std::to_string(rerollItemAmount).append(" ").append(rerollItemName));
                            needsAnd = true;
                        }
                    }
                    if (rerollGoldCost) {
                        if (needsAnd)
                            rerollQuestion = rerollQuestion.append("\nAnd\n");
                    }
                    AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, std::string(slotname).append(std::string(": ")).append(itemQualityColor(invItem->GetTemplate()->Quality)).append(invItem->GetTemplate()->Name1).append("|r")
                        , invItem->GetGUIDLow(), REFORGE, (rerollQuestion),
                        rerollGoldCost, false);
                }
            }
        }
        AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Update menu", 0, Melt(MAIN_MENU, 0));
        SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
        return true;
    }


    bool OnGossipSelect(Player* player, Creature* creature, uint32 sender, uint32 menu)
    {
        ClearGossipMenuFor(player);

        switch (menu)
        {
        case MAIN_MENU: OnGossipHello(player, creature); break;
        case SELECT_STAT_INCREASE:
            // sender = item guidlow
            // action = StatsCount id
        {
            AddGossipItemFor(player, GOSSIP_ICON_CHAT, "Update menu", 0, MAIN_MENU);
            Item* invItem = GetEquippedItem(player, sender);
            if (invItem)
            {
                const ItemTemplate* pProto = invItem->GetTemplate();
                std::ostringstream oss;
                oss << "";
                std::string rerollQuestion = std::string("Are you sure you want to reroll\n\n").append(invItem->GetTemplate()->Name1).append(std::string("\n\nThis will cost you \n"));
                bool needsAnd = false;
                if (rerollItem) {
                    if (rerollItemAmount) {
                        rerollQuestion = rerollQuestion.append(std::to_string(rerollItemAmount).append(" ").append(rerollItemName));
                        needsAnd = true;
                    }
                }
                if (rerollGoldCost) {
                    if (needsAnd)
                        rerollQuestion = rerollQuestion.append("\nAnd");
                }
                AddGossipItemFor(player, GOSSIP_ICON_INTERACT_1, pProto->Name1, sender, REFORGE, (rerollQuestion), rerollGoldCost, false);
                SendGossipMenuFor(player, DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());
            }
        }
        break;
        default:
        {
            Item* invItem = GetEquippedItem(player, sender);
            if (invItem)//&& IsReforgable(invItem, player))
            {
                if ((((rerollItem && rerollItemAmount) ? player->HasItemCount(rerollItem, rerollItemAmount, true) : true)
                    && player->HasEnoughMoney(rerollGoldCost)))
                {
                    {
                        player->DestroyItemCount(rerollItem, rerollItemAmount, true);
                        player->ModifyMoney(-rerollGoldCost);
                    }
                    RandomEnchantsPlayer::RollPossibleEnchant(player, invItem);
                    player->GetSession()->SendNotification("Thank you for your purchase, enjoy your item!");
                    CloseGossipMenuFor(player);
                }
                else
                {
                    if (!((rerollItem && rerollItemAmount)? player->HasItemCount(rerollItem, rerollItemAmount, true) : true)) {
                        player->GetSession()->SendNotification("You do not have the required item(s): %d %s", (rerollItemAmount), rerollItemName.c_str());
                        CloseGossipMenuFor(player);
                    }
                    if (!player->HasEnoughMoney(rerollGoldCost)) {
                        player->GetSession()->SendNotification("You do not have the required money");
                        CloseGossipMenuFor(player);
                    }
                }
            }
            else
            {
                player->GetSession()->SendNotification("Invalid item selected");
                CloseGossipMenuFor(player);
            }
        }
        }
        return true;
    }

    enum Menus
    {
        MAIN_MENU = 200, // stat_type_max
        SELECT_ITEM,
        SELECT_STAT_REDUCE,
        SELECT_STAT_INCREASE,
        SELECT_RESTORE,
        RESTORE,
        REFORGE,
    };

};

void AddRandomEnchantsScripts() {
	new RandomEnchantsWorld();
    new RandomEnchantsPlayer();
    new Random_Enchanter_NPC();
}

