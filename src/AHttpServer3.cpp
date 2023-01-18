#include <string>
#include <sstream>
#include <cstdint>
#include "HttpServer.h"
#include "HttpDefaults.h"
#include "ASqlWrapper.h"
#include "Utils.h"
#include "HOF.h"


typedef unsigned char ByteValue;


 


std::vector<ByteValue> transform_string_to_number(std::string value, bool prepend_length = false) {
	std::vector<ByteValue> ret;
	for (auto i = 1;i < value.size();i += 2)
	{
		std::stringstream stream;
		stream << value[i - 1] << value[i];
		std::cout << stream.str();
		ret.emplace_back((ByteValue)std::atol(stream.str().c_str()));
	}
	return ret;
}

std::string reverse_string(std::string org)
{
	std::stringstream stream;
	int i = org.length();
	while(--i>=0)
		stream << org[i];
	return stream.str();
}

std::string convert_byte_to_binary(ByteValue b)
{
	std::stringstream stream;
	for (auto it = 0;it <= 7;it++)
	{
		stream << ((b >> it) & 0x1 == 0x1) ? '1' : '0';
	}

	return reverse_string(stream.str());
}



std::string convert_byte_to_string(ByteValue value) {
	return convert_byte_to_binary(value);
}


void serve() {


	auto theServer = new HttpServer{ 9091 };
	theServer
		->Get("/favicon.ico", http_send_static_html_file_favicon)
		->Get("/file", http_send_static_file)
		->Get("/mp3", http_send_static_mp3_file)
		->Get("/horse.mp3", http_send_static_mp3_file)
		->Get("/html", http_send_static_html_file)
		->Get("/printer", http_send_default_printer)
		->Get("/guids", http_send_default_default_session_guids)
		->Start()
		->Listen();

}

#include "iso8583.h"

enum class FieldDT {
	Integer,
	Varying,
	Varchar,
	Boolean,
	Blob
};

enum class SqlStatus {
	Success,
	Error,
	Warning,
	InternalError
};

enum class Constraint {
	NotNull,
	PrimaryKey,
	ConditionalValues
};

class DBMSHandler {
public:
	virtual SqlStatus execute(std::string sql) = 0;
};
struct Field {
	std::string name;
	FieldDT dataType;
	std::vector<Constraint> constraints;
	Field(std::string name, FieldDT type) {
		this->name = name;
		this->dataType = type;
	}
};

class Table {

protected:
	std::string table_name;
	std::vector<Field> fields;
	std::string generate_create_sql_script() {
		std::stringstream stream;
		stream << "CREATE TABLE " << table_name << "(";
		for (auto it : fields) {
			stream << it.name << " " << "INTEGER" << " ";
			stream << ",";
		}
		std::string ret = stream.str();
		ret.pop_back();
		ret += ")";
		return ret;
	}
	DBMSHandler* dbmsHandler;
public:
	virtual SqlStatus register_data_of_table() = 0;
	Table(DBMSHandler* dbmsHandler) :dbmsHandler{dbmsHandler} {}
};



class UserTable : public Table {
public:
	UserTable(DBMSHandler *handler): Table(handler) {
		register_data_of_table();
	}
	  SqlStatus register_data_of_table() override{
		table_name = "user";
		fields.emplace_back(Field("ID", FieldDT::Integer));
		fields.emplace_back(Field("real_name", FieldDT::Varchar));
		fields.emplace_back(Field("nickname", FieldDT::Varchar));
		fields.emplace_back(Field("email", FieldDT::Varchar));
		return dbmsHandler->execute(generate_create_sql_script());
	}
};


int main() {
	 
	struct SqliteNorADBMSButSqlComplaint : public DBMSHandler {
		SqlStatus execute(std::string sql) {
			std::cout << " Pretending this works " << sql << "\n";
			return SqlStatus::Success;
		}
	}sqlite;
	UserTable table(&sqlite);
	for (;;);

	return 0;
}
