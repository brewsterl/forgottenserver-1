/**
 * The Forgotten Server - a server application for the MMORPG Tibia
 * Copyright (C) 2013  Mark Samman <mark.samman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "otpch.h"

#include "definitions.h"

#include "tools.h"
#include "configmanager.h"
#include "sha1.h"

#include <sstream>
#include <iomanip>
#include <random>

#include <cctype>

extern ConfigManager g_config;

std::string transformToSHA1(const std::string& plainText)
{
	SHA1 sha1;
	unsigned sha1Hash[5];

	sha1.Input((const unsigned char*)plainText.c_str(), plainText.length());
	sha1.Result(sha1Hash);

	std::ostringstream hexStream;
	hexStream.flags(std::ios::hex);
	for (uint32_t i = 0; i < 5; ++i) {
		hexStream << std::setw(8) << std::setfill('0') << (uint32_t)sha1Hash[i];
	}
	return hexStream.str();
}

bool passwordTest(const std::string& plain, const std::string& hash)
{
	if (g_config.getNumber(ConfigManager::PASSWORD_TYPE) == PASSWORD_TYPE_SHA1) {
		return transformToSHA1(plain) == hash;
	}
	return plain == hash;
}

void replaceString(std::string& str, const std::string& sought, const std::string& replacement)
{
	size_t pos = 0;
	size_t start = 0;
	size_t soughtLen = sought.length();
	size_t replaceLen = replacement.length();

	while ((pos = str.find(sought, start)) != std::string::npos) {
		str = str.substr(0, pos) + replacement + str.substr(pos + soughtLen);
		start = pos + replaceLen;
	}
}

void trim_right(std::string& source, char t)
{
	source.erase(source.find_last_not_of(t) + 1);
}

void trim_left(std::string& source, char t)
{
	source.erase(0, source.find_first_not_of(t));
}

void toLowerCaseString(std::string& source)
{
	std::transform(source.begin(), source.end(), source.begin(), tolower);
}

std::string asLowerCaseString(const std::string& source)
{
	std::string s = source;
	toLowerCaseString(s);
	return s;
}

std::string asUpperCaseString(const std::string& source)
{
	std::string s = source;
	std::transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}

StringVec explodeString(const std::string& inString, const std::string& separator, int32_t limit/* = -1*/)
{
	StringVec returnVector;
	std::string::size_type start = 0, end = 0;

	while (--limit != -1 && (end = inString.find(separator, start)) != std::string::npos) {
		returnVector.push_back(inString.substr(start, end - start));
		start = end + separator.size();
	}

	returnVector.push_back(inString.substr(start));
	return returnVector;
}

IntegerVec vectorAtoi(const StringVec& stringVector)
{
	IntegerVec returnVector;
	for (const auto& string : stringVector) {
		returnVector.push_back(atoi(string.c_str()));
	}
	return returnVector;
}

bool hasBitSet(uint32_t flag, uint32_t flags)
{
	return ((flags & flag) == flag);
}

std::mt19937& getRandomGenerator()
{
	static std::random_device rd;
	static std::mt19937 generator(rd());
	return generator;
}

int32_t uniform_random(int32_t minNumber, int32_t maxNumber)
{
	static std::uniform_int_distribution<int32_t> uniformRand;
	if (minNumber == maxNumber) {
		return minNumber;
	} else if (minNumber > maxNumber) {
		std::swap(minNumber, maxNumber);
	}
	return uniformRand(getRandomGenerator(), std::uniform_int_distribution<int32_t>::param_type(minNumber, maxNumber));
}

int32_t normal_random(int32_t minNumber, int32_t maxNumber)
{
	static std::normal_distribution<float> normalRand(0.5f, 0.25f);
	if (minNumber == maxNumber) {
		return minNumber;
	} else if (minNumber > maxNumber) {
		std::swap(minNumber, maxNumber);
	}
	return minNumber + std::max<float>(0.f, std::min<float>(1.f, normalRand(getRandomGenerator()))) * (maxNumber - minNumber);
}

