// BattleTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../BattleCalc/combatsimulator.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

// the split function is written by Daisy
void split(std::vector<std::string> & toks, const std::string & src, const std::string & anyof)
{
	std::string target;
	std::string::size_type n, nxt, offset = 0;
	toks.clear();
	if (anyof.size() == 0)
		return;
	while (true)
	{
		n = src.find_first_not_of(anyof[0], offset);
		if (n != std::string::npos)//non-delim found
		{
			nxt = src.find_first_of(anyof[0], n + 1);
			if (nxt != std::string::npos)//found another delim
			{
				toks.push_back(src.substr(n, nxt - n));
				offset = nxt;
			}
			else//no delim found, end of content
			{
				toks.push_back(src.substr(n, nxt - n));
				break;
			}
		}
		else//nothing else found
			break;
	}
}
bool testData(const char* filename)
{
	std::ifstream file(filename);
	std::string atkstr;
	std::vector<std::string> all;
	std::vector<std::string> atk;
	std::vector<std::string> def;
	std::vector<std::string> atkl;
	std::vector<std::string> defl;
	std::vector<std::string> mn;
	int linecount = 0;
	while (std::getline(file,atkstr)) {
		++linecount;
		split(all, atkstr, "|");
		if (all.size() != 5) {
			std::cout << "Unable to read line " << linecount << " in file " << filename << " " << atkstr << "\n";
			continue;
		}
		split(atk, all[0], ",");
		split(def, all[1], ",");
		int rounds = atoi(all[2].c_str());
		split(atkl, all[3], ",");
		split(defl, all[4], ",");
		attacker x;
		defender y;
		for (std::string& s : atk) {
			split(mn, s, "_");
			x.troops[atoi(mn[0].c_str())] = atoi(mn[1].c_str());
		}
		for (std::string& s : def) {
			split(mn, s, "_");
			y.troops[atoi(mn[0].c_str())] = atoi(mn[1].c_str());
		}
		int atkloss[12] = {};
		int defloss[12] = {};
		for (std::string& s : atkl) {
			split(mn, s, "_");
			atkloss[atoi(mn[0].c_str())] = atoi(mn[1].c_str());
		}
		for (std::string& s : defl) {
			split(mn, s, "_");
			defloss[atoi(mn[0].c_str())] = atoi(mn[1].c_str());
		}
		battleResult z;
		CombatSimulator::fight(x, y, &z);
		bool ok = true;
		for (int i = 0; i < 12; i++) {
			if (z.attackerTroops[i] != (x.troops[i] - atkloss[i]) || z.defenderTroops[i] != (y.troops[i] - defloss[i])) {
				ok = false;
				break;
			}
		}
		if (z.totalRounds != rounds) ok = false;
		if (!ok) {
			std::cout << "Failed with troops: " << atkstr << "\n";
			return false;
		}
	}
	return true;
}

