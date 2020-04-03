
#include <iostream>
#include <algorithm>
#include <string>
#include <iterator>
#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include <list>
#include <map>
#include "instruction.h"
#include "student.h"
#include "config_aRoom.h"


extern struct config config_i;
extern std::list<stu_h> stu_enrolled;
extern std::list<stu_h> stu_quit;
extern std::list<std::list<aRoom>> rooms;
extern std::map<std::string, std::list<stu_h>::iterator> stu_enrolled_index;
extern std::map<std::string, std::list<stu_h>::iterator> stu_quit_index;



void enrol(const std::string& ins)
{
	size_t i;
	//get name
	if ((i = ins.find("男", 0)) == std::string::npos)
		i = ins.find("女", 0);
	std::string name = ins.substr(0, i - 1);
	std::list<stu_h>::iterator it;

	//check if the student has enrolled
	if (std::find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) {return x.name() == name; })
		!= stu_enrolled.end())
		std::cout << name << "：不可重复入学！" << std::endl;
	
	//student who quit & re-enrols
	else if ((it = std::find_if(stu_quit.begin(), stu_quit.end(), [name](stu_h x) { return x.name() == name; })) 
			!= stu_quit.end())
	{
		//enrol
		stu_enrolled.push_back(*it);
		stu_enrolled_index[it->name()] = std::find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) { return x.name() == name; });
		//quit erase
		stu_quit_index.erase(it->name());
		stu_quit.erase(it);		
		
	}
	// new student
	else
	{
		size_t i = ins.find("ENROL", 0);
		std::string ins1 = ins;
		ins1.erase(i, 5);
		stu_h nu(ins1);
		stu_enrolled.push_back(nu);
		stu_enrolled_index[nu.name()] = std::find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) { return x.name() == name; });
	}/**/
}

void stu_instruction(const std::string& ins)//which stu_h
{
	std::string name = ins.substr(0, ins.find(9, 0));
	
	//check if the student is enrolled
	std::list<stu_h>::iterator it;
	
	if ((it = find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) {return x.name() == name; }))
		!= stu_enrolled.end())
		//it->test();
	{
		//if (it->gender() == 1)
		it->charm_inc(ins);
	}

		
	else
		std::cout << name << "未入学！" << std::endl;
}

void add_rooms(const std::string& ins) 
{
	size_t p0;
	std::string val;
	size_t room_inc;
	unsigned int floor, rm;
	
	//room_inc
	p0 = ins.find('D', 0);
	val = ins.substr(p0 + 2, 2);
	std::stringstream ss0 (val);
	ss0 >> room_inc;

	//position
	if (ins.find('-', 0) == std::string::npos)
	{
		p0 = ins.find("TO", 0);
		val = ins.substr(p0 + 3, 2);
		std::stringstream ss1(val);
		ss1 >> floor;
	}
	else
	{
		p0 = ins.find('-', 0);
		val = ins.substr(p0 + 1, 2);
		std::stringstream ss1(val);
		ss1 >> rm;
		val = ins.substr(p0 - 2, 2);
		std::stringstream ss2(val);
		ss2 >> floor;
	}
	

	//if the floor number is valid
	if (floor > rooms.size()) 
	{
		std::cout << "该楼层不存在！" << std::endl;
		return;
	}
		

	//get to the floor
	size_t i = 0, j = 0;
	auto it_f = rooms.begin();
	for (; i < floor; i++)
		it_f++;

	//if this floor is full
	if (it_f->size() == 99) {
		std::cout << floor << "层房间数已达99，不可再继续增加！" << std::endl;
		return;
	}
	
	//add
	aRoom init_room;
	//
	if (ins.find('-', 0) != std::string::npos)
	{ 
		if (rm > it_f->size())
		{
			std::cout << "该房间不存在！" << std::endl;
			return;
		}	
		auto it_r = it_f->begin();
		for (; j + 1 < rm; j++)
			it_r++;			
		if (room_inc + it_f->size() > 99)
			room_inc = 99 - it_f->size();
		it_f->insert(it_r, room_inc, init_room);
	}
	//else add to the end
	else
	{
		for (; j < room_inc; j++)
			it_f->push_back(init_room);
	}
}

void add_floor(const std::string& ins)
{
	size_t p0;
	std::string val;
	size_t floor_inc, floor;

	//floor_inc & floor
	p0 = ins.find("ADD", 0);
	val = ins.substr(p0 + 4, 2);
	std::stringstream ss0(val);
	ss0 >> floor_inc;
	if (ins.find("UNDER", 0) != std::string::npos) 
	{
		p0 = ins.find("UNDER", 0);
		val = ins.substr(p0 + 6, 2);
		std::stringstream ss1(val);
		ss1 >> floor;

		//if the floor number is valid
		if (floor > rooms.size())
		{
			std::cout << "该楼层不存在！" << std::endl;
			return;
		}
	}		
	
	//if the whole building is full
	if (rooms.size() == 99)
	{
		std::cout << "层数已达99，不可再继续增加！" << std::endl;
		return;
	}

	
	//initialize a "floor"
	aRoom init_room;
	std::list<aRoom> init_floor;
	for (size_t j = 0; j < config_i.rooms_default; j++)
		init_floor.push_back(init_room);
	
	//add floors
	//add in the middle
	if (ins.find("UNDER", 0) != std::string::npos)
	{		
		size_t i = 0;
		auto it_f = rooms.begin();
		//get to the floor
		for (; i + 1 < floor; i++)
			it_f++;
		
		//add
		if (floor_inc + rooms.size() > 99)
			floor_inc = 99 - rooms.size();
		rooms.insert(it_f, floor_inc, init_floor);
	}
	//add on the top
	else
	{
		//add to the top
		size_t i = 0;	
		while (rooms.size() != 99 && i++ < floor_inc)
			rooms.push_back(init_floor);	
	}

}