bool boolean_random(double probability/* = 0.5*/)
{
	static std::bernoulli_distribution booleanRand;
	return booleanRand(getRandomGenerator(), std::bernoulli_distribution::param_type(probability));
}

bool isNumber(char character)
{
	return (character >= 48 && character <= 57);
}

void trimString(std::string& str)
{
	str.erase(str.find_last_not_of(' ') + 1);
	str.erase(0, str.find_first_not_of(' '));
}

std::string parseNextParam(Tokenizer::iterator& it, Tokenizer::iterator end)
{
	std::string tmp;
	if (it == end) {
		return tmp;
	}

	tmp = *it++;
	trimString(tmp);
	return tmp;
}

std::string convertIPToString(uint32_t ip)
{
	char buffer[17];

	int res = sprintf(buffer, "%u.%u.%u.%u", ip & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24));
	if (res < 0) {
		return "";
	}

	return buffer;
}

std::string formatDate(time_t time)
{
	const tm* tms = localtime(&time);
	if (!tms) {
		return std::string();
	}

	char buffer[20];
	int res = sprintf(buffer, "%02d/%02d/%04d %02d:%02d:%02d", tms->tm_mday, tms->tm_mon + 1, tms->tm_year + 1900, tms->tm_hour, tms->tm_min, tms->tm_sec);
	if (res < 0) {
		return std::string();
	}
	return std::string(buffer, 19);
}

std::string formatDateShort(time_t time)
{
	const tm* tms = localtime(&time);
	if (!tms) {
		return std::string();
	}

	char buffer[12];
	size_t res = strftime(buffer, 12, "%d %b %Y", tms);
	if (res == 0) {
		return std::string();
	}
	return std::string(buffer, 11);
}

Direction getDirection(const std::string& string)
{
	Direction direction = NORTH;

	if (string == "north" || string == "n" || string == "0") {
		direction = NORTH;
	} else if (string == "east" || string == "e" || string == "1") {
		direction = EAST;
	} else if (string == "south" || string == "s" || string == "2") {
		direction = SOUTH;
	} else if (string == "west" || string == "w" || string == "3") {
		direction = WEST;
	} else if (string == "southwest" || string == "south west" || string == "south-west" || string == "sw" || string == "4") {
		direction = SOUTHWEST;
	} else if (string == "southeast" || string == "south east" || string == "south-east" || string == "se" || string == "5") {
		direction = SOUTHEAST;
	} else if (string == "northwest" || string == "north west" || string == "north-west" || string == "nw" || string == "6") {
		direction = NORTHWEST;
	} else if (string == "northeast" || string == "north east" || string == "north-east" || string == "ne" || string == "7") {
		direction = NORTHEAST;
	}

	return direction;
}

Position getNextPosition(Direction direction, Position pos)
{
	switch (direction) {
		case NORTH:
			pos.y--;
			break;

		case SOUTH:
			pos.y++;
			break;

		case WEST:
			pos.x--;
			break;

		case EAST:
			pos.x++;
			break;

		case SOUTHWEST:
			pos.x--;
			pos.y++;
			break;

		case NORTHWEST:
			pos.x--;
			pos.y--;
			break;

		case NORTHEAST:
			pos.x++;
			pos.y--;
			break;

		case SOUTHEAST:
			pos.x++;
			pos.y++;
			break;

		default:
			break;
	}

	return pos;
}

Direction getDirectionTo(const Position& from, const Position& to)
{
	Direction dir;

	int32_t x_offset = Position::getOffsetX(from, to);
	if (x_offset < 0) {
		dir = EAST;
		x_offset = std::abs(x_offset);
	} else {
		dir = WEST;
	}

	int32_t y_offset = Position::getOffsetY(from, to);
	if (y_offset >= 0) {
		if (y_offset > x_offset) {
			dir = NORTH;
		} else if (y_offset == x_offset) {
			if (dir == EAST) {
				dir = NORTHEAST;
			} else {
				dir = NORTHWEST;
			}
		}
	} else {
		y_offset = std::abs(y_offset);
		if (y_offset > x_offset) {
			dir = SOUTH;
		} else if (y_offset == x_offset) {
			if (dir == EAST) {
				dir = SOUTHEAST;
			} else {
				dir = SOUTHWEST;
			}
		}
	}
	return dir;
}