int main()
{
	attacker atk;
	defender def;
	atk.troops[7] = 20000;
	def.fortifications[1] = 10000;
	def.wallLevel = 1;
	battleResult z;
	CombatSimulator::fight(atk, def, &z);
	std::getchar();
	return 0;
	std::string tests[149] = {  "randomsample4","randomsample3","randomsample1","randomsample","general","archer vs archer", "archer vs pikemen", "archer vs militia", "archer vs peasants", "archer vs swordsmen", "archer vs lightCavalry", "archer vs heavyCavalry", "archer vs ballista", "archer vs batteringRam", "archer vs catapult", "archer vs scouter", "archer vs carriage", "pikemen vs archer", "pikemen vs pikemen", "pikemen vs militia", "pikemen vs peasants", "pikemen vs swordsmen", "pikemen vs lightCavalry", "pikemen vs heavyCavalry", "pikemen vs ballista", "pikemen vs batteringRam", "pikemen vs catapult", "pikemen vs scouter", "pikemen vs carriage", "militia vs archer", "militia vs pikemen", "militia vs militia", "militia vs peasants", "militia vs swordsmen", "militia vs lightCavalry", "militia vs heavyCavalry", "militia vs ballista", "militia vs batteringRam", "militia vs catapult", "militia vs scouter", "militia vs carriage", "peasants vs archer", "peasants vs pikemen", "peasants vs militia", "peasants vs peasants", "peasants vs swordsmen", "peasants vs lightCavalry", "peasants vs heavyCavalry", "peasants vs ballista", "peasants vs batteringRam", "peasants vs catapult", "peasants vs scouter", "peasants vs carriage", "swordsmen vs archer", "swordsmen vs pikemen", "swordsmen vs militia", "swordsmen vs peasants", "swordsmen vs swordsmen", "swordsmen vs lightCavalry", "swordsmen vs heavyCavalry", "swordsmen vs ballista", "swordsmen vs batteringRam", "swordsmen vs catapult", "swordsmen vs scouter", "swordsmen vs carriage", "lightCavalry vs archer", "lightCavalry vs pikemen", "lightCavalry vs militia", "lightCavalry vs peasants", "lightCavalry vs swordsmen", "lightCavalry vs lightCavalry", "lightCavalry vs heavyCavalry", "lightCavalry vs ballista", "lightCavalry vs batteringRam", "lightCavalry vs catapult", "lightCavalry vs scouter", "lightCavalry vs carriage", "heavyCavalry vs archer", "heavyCavalry vs pikemen", "heavyCavalry vs militia", "heavyCavalry vs peasants", "heavyCavalry vs swordsmen", "heavyCavalry vs lightCavalry", "heavyCavalry vs heavyCavalry", "heavyCavalry vs ballista", "heavyCavalry vs batteringRam", "heavyCavalry vs catapult", "heavyCavalry vs scouter", "heavyCavalry vs carriage", "ballista vs archer", "ballista vs pikemen", "ballista vs militia", "ballista vs peasants", "ballista vs swordsmen", "ballista vs lightCavalry", "ballista vs heavyCavalry", "ballista vs ballista", "ballista vs batteringRam", "ballista vs catapult", "ballista vs scouter", "ballista vs carriage", "batteringRam vs archer", "batteringRam vs pikemen", "batteringRam vs militia", "batteringRam vs peasants", "batteringRam vs swordsmen", "batteringRam vs lightCavalry", "batteringRam vs heavyCavalry", "batteringRam vs ballista", "batteringRam vs batteringRam", "batteringRam vs catapult", "batteringRam vs scouter", "batteringRam vs carriage", "catapult vs archer", "catapult vs pikemen", "catapult vs militia", "catapult vs peasants", "catapult vs swordsmen", "catapult vs lightCavalry", "catapult vs heavyCavalry", "catapult vs ballista", "catapult vs batteringRam", "catapult vs catapult", "catapult vs scouter", "catapult vs carriage", "scouter vs archer", "scouter vs pikemen", "scouter vs militia", "scouter vs peasants", "scouter vs swordsmen", "scouter vs lightCavalry", "scouter vs heavyCavalry", "scouter vs ballista", "scouter vs batteringRam", "scouter vs catapult", "scouter vs scouter", "scouter vs carriage", "carriage vs archer", "carriage vs pikemen", "carriage vs militia", "carriage vs peasants", "carriage vs swordsmen", "carriage vs lightCavalry", "carriage vs heavyCavalry", "carriage vs ballista", "carriage vs batteringRam", "carriage vs catapult", "carriage vs scouter", "carriage vs carriage" };
	std::string ll;
	for (std::string& u : tests) {
		ll = "testData\\" + u + ".txt";
		std::cout << "Testing " << u << "\n";
		if (!testData(ll.c_str())) {
#if _DEBUG
			break;
#endif
		}
		else std::cout << "Passed\n";
	}
	std::getchar();
}