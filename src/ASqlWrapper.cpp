
#include "ASqlWrapper.h"



DBColumn* create_column(const char* name, int type, const char* constraint)
{
	DBColumn* c = new DBColumn();
	c->next_column = NULL;
	strcpy(c->name, name);
	c->datatype = type;
	strcpy(c->constraint, constraint);
	return c;
}

DBTable* create_table(char* name, bool add_primary_key_id) {
	DBTable* db = new DBTable();
	db->columns = add_primary_key_id ? create_column("ID", 1, "PRIMARY KEY NOT NULL") : NULL;
	strcpy(db->name, name);
	return db;
}



DBTable* add_column(DBTable* table, const char* name, int type, const char* constraint) {
	DBColumn* column = create_column(name, 1, constraint);
	DBColumn* tmpColumn = table->columns;

	if (tmpColumn == NULL)
	{
		table->columns = column;
		return table;
	}

	while (tmpColumn->next_column != NULL)
		tmpColumn = tmpColumn->next_column;
	tmpColumn->next_column = column;
	return table;
}

int CountColumn(DBTable* table)
{
	int ret = 0;
	DBColumn* tmpColumn = table->columns;
	while (tmpColumn != NULL)
	{
		tmpColumn = tmpColumn->next_column;
		ret++;
	}

	return ret;
}

char* generate_create_table_for_sqlite(DBTable* table)
{
	char* sql = new char[2048];
	memset(sql, 0x00, 2048);
	int iter = 0;
	DBColumn* traverser = table->columns;
	sprintf(sql, "CREATE TABLE IF NOT EXISTS %s(", table->name);
	while (traverser)
	{
		char tmp[256] = { 0x00 };
		sprintf(tmp, "%s %s %s,", traverser->name, traverser->datatype == 1 ? "INT" : "Text", traverser->constraint);
		strcat(sql, tmp);
		traverser = traverser->next_column;
	}
	iter = strlen(sql);
	sql[iter - 1] = ')';
	return sql;
}

void add_value_to_column(DBTable* table, const char* column_name, const char* value)
{
	DBColumn* col = table->columns;
	while (col != NULL) {
		if (strcmp(col->name, column_name) == 0)
			break;
		col = col->next_column;
	}
	if (col)
	{
		switch (col->datatype)
		{
		case 1: strcpy(col->value, value);break;
		case 2: sprintf(col->value, "\"%s\"", value);break;
		}
	}
}
