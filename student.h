#pragma once

#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <map>
#include "config_aRoom.h"

class student {
public:
	friend class stu_h;

	//constructor
	//default constructor
	student() { wei = mon = kno = cha = 0; }
	student(const student& oth)
	{
		//student infos
		Name = oth.Name;
		//int gen;
		dorm_f = oth.dorm_f;
		dorm_rn = oth.dorm_rn;
		wei = oth.wei;
		mon = oth.mon;
		kno = oth.kno;
		cha = oth.cha;
		//love ralated
		ex_collection = oth.ex_collection;
		curr_lover = oth.curr_lover;
	}

	//destructor
	virtual ~student() {};
	
	//input
	virtual void read_info(const std::string&) = 0;
	virtual int charm_inc(const std::string&) = 0;//charm_inc

	int move(const std::string&);//move
	int quit();//quit

	int court(const std::string);
	int breakup(const std::string);
	int relate_with(const std::string);

	//visitors
	std::string name() const { return Name; }
	int dorm_floor() const;
	int dorm_roomnum() const;
	int gender() const { return gen; }
	double weight() const { return wei; }
	double money() const { return mon; }
	double knowledge() const { return kno; }
	double charm() const { return cha; }
	virtual double spec() const = 0;
	std::string lover() const { return curr_lover; }
	std::map<std::string, int> ex() const { return ex_collection; }

	
protected:
	
	//clone
	virtual std::shared_ptr<student> clone() = 0;

	//student infos
	std::string Name;
	int gen;
	std::list<std::list<aRoom>>::iterator dorm_f;
	std::list<aRoom>::iterator dorm_rn;
	
	double wei, mon, kno, cha;

	//charm cal
	virtual void charm_calculator(int, const std::string) = 0;	
	
	//love ralated
	std::map<std::string, int> ex_collection;
	std::string curr_lover;
	
};

class male_stu: public student
{
public:
	//construtor
	male_stu() { hp = config_i.health; }
	male_stu(const std::string& info) { read_info(info); }
	male_stu(const male_stu& oth) 
	{
		//student infos
		Name = oth.Name;
		gen = 1;
		dorm_f = oth.dorm_f;
		dorm_rn = oth.dorm_rn;
		wei = oth.wei;
		mon = oth.mon;
		kno = oth.kno;
		cha = oth.cha;
		hp = oth.hp; 
		//love ralated
		ex_collection = oth.ex_collection;
		curr_lover = oth.curr_lover; 		
	}
	
	//destructor
	~male_stu() {}

	//
	void read_info(const std::string&);
	int charm_inc(const std::string&);

	//visitor
	double spec() const { return hp; };

protected:
	//charm_inc
	void charm_calculator(int, const std::string);

	//clone
	std::shared_ptr<student> clone();

	//property
	double hp;
	
};

class female_stu : public student
{
public:
	//construtor
	female_stu() { lk = config_i.looks; }
	female_stu(const female_stu& oth)
	{
		//student infos
		Name = oth.Name;
		gen = 0;
		dorm_f = oth.dorm_f;
		dorm_rn = oth.dorm_rn;
		wei = oth.wei;
		mon = oth.mon;
		kno = oth.kno;
		cha = oth.cha;
		lk = oth.lk;
		//love ralated
		ex_collection = oth.ex_collection;
		curr_lover = oth.curr_lover;

	}
	female_stu(const std::string& info) { read_info(info); }

	//destructor
	~female_stu() {}

	//
	void read_info(const std::string&);
	int charm_inc(const std::string&);//charm_inc
	
	//visitor
	double spec() const { return lk; };	

protected:
	//charm_inc
	void charm_calculator(int, const std::string);

	//clone
	std::shared_ptr<student> clone();

	//property
	double lk;
};

class stu_h
{
public:
	//constructor
	stu_h() : ptr(nullptr) {};
	stu_h(const std::string&);
	stu_h(const stu_h& oth) { ptr = oth.ptr->clone(); 

	}
	stu_h& operator=(const stu_h& oth) { ptr = oth.ptr->clone(); 
	
		return *this; };

	//destructor
	~stu_h() {}

	int charm_inc(const std::string& ins) {	return ptr->charm_inc(ins); }
	int move(const std::string& ins) { return ptr->move(ins); }
	int quit() { return ptr->quit(); }

	int court(const std::string ins) { return ptr->court(ins);	}
	int breakup(const std::string ins) { return ptr->breakup(ins); }
	int relate_with(const std::string ins) { return ptr->relate_with(ins); }

	//visitors
	std::string name() const { return ptr->Name; }
	int dorm_floor() const { return ptr->dorm_floor(); }
	int dorm_roomnum() const { return ptr->dorm_roomnum(); }
	int gender() const { return ptr->gen; }
	double weight() const { return ptr->wei; }
	double money() const { return ptr->mon; }
	double knowledge() const { return ptr->kno; }
	double charm() const { return ptr->cha; }
	double spec() const { return ptr->spec(); }
	std::string lover() const { return ptr->curr_lover; }
	std::map<std::string, int> ex() const { return ptr->ex_collection; }


private:
	std::shared_ptr<student> ptr;
};