struct MagicEffectNames {
	const char* name;
	MagicEffectClasses effect;
};

struct ShootTypeNames {
	const char* name;
	ShootType_t shoot;
};

struct CombatTypeNames {
	const char* name;
	CombatType_t combat;
};

struct AmmoTypeNames {
	const char* name;
	Ammo_t ammoType;
};

struct AmmoActionNames {
	const char* name;
	AmmoAction_t ammoAction;
};

MagicEffectNames magicEffectNames[] = {
	{"redspark",		NM_ME_DRAW_BLOOD},
	{"bluebubble",		NM_ME_LOSE_ENERGY},
	{"poff",		NM_ME_POFF},
	{"yellowspark",		NM_ME_BLOCKHIT},
	{"explosionarea",	NM_ME_EXPLOSION_AREA},
	{"explosion",		NM_ME_EXPLOSION_DAMAGE},
	{"firearea",		NM_ME_FIRE_AREA},
	{"yellowbubble",	NM_ME_YELLOW_RINGS},
	{"greenbubble",		NM_ME_POISON_RINGS},
	{"blackspark",		NM_ME_HIT_AREA},
	{"teleport",		NM_ME_TELEPORT},
	{"energy",		NM_ME_ENERGY_DAMAGE},
	{"blueshimmer",		NM_ME_MAGIC_ENERGY},
	{"redshimmer",		NM_ME_MAGIC_BLOOD},
	{"greenshimmer",	NM_ME_MAGIC_POISON},
	{"fire",		NM_ME_HITBY_FIRE},
	{"greenspark",		NM_ME_POISON},
	{"mortarea",		NM_ME_MORT_AREA},
	{"greennote",		NM_ME_SOUND_GREEN},
	{"rednote",		NM_ME_SOUND_RED},
	{"poison",		NM_ME_POISON_AREA},
	{"yellownote",		NM_ME_SOUND_YELLOW},
	{"purplenote",		NM_ME_SOUND_PURPLE},
	{"bluenote",		NM_ME_SOUND_BLUE},
	{"whitenote",		NM_ME_SOUND_WHITE},
	{"bubbles",		NM_ME_BUBBLES},
	{"dice",		NM_ME_CRAPS},
	{"giftwraps",		NM_ME_GIFT_WRAPS},
	{"yellowfirework",	NM_ME_FIREWORK_YELLOW},
	{"redfirework",		NM_ME_FIREWORK_RED},
	{"bluefirework",	NM_ME_FIREWORK_BLUE},
	{"stun",		NM_ME_STUN},
	{"sleep",		NM_ME_SLEEP},
	{"watercreature",	NM_ME_WATERCREATURE},
	{"groundshaker",	NM_ME_GROUNDSHAKER},
	{"hearts",		NM_ME_HEARTS},
	{"fireattack",		NM_ME_FIREATTACK},
	{"energyarea",		NM_ME_ENERGY_AREA},
	{"smallclouds",		NM_ME_SMALLCLOUDS},
	{"holydamage",		NM_ME_HOLYDAMAGE},
	{"bigclouds",		NM_ME_BIGCLOUDS},
	{"icearea",		NM_ME_ICEAREA},
	{"icetornado",		NM_ME_ICETORNADO},
	{"iceattack",		NM_ME_ICEATTACK},
	{"stones",		NM_ME_STONES},
	{"smallplants",		NM_ME_SMALLPLANTS},
	{"carniphila",		NM_ME_CARNIPHILA},
	{"purpleenergy",	NM_ME_PURPLEENERGY},
	{"yellowenergy",	NM_ME_YELLOWENERGY},
	{"holyarea",		NM_ME_HOLYAREA},
	{"bigplants",		NM_ME_BIGPLANTS},
	{"cake",		NM_ME_CAKE},
	{"giantice",		NM_ME_GIANTICE},
	{"watersplash",		NM_ME_WATERSPLASH},
	{"plantattack",		NM_ME_PLANTATTACK},
	{"tutorialarrow",	NM_ME_TUTORIALARROW},
	{"tutorialsquare",	NM_ME_TUTORIALSQUARE},
	{"mirrorhorizontal",	NM_ME_MIRRORHORIZONTAL},
	{"mirrorvertical",	NM_ME_MIRRORVERTICAL},
	{"skullhorizontal",	NM_ME_SKULLHORIZONTAL},
	{"skullvertical",	NM_ME_SKULLVERTICAL},
	{"assassin",		NM_ME_ASSASSIN},
	{"stepshorizontal",	NM_ME_STEPSHORIZONTAL},
	{"bloodysteps",		NM_ME_BLOODYSTEPS},
	{"stepsvertical",	NM_ME_STEPSVERTICAL},
	{"yalaharighost",	NM_ME_YALAHARIGHOST},
	{"bats",		NM_ME_BATS},
	{"smoke",		NM_ME_SMOKE},
	{"insects",		NM_ME_INSECTS},
	{"dragonhead",		NM_ME_DRAGONHEAD},
	{"orcshaman",		NM_ME_ORCSHAMAN},
	{"orcshamanfire",	NM_ME_ORCSHAMAN_FIRE},
	{"thunder",		NM_ME_THUNDER},
	{"ferumbras",		NM_ME_FERUMBRAS},
	{"confettihorizontal",	NM_ME_CONFETTI_HORIZONTAL},
	{"confettivertical",	NM_ME_CONFETTI_VERTICAL},
	{"blacksmoke",		NM_ME_BLACKSMOKE}
};

