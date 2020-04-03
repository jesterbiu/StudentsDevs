  #include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
#include <iterator>
#include <iostream>
#include <string>
#include <list>
#include <map>
#include <sstream>
#include "exprtk.hpp"
#include "student.h"
#include "config_aRoom.h"

#pragma warning(disable : 4996) 


extern struct config config_i;
extern std::list<stu_h> stu_enrolled;
extern std::list<stu_h> stu_quit;
extern std::list<std::list<aRoom>> rooms;
extern std::map<std::string, std::list<stu_h>::iterator> stu_enrolled_index;
extern std::map<std::string, std::list<stu_h>::iterator> stu_quit_index;

int student::dorm_floor() const
{
	if (dorm_f == rooms.end())
		return 0;

	int i = 1;
	auto it = rooms.begin();
	for (; it != dorm_f; it++)
		i++;
	return i;
}

int student::dorm_roomnum() const
{
	if (dorm_floor() == 0)
		return 0;

	int i = 1;
	auto it = dorm_f->begin();
	for (; it != dorm_rn; it++)
		i++;
	return i;
} 

int student::move(const std::string& ins)
{

	//find number
	std::string::size_type pos;
	pos = ins.find("TO", 0);
	std::string dormf = ins.substr(pos + 3, pos + 4);
	std::string dormrn = ins.substr(pos + 6, pos + 7);


	//string->int trans
	unsigned int dorm_f_new, dorm_rn_new;
	std::stringstream ss0(dormf);
	std::stringstream ss1(dormrn);
	ss0 >> dorm_f_new;
	ss1 >> dorm_rn_new;

	if (rooms.size() < dorm_f_new)
	{
		std::cout << name() << "该楼层不存在！调宿舍失败。" << std::endl;
		return 0;
	}
	size_t i = 1, j = 1;
	auto it_f = rooms.begin();
	for (; i < dorm_f_new; i++)
		it_f++;
	auto it_r = it_f->begin();
	for (; j < dorm_rn_new && it_r != it_f->end(); j++)
		it_r++;
	if (it_r == it_f->end())
	{
		std::cout << name() << "该房间不存在！调宿舍失败。" << std::endl;
		return 0;
	}
	if (it_r->capacity
		== it_r->population) {
		std::cout << name() << "要转去的宿舍已满！调宿舍失败。" << std::endl;
		return 0;
	}
	if (it_r->population > 0 && it_r->gender != gen) 
	{
		std::cout << name() << "男女不可混住！调宿舍失败。" << std::endl;
		return 0;
	}
	//old room
	dorm_rn->population--;
	std::string x = Name;
	dorm_rn->name.erase(std::find_if(dorm_rn->name.begin(), dorm_rn->name.end(), [x](std::string n) { return x == n; }));
	//new room
	it_r->population++;
	it_r->name.push_back(Name);
	//write info
	dorm_f = it_f;
	dorm_rn = it_r;

	return 1;
}

int student::quit()
{
	std::string n = name();
	auto iter = std::find_if(stu_enrolled.begin(), stu_enrolled.end(), [n](stu_h x) -> bool { return x.name() == n; });

	//quit
	stu_quit.push_back(*iter);
	stu_quit_index[name()] = std::find_if(stu_quit.begin(), stu_quit.end(), [n](stu_h x) { return x.name() == n; });

	//enrolled
	stu_enrolled_index.erase(n);
	stu_enrolled.erase(iter);
	return 1;
}

int student::relate_with(const std::string NaMe)
{
	if (!NaMe.empty())
		curr_lover = NaMe;
	return 0;
}

int student::breakup(const std::string NaMe)
{
	//if we are a couple
	if (lover() != NaMe)
	{
		std::cout << NaMe << "不是" << name() << "当前的恋人！" << std::endl;
		return 0;
	}

	//update ex_collection
	ex_collection[NaMe]++;

	//curr_lover.clear
	curr_lover.clear();

	return 0;
}

