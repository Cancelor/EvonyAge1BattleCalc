// BattleCalc.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "combatsimulator.h"
#include <array>
#include <iostream>

std::array<int32_t,5> CombatSimulator::meleeTroopTypes = { 0,1,2,3,4 };
std::array<int32_t,3> CombatSimulator::rangedTroopTypes = { 5,9,11 };
std::array<int32_t,3> CombatSimulator::mechTroopTypes = { 9,10,11 };
std::array<int32_t,2> CombatSimulator::mountedTroopTypes = { 7,8 };
std::array<int32_t,6> CombatSimulator::groundTroopTypes = { 0,1,2,3,4,5 };
int8_t CombatSimulator::troopTypes[12] = { true,true,true,true,true,false,true,true,true,false,true,false };
int32_t CombatSimulator::movementOrder[12]= { 2,7,8,3,4,1,0,6,10,5,9,11 };
int32_t CombatSimulator::nonRangedTroops[9]={ 7,8,3,4,1,0,6,2,10 };
troopStat CombatSimulator::baseStats[12] = {
	troopStat{ 100,5,50,180,10 }, // worker
	troopStat{ 200,50,50,200,20 }, // warrior
	troopStat{ 100,20,50,3000,20 }, // scout
	troopStat{ 300,150,150,300,50 }, // pike
	troopStat{ 350,100,250,275,30 }, // swords
	troopStat{ 250,120,50,250,1200 }, // archer
	troopStat{ 700,10,60,150,10 }, //transporter
	troopStat{ 500,250,180,1000,100 }, // cavalry
	troopStat{ 1000,350,350,750,80 }, // cataphract
	troopStat{ 320,450,160,100,1400 }, // ballista
	troopStat{ 5000,250,160,120,600 }, // ram
	troopStat{ 480,600,200,80,1500 } // catapult
};
troopStat CombatSimulator::baseFortificationStats[5] = {
	troopStat{}, // trap :: TODO
	troopStat{}, // abatis :: TODO
	troopStat{ 2000,300,360,0,1300 }, // archer tower
	troopStat{}, // rolling logs :: TODO
	troopStat{} // trebuchet :: TODO
};
float CombatSimulator::damageModifiers[12][12] = {
	{}, // modifiers for worker attacking other troops
	{}, // for warrior
	{}, // for scout
	{ 0,0,0,0,0,0,0,1.8,1.8 }, // for pike
	{ 0,0,0,1.1 }, // for swords
	{}, // for archer
	{}, // for transporter
	{ 0,0,0,0,0,1.2 }, // for cavs
	{ 0,0,0,0,0,1.2 }, // for cataphracts
	{}, // for ballista
	{}, // for ram
	{} // for catapult
};
void CombatSimulator::modifyStats(troopStat* base, researchStats res, heroStat hero, float atk_modifier, float def_modifier, float life_modifier) {
	for (int i = 0; i < 12; i++) {
		base[i].defense = max((float)(1000 - ((res.iron_working * 5 + 100)/100)*((100 + hero.intel)/100)*base[i].defense*def_modifier)/1000, 0.5);
	}
	for (int i : groundTroopTypes) {
		base[i].life = base[i].life*(res.medicine * 5 + 100)*life_modifier / 100;
		base[i].attack = base[i].attack*atk_modifier;
		base[i].speed = base[i].speed*(res.compass * 10 + 100) / 100;
	}
	for (int i : mechTroopTypes) {
		base[i].life = base[i].life*life_modifier;
		base[i].attack = base[i].attack*atk_modifier;
		base[i].speed = base[i].speed*(res.horseback_riding * 5 + 100) / 100;
	}
	for (int i : rangedTroopTypes) {
		base[i].range = base[i].range*(res.archery * 5 + 100) / 100;
	}
	for (int i : mountedTroopTypes) {
		base[i].life = base[i].life*life_modifier;
		base[i].attack = base[i].attack*atk_modifier;
		base[i].speed = base[i].speed*(res.horseback_riding * 5 + 100) / 100;
	}
}
int8_t CombatSimulator::compareSpeed(combatTroops& x, combatTroops& y) {
	if (x.stat->speed == y.stat->speed) {
		if (x.isAttacker) return false;
		else return true;
	}
	return (x.stat->speed > y.stat->speed);
}
// id for the fortifications are reduced by 14
void CombatSimulator::fight(attacker atk, defender def,battleResult* br) {
	troopStat attackerTroopStats[12];
	std::copy(baseStats, baseStats + 12, attackerTroopStats);
	troopStat defenderTroopStats[12];
	std::copy(baseStats, baseStats + 12, defenderTroopStats);
	modifyStats(attackerTroopStats, atk.research, atk.hero, atk.attack_modifier, atk.defence_modifier, atk.life_modifier);
	modifyStats(defenderTroopStats, def.research, def.hero, def.attack_modifier, def.defence_modifier, def.life_modifier);
	
	// modifying archer tower stats
	// TODO: other wall defenses
	troopStat defenderFortificationStats[5];
	std::copy(baseFortificationStats,baseFortificationStats+5,defenderFortificationStats);
	defenderFortificationStats[2].life = (def.research.medicine * 5 + 100)*defenderFortificationStats[2].life / 100;
	defenderFortificationStats[2].attack = defenderFortificationStats[2].attack*def.attack_modifier;
	defenderFortificationStats[2].defense = max(1 - (def.research.iron_working * 5 + 100)*(100 + def.hero.intel)*defenderFortificationStats[2].defense*def.defence_modifier / 10000000, 0.5);
	defenderFortificationStats[2].range = defenderFortificationStats[2].range*(def.research.archery * 5 + def.wallLevel*4.5 + 100) / 100;
	combatTroops combatTroopsArrayAtk[12];
	combatTroops combatTroopsArrayDef[12];
	// Calculating field size
	int field_size=0;
/*	for (int i : {}) {
		if (atk.troops[i]>0 || def.troops[i]>0) {
			field_size=max(field_size,baseStats[i].speed+baseStats[i].range);
		}
	}*/
	for (int i : {0, 1, 3, 4, 6, 5, 7, 8, 9, 10, 11}) {
		if (atk.troops[i] > 0 || def.troops[i] > 0) {
			field_size = max(field_size, 200 + baseStats[i].range);
		}
	}

#if _DEBUG
	std::cout << "Field size " << field_size << "\n";
#endif
	// Creating an array with attacking troops
    combatTroops* u;
	for (int i = 0; i < 12; i++) {
        u=&combatTroopsArrayAtk[i];
		u->count = atk.troops[i];
		u->location = 0;
		u->typeId = i;
		u->stat = &attackerTroopStats[i];
		u->isAttacker = true;
        u->effectiveTroops=atk.troops[i];
	}

	// Creating an array with defending troops
	for (int i = 0; i < 12; i++) {
        u=&combatTroopsArrayDef[i];
		u->count = def.troops[i];
		u->location = field_size;
		u->typeId = i;
		u->stat = &defenderTroopStats[i];
        u->effectiveTroops=def.troops[i];
	}

	int8_t atkerType;
	float atkValue;
	combatTroops* defenderTroop;
	float damageModifier;
	int8_t inRange;
	int64_t damage;
	int64_t damage1;
	int8_t attackerAlive;
	int8_t defenderAlive;
	int32_t maxRange;
	int round;
	for (round = 0; round < 100; ++round) {

		// move the troops according to the movement order
		for (int i : movementOrder) {
			combatTroops* pq = &combatTroopsArrayDef[i];

			// first moving the defender troop of that type
			if (i!=2 && i!=6 && pq->count > 0) {
				int32_t nearestPosition = 0;
				inRange=false;
				maxRange = pq->location - pq->stat->range;
				// checking if some troop is already in its range
				for (combatTroops& xp : combatTroopsArrayAtk) {
					if (xp.count > 0 ) {
						if (xp.location >= maxRange) {
							inRange=true;
							break;
						}
						nearestPosition=max(nearestPosition,xp.location);
					}
				}

				// if no troop is in range, move
				if (!inRange) {
					pq->location=max(nearestPosition,pq->location-pq->stat->speed);
//					else pq->location = max(min(pq->location,nearestPosition+pq->stat->range),pq->location-pq->stat->speed);
#if _DEBUG
					std::cout << "Defender troops of type " << pq->typeId << " moves to " << pq->location << "\n";
#endif
				}
			}
			
            if (i==2 || i==6) {
                bool scoutOnly=true;
                for (int i: {0,1,3,4,5,7,8,9,10,11}) {
                    if (combatTroopsArrayAtk[i].count>0) {
                        scoutOnly=false;
                        break;
                    }
                }
                if (!scoutOnly) continue;
            }
			pq = &combatTroopsArrayAtk[i];

			// next moving the attacker troop of that type
			if (pq->count > 0) {
				int32_t nearestPosition = field_size;
				inRange=false;
				maxRange = pq->location + pq->stat->range;
				// checking if some troop is already in its range
				for (combatTroops& xp : combatTroopsArrayDef) {
					if (xp.count > 0 ) {
						if (xp.location <= maxRange) {
							inRange=true;
							break;
						}
						nearestPosition=min(nearestPosition,xp.location);
					}
				}

				// if no troop is in range, move
				if (!inRange) {
					pq->location=min(nearestPosition,pq->location+pq->stat->speed);
//					else pq->location = min(max(pq->location,nearestPosition-pq->stat->range),pq->location+pq->stat->speed);
#if _DEBUG
					std::cout << "Attacker troops of type " << pq->typeId << " moves to " << pq->location << "\n";
#endif
				}
			}
		}

		// perform attacks according to the movement order
		for (int i : { 2, 7, 8, 3, 4, 5, 1, 0, 6, 10, 9, 11 }) {

            // first defender troop of that type attacks

			combatTroops* pq=&combatTroopsArrayDef[i];
            if (pq->count > 0) {

                // choose which troop to attack
                defenderTroop=nullptr;
                atkValue=0;
                maxRange=pq->location - pq->stat->range;
                int32_t mindistance=-1;
                if (troopTypes[i]) {
                    for (combatTroops& lp:combatTroopsArrayAtk) {
                        if (lp.effectiveTroops> 0 && lp.location>=maxRange) {
                            if (mindistance==-1||mindistance>(pq->location-lp.location)||(mindistance==(pq->location-lp.location)&&(lp.stat->attack*lp.effectiveTroops>atkValue))) {
                                defenderTroop=&lp;
                                atkValue=lp.stat->attack*lp.effectiveTroops;
                                mindistance=pq->location-lp.location;
                            }
                        }
                    }
                }
                else {
                    // checking if there is a ranged troop in range
                    for (int j : rangedTroopTypes) {
                        combatTroops& lp=combatTroopsArrayAtk[j];
                        if (lp.effectiveTroops>0) {
                            if (lp.location >= maxRange) {
                                if ((lp.stat->attack*lp.effectiveTroops)>atkValue) {
                                    atkValue=lp.stat->attack*lp.effectiveTroops;
                                    defenderTroop=&lp;
                                }
                            }
                        }
                    }

                    // checking for fastest melee troop if no ranged troop is in range
                    float bestSpeed=-1;
                    if (defenderTroop==nullptr) {
                        for (int j : nonRangedTroops) {
                            combatTroops& lp=combatTroopsArrayAtk[j];
                            if (lp.effectiveTroops> 0 && lp.location>=maxRange && lp.stat->speed>bestSpeed) {
                                defenderTroop=&lp;
                                bestSpeed=lp.stat->speed;
                            }
                        }
                    }
                }

                // attack if there is a troop to attack
                if (defenderTroop!=nullptr) {
                    int64_t killed = 0;
                    if (troopTypes[i]) {
                        damageModifier=damageModifiers[pq->typeId][defenderTroop->typeId];
                        if (damageModifier==0) damageModifier=1;
                        // calculating damage by the attacker
#if _DEBUG
                        std::cout << "floor " << damageModifier << "*" << pq->count << "*" << pq->stat->attack << "*" << defenderTroop->stat->defense << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=(float)(damageModifier*pq->count*pq->stat->attack*defenderTroop->stat->defense) / defenderTroop->stat->life;
                    }
                    else {
                        int64_t distance=pq->location-defenderTroop->location;
                        if (distance <= defenderTroop->stat->range && troopTypes[defenderTroop->typeId]) damageModifier=0.25;
                        else if (distance > ((pq->stat->range)/2)) damageModifier=0.5;
                        else damageModifier=1;
#if _DEBUG                        
                        std::cout << "ceil " << pq->stat->attack << "*" << defenderTroop->stat->defense << "*" << pq->count << "*" << damageModifier << "/" << defenderTroop->stat->life << "\n";
#endif                        
                        killed=ceil(pq->stat->attack*defenderTroop->stat->defense*pq->count*damageModifier / defenderTroop->stat->life);
                    }
#if _DEBUG
                    std::cout << "Defender troops of type " << pq->typeId << " kills " << killed << " troops of type " << defenderTroop->typeId << "\n";
#endif
                    if (killed>defenderTroop->effectiveTroops) defenderTroop->effectiveTroops=0;
                    else defenderTroop->effectiveTroops-=killed;
                }
            }

            // now doing the same for attacker troop of that type
			pq=&combatTroopsArrayAtk[i];
			if (pq->count>0) {
				// choose which troop to attack
				defenderTroop=nullptr;
				atkValue=0;
				maxRange=pq->location + pq->stat->range;
                int32_t mindistance=-1;

                // if attacker is a melee troop
                if (troopTypes[i]) {
                    for (int j=0;j<12;j++) {
                        combatTroops& lp=combatTroopsArrayDef[j];
                        if (lp.effectiveTroops> 0 && lp.location<=maxRange) {
                            if (mindistance==-1||mindistance>(lp.location-pq->location)||(mindistance==(lp.location-pq->location)&&(lp.stat->attack*lp.effectiveTroops>atkValue))) {
                                defenderTroop=&lp;
                                atkValue=lp.stat->attack*lp.effectiveTroops;
                                mindistance=lp.location-pq->location;
                            }
                        }
                    }
                }
                // if attacker is a ranged troop
                else {
                    // checking if there is a ranged troop in range
                    for (int j : rangedTroopTypes) {
                        combatTroops& lp=combatTroopsArrayDef[j];
                        if (lp.effectiveTroops>0) {
                            if (lp.location <= maxRange) {
                                if ((lp.stat->attack*lp.effectiveTroops)>atkValue) {
                                    atkValue=lp.stat->attack*lp.effectiveTroops;
                                    defenderTroop=&lp;
                                }
                            }
                        }
                    }

                    // checking for fastest melee troop if no ranged troop is in range
                    float bestSpeed=-1;
                    if (defenderTroop==nullptr) {
                        for (int j : nonRangedTroops) {
                            combatTroops& lp=combatTroopsArrayDef[j];
                            if (lp.effectiveTroops> 0 && lp.location<=maxRange && lp.stat->speed>bestSpeed) {
                                defenderTroop=&lp;
                                bestSpeed=lp.stat->speed;
                            }
                        }
                    }
                }

				// attack if there is a troop to attack
				if (defenderTroop!=nullptr) {
					int64_t killed=0;
                    if (troopTypes[i]) {
                        damageModifier=damageModifiers[pq->typeId][defenderTroop->typeId];
                        if (damageModifier==0) damageModifier=1;
                        // calculating damage by the attacker
#if _DEBUG
                        std::cout << "floor " << damageModifier << "*" << pq->count << "*" << pq->stat->attack << "*" << defenderTroop->stat->defense << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=(float)(damageModifier*pq->count*pq->stat->attack*defenderTroop->stat->defense) / defenderTroop->stat->life;
                    }
                    else {
                        int64_t distance=defenderTroop->location-pq->location;
                        if (distance <= defenderTroop->stat->range && troopTypes[defenderTroop->typeId]) damageModifier=0.25;
                        else if (distance > ((pq->stat->range)/2) ) damageModifier=0.5;
                        else damageModifier=1;
#if _DEBUG
                        std::cout << "ceil " << pq->stat->attack << "*" << defenderTroop->stat->defense << "*" << pq->count << "*" << damageModifier << "/" << defenderTroop->stat->life << "\n";
#endif
                        killed=ceil(pq->stat->attack*defenderTroop->stat->defense*pq->count*damageModifier / defenderTroop->stat->life);
                    }
#if _DEBUG
					std::cout << "Attacker troops of type " << pq->typeId << " kills " << killed << " troops of type " << defenderTroop->typeId << "\n";
#endif
					if (killed>defenderTroop->effectiveTroops) defenderTroop->effectiveTroops=0;
                    else defenderTroop->effectiveTroops-=killed;
				}
			}


		}


#if _DEBUG
		std::cout << "==== Round finished ====\n";
#endif

		// calculating total damage for all the troops and reducing troop count
		attackerAlive = false;
		defenderAlive = false;
		for (combatTroops& xp:combatTroopsArrayAtk) {
#if _DEBUG
			if (xp.effectiveTroops<xp.count) {
				std::cout << "Attacker troops of type " << xp.typeId << " killed total " << floor(xp.count-xp.effectiveTroops) << "\n";
            }
#endif
            xp.count=xp.effectiveTroops;
			if (xp.count>0) attackerAlive=true;
		}
		for (combatTroops& xp:combatTroopsArrayDef) {
#if _DEBUG
            if (xp.effectiveTroops<xp.count) {
                std::cout << "Defender troops of type " << xp.typeId << " killed total " << floor(xp.count-xp.effectiveTroops) << "\n";
            }
#endif
            xp.count=xp.effectiveTroops;
            if (xp.count>0) defenderAlive=true;
		}
#if _DEBUG
		std::cout << "==== ROUND " << round << " END ==== \n";
#endif
		if (!attackerAlive || !defenderAlive) break;
	}
	std::copy(atk.troops, atk.troops + 12, br->attackerTroops);
	std::copy(def.troops, def.troops + 12, br->defenderTroops);
	for (combatTroops& uh : combatTroopsArrayAtk) {
		br->attackerTroops[uh.typeId] = uh.count;
	}
	for (combatTroops& uh : combatTroopsArrayDef) {
		br->defenderTroops[uh.typeId] = uh.count;
	}
	br->result = attackerAlive | (defenderAlive << 1);
	br->totalRounds = min(round + 1,100);
#if _DEBUG
	std::cout << "==== Remaining Troops ====\n";
	for (combatTroops& hu : combatTroopsArrayAtk) {
		if (hu.count > 0) {
			std::cout << "Attacker troop of type " << hu.typeId << " count " << hu.count << "\n";
		}
	}
	for (combatTroops& hu : combatTroopsArrayDef) {
		if (hu.count > 0) {
			std::cout << "Defender troop of type " << hu.typeId << " count " << hu.count << "\n";
		}
	}
#endif
}