ShootTypeNames shootTypeNames[] = {
	{"spear",		NM_SHOOT_SPEAR},
	{"bolt",		NM_SHOOT_BOLT},
	{"arrow",		NM_SHOOT_ARROW},
	{"fire",		NM_SHOOT_FIRE},
	{"energy",		NM_SHOOT_ENERGY},
	{"poisonarrow",		NM_SHOOT_POISONARROW},
	{"burstarrow",		NM_SHOOT_BURSTARROW},
	{"throwingstar",	NM_SHOOT_THROWINGSTAR},
	{"throwingknife",	NM_SHOOT_THROWINGKNIFE},
	{"smallstone",		NM_SHOOT_SMALLSTONE},
	{"death",		NM_SHOOT_DEATH},
	{"largerock",		NM_SHOOT_LARGEROCK},
	{"snowball",		NM_SHOOT_SNOWBALL},
	{"powerbolt",		NM_SHOOT_POWERBOLT},
	{"poison",		NM_SHOOT_POISONFIELD},
	{"infernalbolt",	NM_SHOOT_INFERNALBOLT},
	{"huntingspear",	NM_SHOOT_HUNTINGSPEAR},
	{"enchantedspear",	NM_SHOOT_ENCHANTEDSPEAR},
	{"redstar",		NM_SHOOT_REDSTAR},
	{"greenstar",		NM_SHOOT_GREENSTAR},
	{"royalspear",		NM_SHOOT_ROYALSPEAR},
	{"sniperarrow",		NM_SHOOT_SNIPERARROW},
	{"onyxarrow",		NM_SHOOT_ONYXARROW},
	{"piercingbolt",	NM_SHOOT_PIERCINGBOLT},
	{"whirlwindsword",	NM_SHOOT_WHIRLWINDSWORD},
	{"whirlwindaxe",	NM_SHOOT_WHIRLWINDAXE},
	{"whirlwindclub",	NM_SHOOT_WHIRLWINDCLUB},
	{"etherealspear",	NM_SHOOT_ETHEREALSPEAR},
	{"ice",			NM_SHOOT_ICE},
	{"earth",		NM_SHOOT_EARTH},
	{"holy",		NM_SHOOT_HOLY},
	{"suddendeath",		NM_SHOOT_SUDDENDEATH},
	{"flasharrow",		NM_SHOOT_FLASHARROW},
	{"flammingarrow",	NM_SHOOT_FLAMMINGARROW},
	{"shiverarrow",		NM_SHOOT_SHIVERARROW},
	{"energyball",		NM_SHOOT_ENERGYBALL},
	{"smallice",		NM_SHOOT_SMALLICE},
	{"smallholy",		NM_SHOOT_SMALLHOLY},
	{"smallearth",		NM_SHOOT_SMALLEARTH},
	{"eartharrow",		NM_SHOOT_EARTHARROW},
	{"explosion",		NM_SHOOT_EXPLOSION},
	{"cake",		NM_SHOOT_CAKE},
	{"tarsalarrow",		NM_SHOOT_TARSALARROW},
	{"vortexbolt",		NM_SHOOT_VORTEXBOLT},
	{"prismaticbolt",	NM_SHOOT_PRISMATICBOLT},
	{"crystallinearrow",	NM_SHOOT_CRYSTALLINEARROW},
	{"drillbolt",		NM_SHOOT_DRILLBOLT},
	{"envenomedarrow",	NM_SHOOT_ENVENOMEDARROW}
};

