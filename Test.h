#ifndef TEST_H
#define TEST_H
#include <string>


class TestNonVirtualBase
{
public:
	std::string who_am_i();
};

class TestNonVirtualDerived : public TestNonVirtualBase
{
public:
	std::string who_am_i();
};

class TestVirtualBase
{
public:
	std::string who_am_i();
};

class TestVirtualDerived : public TestVirtualBase
{
public:
	std::string who_am_i();
};

class TestPassingByReferenceObject
{
public:
	TestPassingByReferenceObject(const std::string& name, double money);

	// Those 2 methods are called "getters", they can allow to access the current state of my object data.
	// They don't modify the current object so they need to be declared const !
	std::string get_name() const;
	double get_money();

	// Those 2 methods are called "setters", they can modify my current object so they are non-const methods
	void set_name(const std::string& new_name);
	void set_money(double new_money);
private:
	std::string _name;
	double _money;
};

class TestPassingByReferenceModifier
{
public:
	void change_my_name_and_make_me_super_poor(TestPassingByReferenceObject& object);
	void try_to_change_my_name_and_make_me_super_poor_if_you_can(const TestPassingByReferenceObject& object);
};

class House
{
public:
	House(double square_meters, int number_of_rooms);
	double get_square_meters() const;
private:
	double _square_meters;
	int _number_of_rooms;
};

// THIS IS AN EXAMPLE OF WHAT NOT TO DO !!! [Because the destructor is not defined to delete the object of type "House" pointed by the pointer _house]
class Registry_Non_Clean
{
public:
	Registry_Non_Clean(const std::string& owner, const House& house);
	const House& get_house() const;
private:
	std::string _owner;
	const House* _house;
};

class Registry_Wannabe_Clean_But_Did_Worse
{
public:
	Registry_Wannabe_Clean_But_Did_Worse(const std::string& owner, const House& house);
	Registry_Wannabe_Clean_But_Did_Worse(const Registry_Wannabe_Clean_But_Did_Worse& registry);
	~Registry_Wannabe_Clean_But_Did_Worse();
	Registry_Wannabe_Clean_But_Did_Worse& operator=(const Registry_Wannabe_Clean_But_Did_Worse& registry);
	const House& get_house() const;
private:
	std::string _owner;
	const House* _house;
};

// This is the cleanest approach of handling pointers as private data members
class Registry_Clean
{
public:
	Registry_Clean(const std::string& owner, const House& house);
	Registry_Clean(const Registry_Clean& registry);
	~Registry_Clean();
	Registry_Clean& operator=(const Registry_Clean& registry);
	const House& get_house() const;
private:
	std::string _owner;
	const House* _house;
};

#endif