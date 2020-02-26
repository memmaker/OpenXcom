#pragma once
/*
 * Copyright 2010-2016 OpenXcom Developers.
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
 * along with OpenXcom.  If not, see <http:///www.gnu.org/licenses/>.
 */
#include "ActionMenuState.h"
#include "BattlescapeGame.h"


namespace OpenXcom
{

struct RuleItemUseCost;
class ActionMenuState;
class ActionMenuItem;
class EquipmentLayoutItem;
	
/**
 * Window that allows the player
 * to select a battlescape action.
 */
class SkillMenuState : public ActionMenuState
{
private:
	/// Checck if the required bonuses for this skill are available
	bool hasBonus(Soldier *soldier, const RuleSkill *skillRules);
	/// Adds a new menu item for an action.
	void addItem(const RuleSkill* skill, int *id, SDLKey key);
	void chooseWeaponForSkill(BattleAction* action, const std::vector<std::string> &compatibleWeaponTypes, BattleType compatibleWeaponType, bool checkHandsOnly);
	BattleItem *findItemInInventory(BattleUnit *unit, BattleType battleType);
	BattleType getBattleTypeFromActionType(BattleActionType actionType);
public:
	/// Creates the Action Menu state.
	SkillMenuState(BattleAction *action, int x, int y);
	/// Cleans up the Action Menu state.
	~SkillMenuState();
	/// Handler for clicking a action menu item.
	void btnActionMenuItemClick(Action *action);
};

}