CombatTypeNames combatTypeNames[] = {
	{"physical",		COMBAT_PHYSICALDAMAGE},
	{"energy",		COMBAT_ENERGYDAMAGE},
	{"earth",		COMBAT_EARTHDAMAGE},
	{"fire",		COMBAT_FIREDAMAGE},
	{"undefined",		COMBAT_UNDEFINEDDAMAGE},
	{"lifedrain",		COMBAT_LIFEDRAIN},
	{"manadrain",		COMBAT_MANADRAIN},
	{"healing",		COMBAT_HEALING},
	{"drown",		COMBAT_DROWNDAMAGE},
	{"ice",			COMBAT_ICEDAMAGE},
	{"holy",		COMBAT_HOLYDAMAGE},
	{"death",		COMBAT_DEATHDAMAGE}
};

AmmoTypeNames ammoTypeNames[] = {
	{"spear",		AMMO_SPEAR},
	{"bolt",		AMMO_BOLT},
	{"arrow",		AMMO_ARROW},
	{"poisonarrow",		AMMO_ARROW},
	{"burstarrow",		AMMO_ARROW},
	{"throwingstar",	AMMO_THROWINGSTAR},
	{"throwingknife",	AMMO_THROWINGKNIFE},
	{"smallstone",		AMMO_STONE},
	{"largerock",		AMMO_STONE},
	{"snowball",		AMMO_SNOWBALL},
	{"powerbolt",		AMMO_BOLT},
	{"infernalbolt",	AMMO_BOLT},
	{"huntingspear",	AMMO_SPEAR},
	{"enchantedspear",	AMMO_SPEAR},
	{"royalspear",		AMMO_SPEAR},
	{"sniperarrow",		AMMO_ARROW},
	{"onyxarrow",		AMMO_ARROW},
	{"piercingbolt",	AMMO_BOLT},
	{"etherealspear",	AMMO_SPEAR},
	{"flasharrow",		AMMO_ARROW},
	{"flammingarrow",	AMMO_ARROW},
	{"shiverarrow",		AMMO_ARROW},
	{"eartharrow",		AMMO_ARROW}
};

AmmoActionNames ammoActionNames[] = {
	{"move",		AMMOACTION_MOVE},
	{"moveback",		AMMOACTION_MOVEBACK},
	{"removecharge",	AMMOACTION_REMOVECHARGE},
	{"removecount",		AMMOACTION_REMOVECOUNT}
};

MagicEffectClasses getMagicEffect(const std::string& strValue)
{
	for (size_t i = 0; i < sizeof(magicEffectNames) / sizeof(MagicEffectNames); ++i) {
		if (strcasecmp(strValue.c_str(), magicEffectNames[i].name) == 0) {
			return magicEffectNames[i].effect;
		}
	}
	return NM_ME_UNK;
}

