#pragma once
#include <string>
#include <vector>
enum class DataType {
	Integer,
	Text,
	Number,
	Blob,
	DateTime
};


union Value {
	int intValue;
	std::string strValue;
	float floatValue;
}value;


struct TableColumn {
private:
	std::string name;
	////mutable Value value;
	DataType type;
public:
	
	TableColumn(DataType type, std::string name){
		this->type = type;
		this->name = name;
	}
	
	std::string getName() {
		return name;
	 }

	TableColumn& setValue(std::string value) {
	//	if (type == DataType::Text)
		//	this->value.strValue = value;
	///	return (*this);
	}
};


struct Table {


	std::vector<TableColumn> columns;


	bool isRegisterThisCol(TableColumn col) {
		for (auto it : columns) {
			if (it.getName() == col.getName())return true;
		}
		return false;
	}

	void RegisterColumn(TableColumn column) {
		if (isRegisterThisCol(column))return;
		columns.emplace_back(column);
	}

	TableColumn* getCol(std::string name) {
		for (auto it : columns)
			if (it.getName() == name)
				return &it;
		return nullptr;
	}
};


