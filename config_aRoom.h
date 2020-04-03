#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "config_aRoom.h"

extern struct config config_i;

struct config {
	//
	size_t  floors_default;
	size_t rooms_default;
	unsigned int room_capacity_default;
	
	//
	double weight, money, knowledge, charm_v;//default condition
	double health, looks;
	int FOOD_WEIGHT, FOOD_COST,
		STUDY_WEIGHT,
		STUDY_KNOWLEDGE,
		WORK_MONEY, WORK_WEIGHT,
		//---------------------
		EXERCISE_WEIGHT, EXERCISE_HEALTH,
		MAKEUP_MONEY, MAKEUP_LOOKS;
	
	//
	std::string WEIGHT_INC_MALE, WEIGHT_INC_FEMALE, //weight
		MONEY_INC_MALE, MONEY_INC_FEMALE, //money
		KNOWLEDGE_INC, //knowledge
		HEALTH_INC, //health
		LOOKS_INC, //looks
		CHARM_INC_MALE, CHARM_INC_FEMALE;//charm
		 
	double love_threshold, lover_change_thereshold;
};

class aRoom {
public:
	aRoom() {
		population = 0;
		capacity = config_i.room_capacity_default;
		gender = -1;
	}

	unsigned int population;//住了几人
	unsigned int capacity; //可以住几人
	int gender;
	std::vector<std::string> name;
};