ShootType_t getShootType(const std::string& strValue)
{
	for (size_t i = 0, size = sizeof(shootTypeNames) / sizeof(ShootTypeNames); i < size; ++i) {
		if (strcasecmp(strValue.c_str(), shootTypeNames[i].name) == 0) {
			return shootTypeNames[i].shoot;
		}
	}
	return NM_SHOOT_UNK;
}

CombatType_t getCombatType(const std::string& strValue)
{
	for (size_t i = 0, size = sizeof(combatTypeNames) / sizeof(CombatTypeNames); i < size; ++i) {
		if (strcasecmp(strValue.c_str(), combatTypeNames[i].name) == 0) {
			return combatTypeNames[i].combat;
		}
	}
	return COMBAT_NONE;
}

std::string getCombatName(CombatType_t combatType)
{
	for (size_t i = 0, size = sizeof(combatTypeNames) / sizeof(CombatTypeNames); i < size; ++i) {
		if (combatTypeNames[i].combat == combatType) {
			return combatTypeNames[i].name;
		}
	}
	return "unknown";
}

Ammo_t getAmmoType(const std::string& strValue)
{
	for (size_t i = 0, size = sizeof(ammoTypeNames) / sizeof(AmmoTypeNames); i < size; ++i) {
		if (strcasecmp(strValue.c_str(), ammoTypeNames[i].name) == 0) {
			return ammoTypeNames[i].ammoType;
		}
	}
	return AMMO_NONE;
}

AmmoAction_t getAmmoAction(const std::string& strValue)
{
	for (size_t i = 0, size = sizeof(ammoActionNames) / sizeof(AmmoActionNames); i < size; ++i) {
		if (strcasecmp(strValue.c_str(), ammoActionNames[i].name) == 0) {
			return ammoActionNames[i].ammoAction;
		}
	}
	return AMMOACTION_NONE;
}

std::string getSkillName(uint16_t skillid)
{
	switch (skillid) {
		case SKILL_FIST:
			return "fist fighting";

		case SKILL_CLUB:
			return "club fighting";

		case SKILL_SWORD:
			return "sword fighting";

		case SKILL_AXE:
			return "axe fighting";

		case SKILL_DIST:
			return "distance fighting";

		case SKILL_SHIELD:
			return "shielding";

		case SKILL_FISH:
			return "fishing";

		case SKILL__MAGLEVEL:
			return "magic level";

		case SKILL__LEVEL:
			return "level";

		default:
			return "unknown";
	}
}

skills_t getSkillId(const std::string& param)
{
	if (param == "fist") {
		return SKILL_FIST;
	} else if (param == "club") {
		return SKILL_CLUB;
	} else if (param == "sword") {
		return SKILL_SWORD;
	} else if (param == "axe") {
		return SKILL_AXE;
	} else if (param == "distance" || param == "dist") {
		return SKILL_DIST;
	} else if (param == "shielding" || param == "shield") {
		return SKILL_SHIELD;
	} else if (param == "fishing" || param == "fish") {
		return SKILL_FISH;
	} else {
		return SKILL_FIST;
	}
}

uint32_t adlerChecksum(uint8_t* data, size_t length)
{
	if (length > NETWORKMESSAGE_MAXSIZE) {
		return 0;
	}

	const uint16_t adler = 65521;

	uint32_t a = 1, b = 0;

	while (length > 0) {
		size_t tmp = length > 5552 ? 5552 : length;
		length -= tmp;

		do {
			a += *data++;
			b += a;
		} while (--tmp);

		a %= adler;
		b %= adler;
	}

	return (b << 16) | a;
}

std::string ucfirst(std::string str)
{
	for (uint32_t i = 0; i < str.length(); ++i) {
		if (str[i] != ' ') {
			str[i] = toupper(str[i]);
			break;
		}
	}
	return str;
}

std::string ucwords(std::string str)
{
	size_t strLength = str.length();
	if (strLength == 0) {
		return str;
	}

	str[0] = toupper(str[0]);

	for (size_t i = 1; i < strLength; ++i) {
		if (str[i - 1] == ' ') {
			str[i] = toupper(str[i]);
		}
	}

	return str;
}

