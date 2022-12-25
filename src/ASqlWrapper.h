#pragma once 
#include <string.h>
#include <stdio.h>

struct DBColumn {
	char name[128];
	int  datatype;
	char constraint[128];
	char value[512];
	DBColumn* next_column;
};

struct DBTable {
	DBColumn* columns;
	char name[128];
};


DBTable* create_table(char* name, bool add_primary_key_id);
DBColumn* create_column(const char* name, int type, const char* constraint);
DBTable* add_column(DBTable* table, const char* name, int type, const char* constraint);
char* generate_create_table_for_sqlite(DBTable* table);