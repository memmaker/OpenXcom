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
#include "../Engine/ScriptBind.h"

namespace OpenXcom
{

RuleSkill::RuleSkill(const std::string &type) : _type(type), _targetMode(BA_NONE), _compatibleWeapons({}), _compatibleBattleType(BT_NONE), _requiredBonus({}), _isPsiRequired(false), _checkHandsOnly(true)
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
	_type = node["type"].as<std::string>(_type);
	
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

	int compBattleType = node["battleType"].as<int>(_compatibleBattleType);
	compBattleType = compBattleType < 0 ? 0 : compBattleType;
	compBattleType = compBattleType > BT_CORPSE ? 0 : compBattleType;
	_compatibleBattleType = static_cast<BattleType>(compBattleType);

	_scriptValues.load(node, parsers.getShared());
}

const std::string RuleSkill::getType() const
{
	return _type;
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
BattleType RuleSkill::getCompatibleBattleType() const
{
	return _compatibleBattleType;
}
const std::vector<std::string> RuleSkill::getRequiredBonus() const
{
	return _requiredBonus;
}

namespace
{
std::string debugDisplayScript(const RuleSkill* rs)
{
	if (rs)
	{
		std::string s;
		s += RuleSkill::ScriptName;
		s += "(name: \"";
		s += rs->getType();
		s += "\")";
		return s;
	}
	else
	{
		return "null";
	}
}
}

void RuleSkill::ScriptRegister(ScriptParserBase* parser)
{
	Bind<RuleSkill> rs = { parser };
	rs.addScriptValue<&RuleSkill::_scriptValues>();
	rs.addDebugDisplay<&debugDisplayScript>();
}

}
