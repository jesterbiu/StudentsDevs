#define SCL_SECURE_NO_WARNINGS
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <map>
#include "config_aRoom.h"
#include "instruction.h"
#include "student.h"
#include "inquiry.h"

using std::cin;			using std::cout;
using std::vector;		using std::endl;
using std::streamsize;	using std::ios;
using std::sort;		using std::string;
using std::ifstream;	using std::find_if;
using std::strstr;		using std::stringstream;
using std::list;		using std::map;

struct config config_i;
list<stu_h> stu_enrolled;
list<stu_h> stu_quit;
list<list<aRoom>> rooms;
map<string, list<stu_h>::iterator> stu_enrolled_index;
map<string, list<stu_h>::iterator> stu_quit_index;


void read_config(ifstream&);
void read_dispatch(ifstream&);
void read_instructions(ifstream&);
void print_config();
void print_rooms();
void print_students();
size_t maxlenth;



int main(int argc, char *argv[])
{
	//open file
	ifstream config_t(argv[1], ios::in);
	if (!config_t.is_open())
		cout << "failed to open " << argv[1] << endl;
	ifstream dispatch_t(argv[2], ios::in);
	if (!dispatch_t.is_open())
		cout << "failed to open " << argv[2] << endl;
	ifstream instructions_t(argv[3], ios::in);
	if (!instructions_t.is_open())
		cout << "failed to open " << argv[3] << endl;


	read_config(config_t);
	print_config();

	
	//build dorm	
	aRoom init_room;
	rooms.resize(config_i.floors_default);
	auto rooms_iter = rooms.begin();
	for (; rooms_iter != rooms.end(); rooms_iter++)//each floor
	{
		size_t r = 0;
		for (; r < config_i.rooms_default; r++)
			rooms_iter->push_back(init_room);
	}
	
	read_dispatch(dispatch_t);

	//build index	
	for (auto i = stu_enrolled.begin(); i != stu_enrolled.end(); i++)
		stu_enrolled_index[i->name()] = i;

	read_instructions(instructions_t);

	print_students();

    print_rooms();

	return  0;
}