int student::court(const std::string NaMe)
{
	//if Im single
	if (!curr_lover.empty())
	{
		std::cout << name() << "不要见异思迁！" << std::endl;
		return 0;
	}

	//if that student is enrolled
	auto lv = stu_enrolled_index.find(NaMe);
	if (lv == stu_enrolled_index.end())
	{
		std::cout << NaMe << "未入学！" << std::endl;
		return 0;
	}

	//if that student is single
	if (lv->second->lover().empty())//single:deltaOrder
	{
		//delta
		double lv_thrs = std::abs(lv->second->charm() - charm()) / (std::max(lv->second->charm(), charm()));
		if (lv_thrs >= config_i.love_threshold)//fail
		{
			std::cout << name() << "追求" << NaMe << "失败！" << std::endl;
			return 0;
		}
		else//succeed
		{
			relate_with(NaMe);
			lv->second->relate_with(name());
		}
	}
	else//notsingle: delta2Order
	{
		//find his or her curr_lover
		auto Main = stu_enrolled_index.find(lv->second->lover());
		//delta2order
		double change_thrs = std::abs((Main->second->charm() - lv->second->charm()) - (Main->second->charm() - charm()))
			/ std::abs((Main->second->charm() - charm()));

		if (change_thrs >= config_i.lover_change_thereshold)//fail
		{
			std::cout << name() << "追求" << NaMe << "失败！" << std::endl;
			return 0;
		}
		else //succeed
		{
			lv->second->breakup(Main->second->name());
			lv->second->relate_with(name());
		}
	}

	return 0;
}

std::shared_ptr<student> male_stu::clone()
{ 
	auto p = std::make_shared<male_stu>(*this);
	
	std::shared_ptr<student> ptr = std::dynamic_pointer_cast<male_stu>(p);
	
	return ptr;
}

std::shared_ptr<student> female_stu::clone()
{
	auto p = std::make_shared<female_stu>(*this);
	std::shared_ptr<student> ptr = std::dynamic_pointer_cast<female_stu>(p);
	return ptr;
}

