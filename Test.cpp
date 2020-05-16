#include "Test.h"

std::string TestNonVirtualBase::who_am_i()
{
	return "I am from the BASE class and I didn't put the keyword virtual. I'm a loser!";
}

std::string TestNonVirtualDerived::who_am_i()
{
	return "I am from the DERIVED class";
}

std::string TestVirtualBase::who_am_i()
{
	return "I am from the BASE class and I DID put the keyword virtual, good job!";
}

std::string TestVirtualDerived::who_am_i()
{
	return "I am from the DERIVED class. Yes I return that because I put the keyword virtual in the Base class. I'm smart!";
}

TestPassingByReferenceObject::TestPassingByReferenceObject(const std::string & name, double money)
	: _name(name), _money(money)
{
}

std::string TestPassingByReferenceObject::get_name() const
{
	return _name;
}

double TestPassingByReferenceObject::get_money()
{
	return _money;
}

void TestPassingByReferenceObject::set_name(const std::string & new_name)
{
	_name = new_name;
}

void TestPassingByReferenceObject::set_money(double new_money)
{
	_money = new_money;
}

void TestPassingByReferenceModifier::change_my_name_and_make_me_super_poor(TestPassingByReferenceObject & object)
{
	object.set_name("James Patagueule");
	object.set_money(0.);
}

void TestPassingByReferenceModifier::try_to_change_my_name_and_make_me_super_poor_if_you_can(const TestPassingByReferenceObject & object)
{
	//object.set_name("Vincent Times");
	//object.set_money(0.);
	// We cannot call the modifiers above
}

House::House(double square_meters, int number_of_rooms):
	_square_meters(square_meters), _number_of_rooms(number_of_rooms)
{
}

double House::get_square_meters() const
{
	return _square_meters;
}

// THIS IS AN EXAMPLE OF WHAT NOT TO DO !!! [the adresses will be the same, so the objects pointed to are the same in the memory]
Registry_Non_Clean::Registry_Non_Clean(const std::string & owner, const House & house)
	: _owner(owner), _house(&house)
{
}

const House& Registry_Non_Clean::get_house() const
{
	return *_house;
}


Registry_Clean::Registry_Clean(const std::string & owner, const House & house)
	: _owner(owner), _house(new House(house))
{
}

Registry_Clean::~Registry_Clean()
{
	delete _house;
}

Registry_Clean & Registry_Clean::operator=(const Registry_Clean & registry)
{
	if (this == &registry)
		return *this;
	else
	{
		delete _house;
		_house = new House(*registry._house);
		_owner = registry._owner;
	}
	return *this;
}

const House& Registry_Clean::get_house() const
{
	return *_house;
}

Registry_Clean::Registry_Clean(const Registry_Clean & registry)
	: _owner(registry._owner), _house(new House(*registry._house))
{}

Registry_Wannabe_Clean_But_Did_Worse::Registry_Wannabe_Clean_But_Did_Worse(const std::string & owner, const House & house)
	: _owner(owner), _house(&house)
{
}

Registry_Wannabe_Clean_But_Did_Worse::Registry_Wannabe_Clean_But_Did_Worse(const Registry_Wannabe_Clean_But_Did_Worse & registry)
	: _owner(registry._owner), _house(registry._house)
{
}

Registry_Wannabe_Clean_But_Did_Worse::~Registry_Wannabe_Clean_But_Did_Worse()
{
	delete _house;
}

Registry_Wannabe_Clean_But_Did_Worse & Registry_Wannabe_Clean_But_Did_Worse::operator=(const Registry_Wannabe_Clean_But_Did_Worse & registry)
{
	if (this == &registry) return *this;
	_owner = registry._owner;
	_house = registry._house;
	return *this;
}

const House & Registry_Wannabe_Clean_But_Did_Worse::get_house() const
{
	return *_house;
}