void add_room_capacity(const std::string& ins)
{
	std::string::size_type p0, p1;
	std::string val;
	size_t floor, room, bed_inc;
	
	//get bed_inc
	p0 = 4;
	p1 = ins.find(' ', 4);
	val = ins.substr(p0, p1);
	std::stringstream ss0(val);
	ss0 >> bed_inc;

	//get floor-room
	p0 = ins.find('T', 0);
	p1 = ins.find('-', 0);

	val = ins.substr(p0 + 2, p1);
	std::stringstream ss1(val);
	ss1 >> floor;

	val = ins.substr(p1 + 1, ins.size() - 1);
	std::stringstream ss2(val);
	ss2 >> room;
	
	//if the floor number is valid
	if (floor > rooms.size())
	{
		std::cout << "该楼层不存在！" << std::endl;
		return;
	}

	size_t i = 1, j = 1;
	auto it_f = rooms.begin();
	for (; i < floor; i++)
		it_f++;

	//if the room number is valid
	if (room > it_f->size())
	{
		std::cout << "该房间不存在！" << std::endl;
		return;
	}

	auto it_r = it_f->begin();
	for (; j < room; j++)
		it_r++;
	
	//if the capacity is already at max
	if (it_r->capacity == 99)
	{
		std::cout << "该房间床位数已达上限！" << std::endl;
	}

	//add beds
	if (bed_inc + it_r->capacity > 99)
		bed_inc = 99 - it_r->capacity;
	it_r->capacity += bed_inc;
}

void instruction(const std::string& ins)
{	
	//dorm  instruction	
	if (ins.find("ADD", 0) != std::string::npos) {
		if (ins.find("ROOM", 0) != std::string::npos)
			add_rooms(ins);		
		if (ins.find("FLOOR", 0) != std::string::npos && ins.find("ROOM", 0) == std::string::npos)
			add_floor(ins);		
		if (ins.find("BED", 0) != std::string::npos)
			add_room_capacity(ins);
	}


	//stu_h instruction
	if (ins.find("EAT", 0) != std::string::npos)
		stu_instruction(ins);
	else if (ins.find("STUDY", 0) != std::string::npos)
		stu_instruction(ins);
	else if (ins.find("WORK", 0) != std::string::npos)
		stu_instruction(ins);
	else if (ins.find("MAKEUP", 0) != std::string::npos)
		stu_instruction(ins);
	
		
	else if (ins.find("EXERCISE", 0) != std::string::npos)
		stu_instruction(ins);
	else {}

	
	if (ins.find("MOVE", 0) != std::string::npos)
	{
		//get name
		std::string name;
		if (ins.find(9, 0) != std::string::npos)
			 name = ins.substr(0, ins.find(9, 0));
		else
			name = ins.substr(0, ins.find(' ', 0) - 1);
		
		//move
		std::list<stu_h>::iterator it;
		if ( (it = find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) {return x.name() == name; }))//check if the stu_h has enrolled
			!= stu_enrolled.end())
			it->move(ins);
		else
			std::cout << name << "：未入学，调宿舍失败！" << std::endl;
		}

	if (ins.find("QUIT", 0) != std::string::npos)
	{
		//get name
		std::string name;
		if (ins.find(9, 0) != std::string::npos)
			name = ins.substr(0, ins.find(9, 0));
		else
			name = ins.substr(0, ins.find(' ', 0) - 1);
		
		//quit
		std::list<stu_h>::iterator it;
		if ((it = find_if(stu_enrolled.begin(), stu_enrolled.end(), [name](stu_h x) {return x.name() == name; }))//check if the stu_h has enrolled
			!= stu_enrolled.end())
			it->quit();
		else
			std::cout << name << "：未入学，无法退学！" << std::endl;
	}
	else
	{ }
	
	if (ins.find("ENROL", 0) != std::string::npos)//enrol
		enrol(ins);


	size_t pos;
	if ((pos = ins.find("COURT", 0)) != std::string::npos)
	{
		std::string name1 = ins.substr(0, pos - 1);
		std::string name2 = ins.substr(pos + 6, ins.size());
		if (stu_enrolled_index.find(name1) != stu_enrolled_index.end()
			&& stu_enrolled_index.find(name2) != stu_enrolled_index.end()) 
			stu_enrolled_index[name1]->court(name2);
		else
		{
			std::cout << "追求：";
			if (stu_enrolled_index.find(name1) == stu_enrolled_index.end())
				std::cout << name1 << "未入学；";
			if (stu_enrolled_index.find(name2) == stu_enrolled_index.end())
				std::cout << name2 << "未入学；";
			std::cout << "\n";
		}
			
	}
	
	if (ins.find("BREAK UP", 0) != std::string::npos)
	{
		pos = ins.find(" ", 0);
		std::string name1 = ins.substr(0, pos);
		std::string name2 = ins.substr(pos + 1, ins.find(" ", pos + 1) - pos - 1);
		if (stu_enrolled_index.find(name1) != stu_enrolled_index.end()
			&& stu_enrolled_index.find(name2) != stu_enrolled_index.end())
		{
			stu_enrolled_index[name1]->breakup(name2);
			stu_enrolled_index[name2]->breakup(name1);
		}
		else
		{
			std::cout << "分手：";
			if (stu_enrolled_index.find(name1) == stu_enrolled_index.end())
				std::cout << name1 << "未入学；";
			if (stu_enrolled_index.find(name2) == stu_enrolled_index.end())
				std::cout << name2 << "未入学；";
			std::cout << "\n";
		}
	}
}