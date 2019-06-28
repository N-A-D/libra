#pragma once

#include <string>

class Employee {
public:
	Employee()
		: id(s_id++), name("Employee #" + std::to_string(id)) {}

	int get_id() const noexcept { return id; }
	std::string get_name() const noexcept { return name; }
private:
	inline static int s_id = 0;
	int id;
	std::string name;
};

struct EmployeeCompare {

	using is_transparent = std::true_type;

	bool operator()(const Employee& lhs, const Employee& rhs) const noexcept {
		return lhs.get_id() < rhs.get_id();
	}
	bool operator()(const Employee& lhs, int id) const noexcept {
		return lhs.get_id() < id;
	}
	bool operator()(int id, const Employee& rhs) const noexcept {
		return id < rhs.get_id();
	}
};