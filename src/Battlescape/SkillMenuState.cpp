/*
 * Copyright 2010-2020 OpenXcom Developers.
 *
 * This file is part of OpenXcom.
 *
 * OpenXcom is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenXcom is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenXcom.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SkillMenuState.h"
#include "../Engine/Game.h"
#include "../Engine/Options.h"
#include "../Engine/LocalizedText.h"
#include "../Engine/Action.h"
#include "../Engine/Sound.h"
#include "../Engine/Unicode.h"
#include "../Savegame/BattleUnit.h"
#include "../Savegame/BattleItem.h"
#include "../Savegame/SoldierDiary.h"
#include "../Mod/Mod.h"
#include "../Mod/RuleSkill.h"
#include "../Mod/RuleSoldier.h"
#include "../Mod/RuleSoldierBonus.h"
#include "ActionMenuItem.h"
#include "PrimeGrenadeState.h"
#include "MedikitState.h"
#include "ScannerState.h"
#include "../Savegame/SavedGame.h"
#include "../Savegame/SavedBattleGame.h"
#include "../Savegame/Tile.h"
#include "Pathfinding.h"
#include "TileEngine.h"
#include "../Interface/Text.h"
#include "../Savegame/EquipmentLayoutItem.h"

namespace OpenXcom
{
	
/**
 * Initializes all the elements in the Action Menu window.
 * @param game Pointer to the core game.
 * @param action Pointer to the action.
 * @param x Position on the x-axis.
 * @param y position on the y-axis.
 */
SkillMenuState::SkillMenuState(BattleAction *action, int x, int y) : ActionMenuState(action)
{
	// cache the currently selected weapon and skill
	const RuleSkill *currentSkill = _action->skillRules;
	BattleItem *currentWeapon = _action->weapon;
	BattleActionType currentActionType = _action->type;

	_screen = false;
	
	// Set palette
	_game->getSavedGame()->getSavedBattle()->setPaletteByDepth(this);

	for (int i = 0; i < 6; ++i)
	{
		_actionMenu[i] = new ActionMenuItem(i, _game, x, y);
		add(_actionMenu[i]);
		_actionMenu[i]->setVisible(false);
		_actionMenu[i]->onMouseClick((ActionHandler)&SkillMenuState::btnActionMenuItemClick);
	}

	auto soldier = _action->actor->getGeoscapeSoldier();
	const RuleSoldier *soldierRules = soldier->getRules();
	
	// Build up the popup menu
	int id = 0;
	
	auto skills = soldierRules->getSkills();
	std::vector<SDLKey> hotkeys = {Options::keyBattleActionItem5, Options::keyBattleActionItem4, Options::keyBattleActionItem3, Options::keyBattleActionItem2, Options::keyBattleActionItem1};
	for (auto skill : skills)
	{
		if (hasBonus(soldier, skill)
			&& (skill->getCost().Time > 0 || skill->getCost().Mana > 0)
			&& (!skill->isPsiRequired() || _action->actor->getBaseStats()->psiSkill > 0))
		{
			_action->skillRules = skill;
			_action->type = skill->getTargetMode();
			chooseWeaponForSkill(_action, skill->getCompatibleWeapons(), skill->getCompatibleBattleType(), skill->checkHandsOnly());
			addItem(skill, &id, hotkeys.back());
			hotkeys.pop_back();
		}
	}
	// restore previously selected weapon and skill
	_action->type = currentActionType;
	_action->skillRules = currentSkill;
	_action->weapon = currentWeapon;
}

/**
 * Deletes the SkillMenuState.
 */
SkillMenuState::~SkillMenuState()
{

}

bool SkillMenuState::hasBonus(Soldier *soldier, const RuleSkill *skillRules)
{
	// Does the soldier have the required bonus?
	for (auto reqd_bonus : skillRules->getRequiredBonus())
	{
		bool found = false;
		for (const RuleSoldierBonus *bonus : *soldier->getBonuses(_game->getMod()))
		{
			if (bonus->getName() == reqd_bonus)
			{
				found = true;
				break;
			}
		}
		if (!found)
			return false;
	}
	return true;
}

/**
 * Adds a new menu item for an action.
 * @param ba Action type.
 * @param name Action description.
 * @param id Pointer to the new item ID.
 */
void SkillMenuState::addItem(const RuleSkill* skill, int *id, SDLKey key)
{
	BattleActionType ba = skill->getTargetMode();
	const std::string &name = skill->getType();


	std::string s1, s2;
	RuleItemUseCost cost = _action->actor->getActionTUs(ba, _action->skillRules);
	
	if (_action->weapon)
	{
		int acc = _action->actor->getFiringAccuracy(ba, _action->weapon, _game->getMod());
		if (ba == BA_THROW || ba == BA_AIMEDSHOT || ba == BA_SNAPSHOT || ba == BA_AUTOSHOT || ba == BA_LAUNCH || ba == BA_HIT)
			s1 = tr("STR_ACCURACY_SHORT").arg(Unicode::formatPercentage(acc));
	}
	
	if (cost.Time > 0)
	{
		s2 = tr("STR_TIME_UNITS_SHORT").arg(cost.Time);
	}
	else if (cost.Mana > 0)
	{
		s2 = tr("STR_MANA_SHORT").arg(cost.Mana);
	}

	_actionMenu[*id]->setAction(ba, tr(name), s1, s2, cost.Time);
	_actionMenu[*id]->setSkill(skill);
	_actionMenu[*id]->setVisible(true);
	
	if (key != SDLK_UNKNOWN)
	{
		_actionMenu[*id]->onKeyboardPress((ActionHandler)&SkillMenuState::btnActionMenuItemClick, key);
	}
	(*id)++;
}