void male_stu::read_info(const std::string& studentinfo)
{
	std::string::size_type i;
	//gender
	if ((i = studentinfo.find("男", 0)) != std::string::npos)
		gen = 1;
	else {
		gen = 0;
		i = studentinfo.find("女", 0);
	}	

	//name
	Name = studentinfo.substr(0, i-1);

	//default condition
	wei = config_i.weight;
	mon = config_i.money;
	kno = config_i.knowledge;
	cha = config_i.charm_v;
	hp = config_i.health;
	//dorm
	i = studentinfo.find('-', 0);
	std::stringstream ss0(studentinfo.substr(i - 2, i-1));
	std::stringstream ss1(studentinfo.substr(i + 1, studentinfo.size()-1));
	unsigned int d_f, d_rn;
	ss0 >> d_f;
	ss1 >> d_rn;
	
	//get the floor iterator
	unsigned int f = 1, r = 1;
	auto it_f = rooms.begin();
	for (; f < d_f && it_f != rooms.end(); f++)
		it_f++;
		
	//get the room iterator
	auto it_rn = it_f->begin();
	for (; r < d_rn && it_rn != it_f->end(); r++)
		it_rn++;

	//check
	if (it_f == rooms.end())
	{
		std::cout << Name << ": ";
		std::cout << "该楼层不存在，入住失败！\n";
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	if(it_rn == it_f->end())
	{
		std::cout << Name << ": ";
		std::cout << "该房间不存在，入住失败！\n";
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}

	if (it_rn->population == it_rn->capacity)
	{
		std::cout << name() << ":分配的宿舍床位已满，入住失败！" << std::endl;
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	else if (it_rn->population > 0 && it_rn->gender != gender())
	{
		std::cout << name() << ":男女不可混住，入住失败！" << std::endl;
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	else 
	{ 
		dorm_f = it_f;
		dorm_rn = it_rn;
		it_rn->gender = gender();
		it_rn->population++;
		it_rn->name.push_back(Name);
	}

	return;
}

void female_stu::read_info(const std::string& studentinfo)
{
	std::string::size_type i;
	//gender
	if ((i = studentinfo.find("男", 0)) != std::string::npos)
		gen = 1;
	else {
		gen = 0;
		i = studentinfo.find("女", 0);
	}

	//name
	Name = studentinfo.substr(0, i - 1);

	//default condition
	wei = config_i.weight;
	mon = config_i.money;
	kno = config_i.knowledge;
	cha = config_i.charm_v;
	lk = config_i.looks;

	//dorm
	i = studentinfo.find('-', 0);
	std::stringstream ss0(studentinfo.substr(i - 2, i - 1));
	std::stringstream ss1(studentinfo.substr(i + 1, studentinfo.size() - 1));
	unsigned int d_f, d_rn;
	ss0 >> d_f;
	ss1 >> d_rn;

	//get the floor iterator
	unsigned int f = 1, r = 1;
	auto it_f = rooms.begin();
	for (; f < d_f && it_f != rooms.end(); f++)
		it_f++;

	//get the room iterator
	auto it_rn = it_f->begin();
	for (; r < d_rn && it_rn != it_f->end(); r++)
		it_rn++;

	//check
	if (it_f == rooms.end())
	{
		std::cout << Name << ": ";
		std::cout << "该楼层不存在，入住失败！\n";
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	if (it_rn == it_f->end())
	{
		std::cout << Name << ": ";
		std::cout << "该房间不存在，入住失败！\n";
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}

	if (it_rn->population == it_rn->capacity)
	{
		std::cout << name() << ":分配的宿舍床位已满，入住失败！" << std::endl;
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	else if (it_rn->population > 0 && it_rn->gender != gender())
	{
		std::cout << name() << ":男女不可混住，入住失败！" << std::endl;
		dorm_f = rooms.end();
		dorm_rn = it_f->end();
		return;
	}
	else
	{
		dorm_f = it_f;
		dorm_rn = it_rn;
		it_rn->gender = gender();
		it_rn->population++;
		it_rn->name.push_back(Name);
	}

	return;
}

int male_stu::charm_inc(const std::string& ins)//exprth.hpp
{
	std::string::size_type p0, p1;//for string::find
	std::string val;
	
	
	//eat
	if ( (p0 = ins.find("EAT", 0)) != std::string::npos ) {
		//get food_weight
		p1 = ins.find(' ', p0 + 4);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 3, p1 - p0 - 3);
		else
			val = ins.substr(p0 + 4, 3);
		std::stringstream ss (val);
		int food_weight;
		ss >> food_weight;

		//check if food_weight is legit
		if (food_weight < 0) {
			std::cout << "非法food_weight!" << std::endl;
			return 0;
		}
		else {
			charm_calculator(food_weight, "EAT");
		}		
	}//end of eat
	

	//work	
	if ((p0 = ins.find("WORK", 0)) != std::string::npos){
		//get work_time
		p1 = ins.find(' ', p0 + 5);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 4, p1);
		else
			val = ins.substr(p0 + 4);
		std::stringstream ss(val);
		int work_time;
		ss >> work_time;

		//if work_time is legit
		if (work_time < 0) {
			std::cout << "打工时间为负！" << std::endl;
			return 0;
		}
		else if (work_time > 100) {
			std::cout << "超时工作！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(work_time, "WORK");
		}
	}//end of work 
	
	//exercise
	if ((p0 = ins.find("EXERCISE", 0)) != std::string::npos) {
		p1 = ins.find(' ', p0 + 9);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 9, 3);
		else
			val = ins.substr(p0 + 9);
		std::stringstream ss(val);
		int exercise_time;
		ss >> exercise_time;

		//if studytime is legit		
		if (exercise_time < 0) {
			std::cout << "锻炼时间为负！" << std::endl;
			return 0;
		}
		else if (exercise_time > 100) {
			std::cout << "过度锻炼！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(exercise_time, "EXERCISE");
		}
		
	}

	//study
	if ((p0 = ins.find("STUDY", 0)) != std::string::npos) {
		//get study_time
		p1 = ins.find(' ', p0 + 6);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 6, 3);
		else
			val = ins.substr(p0 + 6);
		std::stringstream ss(val);
		int study_time;
		ss >> study_time;
	
		//if studytime is legit		
		if (study_time < 0) {
			std::cout << "学习时间为负！" << std::endl;
			return 0;
		}
		else if (study_time > 100) {
			std::cout << "超时学习！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(study_time, "STUDY");
		}

		
	}
	
	/*{
		std::cout << " 体重：" << weight()
			<< "; 存款：" << money()
			<< "; 知识：" << knowledge()
			<< "; 健康：" << spec()
			<< "; 魅力：" << charm() << std::endl;
	}*/

	return 1;
}

int female_stu::charm_inc(const std::string& ins)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               //exprth.hpp
{
	std::string::size_type p0, p1;//for string::find
	std::string val;

	//eat
	if ((p0 = ins.find("EAT", 0)) != std::string::npos) {
		//get food_weight
		p1 = ins.find(' ', p0 + 4);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 3, p1 - p0 - 3);
		else
			val = ins.substr(p0 + 4, 3);

		std::stringstream ss(val);
		int food_weight;
		ss >> food_weight;

		//check if food_weight is legit
		if (food_weight < 0) {
			std::cout << "非法food_weight!" << std::endl;
			return 0;
		}
		else {
			charm_calculator(food_weight, "EAT");
		}
	}//end of eat
	
	 //work	
	if ((p0 = ins.find("WORK", 0)) != std::string::npos) {
		//get work_time
		p1 = ins.find(' ', p0 + 5);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 4, p1);
		else
			val = ins.substr(p0 + 4);
		std::stringstream ss(val);
		int work_time;
		ss >> work_time;


		//if work_time is legit
		if (work_time < 0) {
			std::cout << "打工时间为负！" << std::endl;
			return 0;
		}
		else if (work_time > 100) {
			std::cout << "超时工作！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(work_time, "WORK");
		}
	}//end of work 

	 //makeup
	if ((p0 = ins.find("MAKEUP", 0)) != std::string::npos) {
		p1 = ins.find(' ', p0 + 7);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 7, 3);
		else
			val = ins.substr(p0 + 7);
		std::stringstream ss(val);
		int makeup_time;
		ss >> makeup_time;

		//std::cout << "makeup_time:" << makeup_time << "\n";

		//if studytime is legit		
		if (makeup_time < 0) {
			std::cout << "化妆时间为负！" << std::endl;
			return 0;
		}
		else if (makeup_time > 100) {
			std::cout << "过度化妆！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(makeup_time, "MAKEUP");
		}

	}

	//study
	if ((p0 = ins.find("STUDY", 0)) != std::string::npos) {
		//get study_time
		p1 = ins.find(' ', p0 + 6);
		if (p1 != std::string::npos)
			val = ins.substr(p0 + 6, 3);
		else
			val = ins.substr(p0 + 6);
		std::stringstream ss(val);
		int study_time;
		ss >> study_time;

		//if studytime is legit		
		if (study_time < 0) {
			std::cout << "学习时间为负！" << std::endl;
			return 0;
		}
		else if (study_time > 100) {
			std::cout << "超时学习！" << std::endl;
			return 0;
		}
		else {
			charm_calculator(study_time, "STUDY");
		}

	}
	
	return 1;
}