bool booleanString(const std::string& str)
{
	const std::string& lowerStr = asLowerCaseString(str);
	return (lowerStr == "yes" || lowerStr == "true" || lowerStr == "y" || atoi(lowerStr.c_str()) > 0);
}

std::string getWeaponName(WeaponType_t weaponType)
{
	switch (weaponType) {
		case WEAPON_SWORD:
			return "sword";
		case WEAPON_CLUB:
			return "club";
		case WEAPON_AXE:
			return "axe";
		case WEAPON_DIST:
			return "distance";
		case WEAPON_WAND:
			return "wand";
		case WEAPON_AMMO:
			return "ammunition";
		default:
			return "";
	}
}

uint32_t combatTypeToIndex(CombatType_t combatType)
{
	switch (combatType) {
		case COMBAT_NONE:
			return 0;
		case COMBAT_PHYSICALDAMAGE:
			return 1;
		case COMBAT_ENERGYDAMAGE:
			return 2;
		case COMBAT_EARTHDAMAGE:
			return 3;
		case COMBAT_FIREDAMAGE:
			return 4;
		case COMBAT_UNDEFINEDDAMAGE:
			return 5;
		case COMBAT_LIFEDRAIN:
			return 6;
		case COMBAT_MANADRAIN:
			return 7;
		case COMBAT_HEALING:
			return 8;
		case COMBAT_DROWNDAMAGE:
			return 9;
		case COMBAT_ICEDAMAGE:
			return 10;
		case COMBAT_HOLYDAMAGE:
			return 11;
		case COMBAT_DEATHDAMAGE:
			return 12;
		default:
			return 0;
	}
}

CombatType_t indexToCombatType(uint32_t v)
{
	if (v == 0) {
		return COMBAT_FIRST;
	}

	return (CombatType_t)(1 << (v - 1));
}

uint8_t serverFluidToClient(uint8_t serverFluid)
{
	uint8_t size = sizeof(clientToServerFluidMap) / sizeof(int8_t);

	for (uint8_t i = 0; i < size; ++i) {
		if (clientToServerFluidMap[i] == serverFluid) {
			return i;
		}
	}

	return 0;
}

uint8_t clientFluidToServer(uint8_t clientFluid)
{
	uint8_t size = sizeof(clientToServerFluidMap) / sizeof(int8_t);

	if (clientFluid >= size) {
		return 0;
	}

	return clientToServerFluidMap[clientFluid];
}

itemAttrTypes stringToItemAttribute(const std::string& str)
{
	if (str == "aid") {
		return ATTR_ITEM_ACTIONID;
	} else if (str == "uid") {
		return ATTR_ITEM_UNIQUEID;
	} else if (str == "description") {
		return ATTR_ITEM_DESC;
	} else if (str == "text") {
		return ATTR_ITEM_TEXT;
	} else if (str == "date") {
		return ATTR_ITEM_WRITTENDATE;
	} else if (str == "writer") {
		return ATTR_ITEM_WRITTENBY;
	} else if (str == "owner") {
		return ATTR_ITEM_OWNER;
	} else if (str == "duration") {
		return ATTR_ITEM_DURATION;
	} else if (str == "decaystate") {
		return ATTR_ITEM_DECAYING;
	} else if (str == "corpseowner") {
		return ATTR_ITEM_CORPSEOWNER;
	} else if (str == "charges") {
		return ATTR_ITEM_CHARGES;
	} else if (str == "fluidtype") {
		return ATTR_ITEM_FLUIDTYPE;
	} else if (str == "doorid") {
		return ATTR_ITEM_DOORID;
	}
	return ATTR_ITEM_NONE;
}

std::string getFirstLine(const std::string& str)
{
	std::string firstLine;
	for (size_t i = 0, strLength = str.length(); i < strLength; ++i) {
		if (str[i] == '\n') {
			break;
		}

		firstLine += str[i];
	}
	return firstLine;
}

double round(double v)
{
	if (v >= 0.0) {
		return std::floor(v + 0.5);
	} else {
		return std::ceil(v - 0.5);
	}
}