/**
 * Executes the action corresponding to this action menu item.
 * @param action Pointer to an action.
 */
void SkillMenuState::btnActionMenuItemClick(Action *action)
{
	_game->getSavedGame()->getSavedBattle()->getPathfinding()->removePreview();
	
	int btnID = -1;

	// got to find out which button was pressed
	for (size_t i = 0; i < std::size(_actionMenu) && btnID == -1; ++i)
	{
		if (action->getSender() == _actionMenu[i])
		{
			btnID = i;
			break;
		}
	}

	if (btnID != -1)
	{
		std::string actionResult = "STR_UNKNOWN"; // needs a non-empty default/fall-back !
		
		OpenXcom::TileEngine *tileEngine = _game->getSavedGame()->getSavedBattle()->getTileEngine();
		const RuleSkill *selectedSkill = _actionMenu[btnID]->getSkill();
		_action->skillRules = selectedSkill;
		_action->type = _actionMenu[btnID]->getAction();
		chooseWeaponForSkill(_action, selectedSkill->getCompatibleWeapons(), selectedSkill->getCompatibleBattleType(), selectedSkill->checkHandsOnly());
		_action->updateTU();

		bool continueAction = tileEngine->skillUse(_action, selectedSkill);

		if (!continueAction || _action->type == BA_NONE)
		{
			_action->type = BA_NONE;
			_action->targeting = false;
			_game->popState();
			return;
		}

		// check if the skill needs an item, if so check if an item was found
		BattleItem *item = _action->weapon;
		if (_action->type != BA_NONE)
		{
			if (!item)
			{
				_action->result = tr("STR_SKILL_NEEDS_ITEM");
				_game->popState();
				return;
			}
		}

		if (_action->type == BA_THROW && _action->weapon->getRules()->getBattleType() == BT_GRENADE)
		{
			// instant grenades
			_action->weapon->setFuseTimer(0);
			_action->weapon->setFuseEnabled(true);
		}
		else if (_action->type == BA_PRIME || _action->type == BA_UNPRIME)
		{
			// nothing, skill menu only supports instant grenades for now
			_game->popState();
			return;
		}
		// default handling from here on...
		ActionMenuState::handleAction();
	}
}

void SkillMenuState::chooseWeaponForSkill(BattleAction* action, const std::vector<std::string> &compatibleWeaponTypes, BattleType compatibleBattleType, bool checkHandsOnly)
{
	auto unit = action->actor;
	action->weapon = nullptr;

	if (action->type == BA_NONE)
	{
		return;
	}
	if (compatibleWeaponTypes.size() > 0) // choose by item type
	{
		for (auto itemType : compatibleWeaponTypes)
		{
			// check both hands, right first
			if (unit->getRightHandWeapon() && unit->getRightHandWeapon()->getRules()->getType() == itemType)
			{
				action->weapon = unit->getRightHandWeapon();
				return;
			}
			else if (unit->getLeftHandWeapon() && unit->getLeftHandWeapon()->getRules()->getType() == itemType)
			{
				action->weapon = unit->getLeftHandWeapon();
				return;
			}
			if (!checkHandsOnly)
			{
				// check special weapons
				BattleItem *item = unit->getSpecialWeapon(itemType);
				if (item)
				{
					action->weapon = item;
					return;
				}
				// check inventory
				for (auto invItem : *unit->getInventory())
				{
					if (invItem->getRules()->getType() == itemType)
					{
						action->weapon = invItem;
						return;
					}
				}
			}
		}
	}
	if (compatibleBattleType == BT_NONE)
	{
		compatibleBattleType = getBattleTypeFromActionType(action->type);
	}
	BattleItem *item = findItemInInventory(action->actor, compatibleBattleType);
	if (item)
	{
		action->weapon = item;
		return;
	}
}

BattleType SkillMenuState::getBattleTypeFromActionType(BattleActionType actionType)
{
	switch (actionType) {
		case BA_HIT:
			return BT_MELEE;
		case BA_SNAPSHOT:
		case BA_AIMEDSHOT:
		case BA_AUTOSHOT:
		case BA_LAUNCH:
			return BT_FIREARM;
		case BA_THROW:
			return BT_GRENADE;
		case BA_USE:
		case BA_PANIC:
		case BA_MINDCONTROL:
			return BT_PSIAMP;
		default:
			return BT_NONE;
	}
}

BattleItem *SkillMenuState::findItemInInventory(BattleUnit *unit, BattleType battleType)
{
	// check inventory
	for (auto invItem : *unit->getInventory())
	{
		if (invItem->getRules()->getBattleType() == battleType)
		{
			return invItem;
		}
	}
	return 0;
}
}