void male_stu::charm_calculator(int val, const std::string ins)
{
	typedef exprtk::symbol_table<double> symbol_table_t;
	typedef exprtk::expression<double>     expression_t;
	typedef exprtk::parser<double>             parser_t;

	//expression0
	std::string weight = config_i.WEIGHT_INC_MALE;
	std::string knowledge = config_i.KNOWLEDGE_INC;
	std::string money = config_i.MONEY_INC_MALE;
	std::string charm = config_i.CHARM_INC_MALE;
	std::string health = config_i.HEALTH_INC;
	
	//var
	double food_weight = config_i.FOOD_WEIGHT, food_cost = config_i.FOOD_COST, //eat
		work_money = config_i.WORK_MONEY, work_weight = config_i.WORK_WEIGHT, //work
		study_weight = config_i.STUDY_WEIGHT, study_knowledge = config_i.STUDY_KNOWLEDGE, //study
		exercise_weight = config_i.EXERCISE_WEIGHT, exercise_health = config_i.EXERCISE_HEALTH; //exercise
	double food_taken = 0, study_time = 0, work_time = 0, exercise_time = 0;
	double knowledge_inc, weight_inc, health_inc;

	symbol_table_t symbol_table;
	//increasement per unit
	symbol_table.add_variable("FOOD_WEIGHT", food_weight);
	symbol_table.add_variable("FOOD_COST", food_cost);
	symbol_table.add_variable("WORK_WEIGHT", work_weight);
	symbol_table.add_variable("WORK_MONEY", work_money);
	symbol_table.add_variable("STUDY_WEIGHT", study_weight);
	symbol_table.add_variable("STUDY_KNOWLEDGE", study_knowledge);
	symbol_table.add_variable("EXERCISE_HEALTH", exercise_health);
	symbol_table.add_variable("EXERCISE_WEIGHT", exercise_weight);

	//amount
	symbol_table.add_variable("WORK_TIME", work_time);
	symbol_table.add_variable("STUDY_TIME", study_time);
	symbol_table.add_variable("FOOD_TAKEN", food_taken);
	symbol_table.add_variable("EXERCISE_TIME", exercise_time);

	//increasement
	symbol_table.add_variable("KNOWLEDGE_INC", knowledge_inc);
	symbol_table.add_variable("WEIGHT_INC_MALE", weight_inc);
	symbol_table.add_variable("HEALTH_INC", health_inc);

	symbol_table.add_constants();

	//expression_t
	expression_t weight_t, knowledge_t, money_t, health_t, charm_t;
	weight_t.register_symbol_table(symbol_table);
	knowledge_t.register_symbol_table(symbol_table);
	money_t.register_symbol_table(symbol_table);
	health_t.register_symbol_table(symbol_table);
	charm_t.register_symbol_table(symbol_table);


	//parser_t
	parser_t parser;
	parser.compile(weight, weight_t);
	parser.compile(knowledge, knowledge_t);
	parser.compile(money, money_t);
	parser.compile(health, health_t);


	//read val
	if (ins == "EAT")
		food_taken = val;
	else if (ins == "WORK")
		work_time = val;
	else if (ins == "STUDY")
		study_time = val;
	else if (ins == "EXERCISE")
		exercise_time = val;
	else
	{ }

	//wei_inc
	if (wei + weight_t.value() <= 0)
		std::cout << name() << "：该指令使得学生体重过低！" << std::endl;
	else
		wei += weight_t.value();

	//kno_inc
	if (kno + knowledge_t.value() > 100)
		kno = 100;
	else
		kno += knowledge_t.value();

	//mon_inc
	mon += money_t.value();
	
	//health_inc
	if (hp + health_t.value() > 100)
		std::cout << name() << "：该指令使得学生太壮！" << std::endl;
	else
		hp += health_t.value();

	//charm_inc
	knowledge_inc = knowledge_t.value();
	weight_inc = weight_t.value();
	health_inc = health_t.value();
	parser.compile(charm, charm_t);

	cha += charm_t.value();
	if (cha > 100) 
		cha = 100;
	else if (cha < 0)
		cha = 0;
	else
	{ }
}