void read_config(ifstream& config_t)
{
	string temp;
	while (!config_t.eof())
	{
		//config_t.clear();
		getline(config_t, temp);
		string::size_type pos;
		
		//dorm condition
		if (temp.find("FLOOR", 0) != string::npos) {
			pos = temp.find(':', 0);
			stringstream ss(temp.substr(pos + 1, temp.size()));
			ss >> config_i.floors_default;
			continue;
		}
		if (temp.find("ROOM", 0) != string::npos) {
			pos = temp.find(':', 0);
			stringstream ss(temp.substr(pos + 1, temp.size()));
			ss >> config_i.rooms_default;
			continue;
		}
		if (temp.find("CAPACITY", 0) != string::npos) {
			pos = temp.find(':', 0);
			stringstream ss(temp.substr(pos + 1, temp.size()));
			ss >> config_i.room_capacity_default;
			continue;
		}
		
		//student conditon
		if (temp.find("WEIGHT", 0) != string::npos) {
			if (temp.find("WORK_WEIGHT", 0) != string::npos) {}
			else if (temp.find("FOOD_WEIGHT", 0) != string::npos) {}
			else if (temp.find("STUDY_WEIGHT", 0) != string::npos) {}
			else if (temp.find("WEIGHT_INC", 0) != string::npos) {}
			else if (temp.find("EXERCISE_WEIGHT", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.weight;
				continue;
			}
		}
		if (temp.find("MONEY", 0) != string::npos) {
			if (temp.find("WORK_MONEY", 0) != string::npos) {}
			else if (temp.find("MONEY_INC", 0) != string::npos) {}
			else if (temp.find("MAKEUP_MONEY", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.money;
				continue;
			}
		}
		if (temp.find("KNOWLEDGE", 0) != string::npos) {
			if (temp.find("STUDY_KNOWLEDGE", 0) != string::npos) {}
			else if (temp.find("KNOWLEDGE_INC", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.knowledge;
				continue;
			}
		}
		if (temp.find("CHARM", 0) != string::npos) {
			if (temp.find("CHARM_INC", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.charm_v;
				continue;
			}
		}
		if (temp.find("LOOKS", 0) != string::npos) {
			if (temp.find("MAKEUP_LOOKS", 0) != string::npos) {}
			else if (temp.find("LOOKS_INC", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.looks;
				continue;
			}
		}
		if (temp.find("HEALTH", 0) != string::npos && temp.find("_INC",0) == string::npos) {
			if (temp.find("EXERCISE_HEALTH", 0) != string::npos) {}
			else if (temp.find("HEALTH_INC", 0) != string::npos) {}
			else {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.health;
				continue;
			}
		}

		//charm val
		if (temp.find("FOOD", 0) != string::npos) {
			if ((temp.find("FOOD_COST", 0) != string::npos)
				&& (temp.find("_INC",0) == string::npos))
			{
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.FOOD_COST;
			}
			else if (temp.find("FOOD_WEIGHT", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.FOOD_WEIGHT;
			}
			else {}
		}

		if (temp.find("STUDY", 0) != string::npos) {
			if (temp.find("STUDY_KNOWLEDGE", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.STUDY_KNOWLEDGE;
			}
			else if (temp.find("STUDY_WEIGHT", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.STUDY_WEIGHT;
			}
			else {}
		}
				
		if (temp.find("WORK", 0) != string::npos && temp.find("WEIGHT_INC", 0) == string::npos) {
			if ((temp.find("WORK_MONEY", 0) != string::npos)
				&& (temp.find("MONEY_INC") == string::npos)) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.WORK_MONEY;
				continue;
			}
			
			else if (temp.find("WORK_WEIGHT", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.WORK_WEIGHT;
				continue;
			}
			else {}
		}
		
		if (temp.find("EXERCISE", 0) != string::npos && temp.find("_INC", 0) == string::npos) {
			if (temp.find("EXERCISE_WEIGHT", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, 2));
				ss >> config_i.EXERCISE_WEIGHT;
				continue;
			}
			else if (temp.find("EXERCISE_HEALTH", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.EXERCISE_HEALTH;
				continue;
			}
			else {}
		}
		
		if (temp.find("MAKEUP", 0) != string::npos && temp.find("_INC", 0) == string::npos) {
			if (temp.find("MAKEUP_MONEY", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.MAKEUP_MONEY;
				continue;
			}
			else if (temp.find("MAKEUP_LOOKS", 0) != string::npos) {
				pos = temp.find(':', 0);
				stringstream ss(temp.substr(pos + 1, temp.size()));
				ss >> config_i.MAKEUP_LOOKS;
				continue;
			}
			else {}
		}
		
	
		//charm_inc
	
		if (temp.find("WEIGHT_INC_MALE = ", 0) != string::npos) 
		{
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.WEIGHT_INC_MALE = temp;
			continue;
		}
		//female
		else if (temp.find("WEIGHT_INC_FEMALE = ", 0) != string::npos) 
		{
			auto p = temp.find('=', 0);				
			temp = temp.substr(p + 2, temp.size());
			config_i.WEIGHT_INC_FEMALE = temp;
			continue;
		}
		else {}

		if ((temp.find("HEALTH_INC", 0) != string::npos) && 
			(temp.find("CHARM_INC", 0) == string::npos)) {
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.HEALTH_INC = temp;
			continue;
		}

		if ((temp.find("LOOKS_INC", 0) != string::npos) && 
			(temp.find("CHARM_INC", 0) == string::npos)) {
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.LOOKS_INC = temp;
			continue;
		}
			
		if (temp.find("MONEY_INC", 0) != string::npos){
			if (temp.find("MONEY_INC_FEMALE", 0) != string::npos) {
				auto p = temp.find('=', 0);
				temp = temp.substr(p + 2, temp.size());
				config_i.MONEY_INC_FEMALE = temp;
				continue;
			}
			//MONEY_INC (FOR MALE)
			else if (temp.find("MONEY_INC_MALE", 0) != string::npos)
			{
				auto p = temp.find('=', 0);
				temp = temp.substr(p + 2, temp.size());
				config_i.MONEY_INC_MALE = temp;
				continue;
			}
			else{}
		}
		
		if ((temp.find("KNOWLEDGE_INC", 0) != string::npos) && 
			(temp.find("CHARM_INC", 0) == string::npos))
		{
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.KNOWLEDGE_INC = temp;
			continue;
		}
			
		if (temp.find("CHARM_INC_MALE", 0) != string::npos) {
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.CHARM_INC_MALE = temp;
			continue;
		}

		if (temp.find("CHARM_INC_FEMALE", 0) != string::npos) {
			auto p = temp.find('=', 0);
			temp = temp.substr(p + 2, temp.size());
			config_i.CHARM_INC_FEMALE = temp;
			continue;
		}

		if (temp.find("LOVE_THRESHOLD", 0) != string::npos) {
			auto p = temp.find(':', 0);
			temp = temp.substr(p + 1, temp.size());
			stringstream ss(temp);
			ss >> config_i.love_threshold;
			continue;
		}

		if (temp.find("LOVER_CHANGE_THRESHOLD", 0) != string::npos) {
			auto p = temp.find(':', 0);
			temp = temp.substr(p + 1, temp.size());
			stringstream ss(temp);
			ss >> config_i.lover_change_thereshold;
			continue;
		}


		
			
	}
}

void read_dispatch(ifstream& dispatch_t)
{
	//dispatch_t.clear();
	string temp;
	while (!dispatch_t.eof())
	{		
		getline(dispatch_t, temp);
		if (temp.find("男", 0) != string::npos || temp.find("女", 0) != string::npos)
		{
			stu_h i(temp);
			stu_enrolled.push_back(i);
			maxlenth = std::max(i.name().size(), maxlenth);
		}		
	}
} 

bool all_space(const string& ins)
{
	for (auto ch : ins)
	{
		if (ch == ' ')
			continue;
		else
			return 0;
	}

	return 1;
}

void read_instructions(ifstream& instructions_t)
{	
	string temp;
	while (!instructions_t.eof())
	{
		getline(instructions_t, temp);
		if (temp.empty())
			continue;
		if (all_space(temp))
			continue;

		instruction(temp);
	}
}

void print_config()
{
	//print config_i----------------------------------
	cout << "---------dorms--------\n";
	cout << config_i.rooms_default << " floors, "
	<< config_i.floors_default << " rooms, "
	<<config_i.room_capacity_default << " beds." << endl;;

	cout << "---------stu_default_condition--------\n";
	cout << "weight:" << config_i.weight << "\n"
		<< "money:" << config_i.money << "\n"
		<< "knowledge:" << config_i.knowledge << "\n"
		<< "looks:" << config_i.looks << "\n"
		<< "health:" << config_i.health << "\n"
		<< "charm:" << config_i.charm_v << endl;

	cout << "---------activity--------\n";
	cout << "food_wight:" << config_i.FOOD_WEIGHT << "\n"
		<< "food_cost:" << config_i.FOOD_COST << " \n"
		<< "study_weight:" << config_i.STUDY_WEIGHT << "\n"
		<< "study_knowledge:" << config_i.STUDY_KNOWLEDGE << "\n"
		<< "work_money:" << config_i.WORK_MONEY << "\n"
		<< "makeup_looks:" << config_i.MAKEUP_LOOKS << "\n"
		<< "makeup_money:" << config_i.MAKEUP_MONEY << "\n"
		<< "exercise_health:" << config_i.EXERCISE_HEALTH << "\n"
		<< "exercise_weight:" << config_i.EXERCISE_WEIGHT << "\n"
		<< "work_weight:" << config_i.WORK_WEIGHT << endl;

	cout << "---------calculationrelated--------\n";
	cout << "weight_inc_male:" << config_i.WEIGHT_INC_MALE << endl;
	cout << "weight_inc_female:" << config_i.WEIGHT_INC_FEMALE << endl;
	cout << "money_inc_male:" << config_i.MONEY_INC_MALE << endl;	
	cout << "money_inc_female:" << config_i.MONEY_INC_FEMALE << endl;
	cout << "knowledge_inc:" << config_i.KNOWLEDGE_INC << endl;
	cout << "health_inc:" << config_i.HEALTH_INC << endl;
	cout << "looks_inc:" << config_i.LOOKS_INC << endl;


	cout << "charm_inc_female:" << config_i.CHARM_INC_FEMALE << endl;
	cout << "charm_inc_male:" << config_i.CHARM_INC_MALE << endl;

	cout << "---------loverelated--------\n";
	cout << "love_th:" << config_i.love_threshold << endl;
	cout << "lover_change_th:" << config_i.lover_change_thereshold << endl;
}

void print_rooms()
{
	cout << "\n-----------rooms-------------\n";
	cout << "共有" << rooms.size() << "层。\n";
	auto it_f = rooms.begin();
	size_t f = 0;
	//print room--------------------------------------
	for (; it_f != rooms.end(); it_f++)
	{
		cout << ++f << "层有" << it_f->size() << "个房间\n";
		
		size_t r = 0;
		auto it_r = it_f->begin();
		for (; it_r != it_f->end(); it_r++)
		{
			r++;

			if (f < 10)
				cout << "0" << f << "-";
			else
				cout << f << "-";
			if (r < 10)
				cout << "0" << r;
			else
				cout << r;

			cout << ": " << it_r->population << "/" << it_r->capacity;				
			if (it_r->population > 0)
			{
				if (it_r->gender == 1)
					cout << " 男宿";
				else if (it_r->gender == 0)
					cout << " 女宿";
			}
			else
				cout << " 未入住";		
				
			cout << endl;
		}
			
	}
}

void print_students()
{
	cout << "目前共有" << stu_enrolled.size() << "个学生在校；" << endl;
	size_t s = stu_enrolled.size() / 10;
	size_t c = 1;
	for (auto i = stu_enrolled.begin(); i != stu_enrolled.end(); i ++)
	{
		//name
		string space (maxlenth - i->name().size() + 1, ' ');
		cout << c << "." << string(s + 1 - c / 10, ' ') << i->name() << space;
		c++;

		//gender
		i->gender() == 1 ? cout << "男 " : cout << "女 ";

		//dorm
		if (i->dorm_floor() == 0 || i->dorm_roomnum() == 0) {
			cout << "分配宿舍失败！";
		}
		else
		{
			cout << "宿舍：";
			if (i->dorm_floor() < 10)
				cout << "0" << i->dorm_floor() << "-";
			else
				cout << i->dorm_floor() << "-";
			if (i->dorm_roomnum() < 10)
				cout << "0" << i->dorm_roomnum() << "   ";
			else
				cout << i->dorm_roomnum() << "   ";
		}

		//condition
		cout << "体重：" << i->weight()
			<< "; 存款：" << i->money()
			<< "; 知识：" << i->knowledge();
		if (i->gender() == 1)
			cout << "；健康：" << i->spec();
		else
			cout << "；容貌：" << i->spec();
		cout << "; 魅力：" << i->charm() << endl;
	}
}