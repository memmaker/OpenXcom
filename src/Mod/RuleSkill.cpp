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
#include "RuleSkill.h"

namespace OpenXcom
{

RuleSkill::RuleSkill(const std::string &type) : _type(type), _targetMode(BA_NONE), _compatibleWeapons({}), _requiredBonus({}), _isPsiRequired(false), _checkHandsOnly(true), _scriptId(-1)
{}

/**
 * Loads the skill from a YAML file.
 * @param node YAML node.
 * @param mod Mod for the skill.
 */
void RuleSkill::load(const YAML::Node &node, Mod *mod, const ModScript &parsers)
{
	if (const YAML::Node &parent = node["refNode"])
	{
		load(parent, mod, parsers);
	}
	_scriptId = node["scriptId"].as<int>(_scriptId);
	
	_cost.loadCost(node, "Use");
	_flat.loadPercent(node, "Use");
	
	_requiredBonus = node["requiredBonus"].as<std::vector<std::string>>(_requiredBonus);
	int targetMode = node["targetMode"].as<int>(_targetMode);
	targetMode = targetMode < 0 ? 0 : targetMode;
	targetMode = targetMode > BA_CQB ? 0 : targetMode;
	_targetMode = static_cast<BattleActionType>(targetMode);
	_isPsiRequired = node["isPsiRequired"].as<bool>(_isPsiRequired);
	_checkHandsOnly = node["checkHandsOnly"].as<bool>(_checkHandsOnly);
	_compatibleWeapons = node["compatibleWeapons"].as<std::vector<std::string>>(_compatibleWeapons);
}
	

const std::string RuleSkill::getType() const
{
	return _type;
}
int RuleSkill::getScriptId() const
{
	return _scriptId;
}
bool RuleSkill::isPsiRequired() const
{
	return _isPsiRequired;
}
bool RuleSkill::checkHandsOnly() const
{
	return _checkHandsOnly;
}
const RuleItemUseCost RuleSkill::getFlat() const
{
	return _flat;
}
const RuleItemUseCost RuleSkill::getCost() const
{
	return _cost;
}
BattleActionType RuleSkill::getTargetMode() const
{
	return _targetMode;
}
const std::vector<std::string> RuleSkill::getCompatibleWeapons() const
{
	return _compatibleWeapons;
}
const std::vector<std::string> RuleSkill::getRequiredBonus() const
{
	return _requiredBonus;
}

void RuleSkill::setScriptId(int scriptId)
{
	_scriptId = scriptId;
}
void RuleSkill::setIsPsiRequired(bool isPsiRequired)
{
	_isPsiRequired = isPsiRequired;
}
void RuleSkill::setCheckHandsOnly(bool checkHandsOnly)
{
	_checkHandsOnly = checkHandsOnly;
}
void RuleSkill::setFlat(RuleItemUseCost flat)
{
	_flat = flat;
}
void RuleSkill::setCost(RuleItemUseCost cost)
{
	_cost = cost;
}
void RuleSkill::setTargetMode(BattleActionType targetMode)
{
	_targetMode = targetMode;
}
void RuleSkill::setCompatibleWeapons(std::vector<std::string> compatibleWeapons)
{
	_compatibleWeapons = compatibleWeapons;
}
void RuleSkill::setRequiredBonus(std::vector<std::string> requiredBonus)
{
	_requiredBonus = requiredBonus;
}

}