void female_stu::charm_calculator(int val, const std::string ins)
{
	typedef exprtk::symbol_table<double> symbol_table_t;
	typedef exprtk::expression<double>     expression_t;
	typedef exprtk::parser<double>             parser_t;

	//expression0
	std::string weight = config_i.WEIGHT_INC_FEMALE;
	std::string knowledge = config_i.KNOWLEDGE_INC;
	std::string money = config_i.MONEY_INC_FEMALE;
	std::string looks = config_i.LOOKS_INC;
	std::string charm = config_i.CHARM_INC_FEMALE;

	//var
	double food_weight = config_i.FOOD_WEIGHT, food_cost = config_i.FOOD_COST, //eat
		work_money = config_i.WORK_MONEY, work_weight = config_i.WORK_WEIGHT, //work
		study_weight = config_i.STUDY_WEIGHT, study_knowledge = config_i.STUDY_KNOWLEDGE, //study
		makeup_money = config_i.MAKEUP_MONEY, makeup_looks = config_i.MAKEUP_LOOKS; //makeup
	double food_taken = 0, study_time = 0, work_time = 0, makeup_time = 0;
	double knowledge_inc, weight_inc, looks_inc;

	symbol_table_t symbol_table;
	//increasement per unit
	symbol_table.add_variable("FOOD_WEIGHT", food_weight);
	symbol_table.add_variable("FOOD_COST", food_cost);
	symbol_table.add_variable("WORK_WEIGHT", work_weight);
	symbol_table.add_variable("WORK_MONEY", work_money);
	symbol_table.add_variable("STUDY_WEIGHT", study_weight);
	symbol_table.add_variable("STUDY_KNOWLEDGE", study_knowledge);
	symbol_table.add_variable("MAKEUP_MONEY", makeup_money);
	symbol_table.add_variable("MAKEUP_LOOKS", makeup_looks);

	//amount
	symbol_table.add_variable("WORK_TIME", work_time);
	symbol_table.add_variable("STUDY_TIME", study_time);
	symbol_table.add_variable("FOOD_TAKEN", food_taken);
	symbol_table.add_variable("MAKEUP_TIME", makeup_time);

	//increasement
	symbol_table.add_variable("KNOWLEDGE_INC", knowledge_inc);
	symbol_table.add_variable("WEIGHT_INC_FEMALE", weight_inc);
	symbol_table.add_variable("LOOKS_INC", looks_inc);

	symbol_table.add_constants();

	//expression_t
	expression_t weight_t, knowledge_t, money_t, looks_t, charm_t;
	weight_t.register_symbol_table(symbol_table);
	knowledge_t.register_symbol_table(symbol_table);
	money_t.register_symbol_table(symbol_table);
	looks_t.register_symbol_table(symbol_table);
	charm_t.register_symbol_table(symbol_table);


	//parser_t
	parser_t parser;
	parser.compile(weight, weight_t);
	parser.compile(knowledge, knowledge_t);
	parser.compile(money, money_t);
	parser.compile(looks, looks_t);


	//read val
	if (ins == "EAT")
		food_taken = val;
	else if (ins == "WORK")
		work_time = val;
	else if (ins == "STUDY")
		study_time = val;
	else if (ins == "MAKEUP")
		makeup_time = val;

	//wei_inc
	if (wei + weight_t.value() <= 0)
		std::cout << name() << "：该指令使得学生体重过低！" << std::endl;
	else
		wei += weight_t.value();

	//kno_inc
	if (kno + knowledge_t.value() > 100)
		kno = 100;
	else
		kno += knowledge_t.value();

	//mon_inc
	mon += money_t.value();

	//looks_inc
	if (lk + looks_t.value() > 100) {
		std::cout << name() << "：该指令化的妆太浓!" << std::endl;
		lk = 100;
	}
	
	else
		lk += looks_t.value();

	//charm_inc
	knowledge_inc = knowledge_t.value(), weight_inc = weight_t.value(), looks_inc = looks_t.value();
	parser.compile(charm, charm_t);

	cha += charm_t.value();
	if (cha > 100) {
		cha = 100;
	}
	else if (cha < 0)
		cha = 0;
	else {}
}

stu_h::stu_h(const std::string& info)
{
	
	if (info.find("男", 0) != std::string::npos) 
	{
		auto p = std::make_shared<male_stu>(info);
		ptr = std::dynamic_pointer_cast<male_stu>(p);
	}
		
	else if (info.find("女", 0) != std::string::npos)
	{
		auto p = std::make_shared<female_stu>(info);
		ptr = std::dynamic_pointer_cast<female_stu>(p);
	}
	else		
	{}
}