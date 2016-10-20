//
// Created by Artur Troian on 10/20/16.
//
#include <propertypp/property.hpp>
#include <exception>
#include <stdexcept>

#include <cstdlib>
#include <iostream>

namespace property {

int sqlite_property::select_exec_cb(void *ptr, int argc, char **argv, char **names)
{
	req_value *value = reinterpret_cast<req_value *>(ptr);

	value->found = true;

	if (argc == 2) {
		value->blob  = std::string(argv[0]);
		value->type  = (value_type)atoi(argv[1]);
		value->valid = true;
	} else {
		value->valid = false;
	}

	return 0;
}

const std::string sqlite_property::property_table_("property_table");

sqlite_property::sqlite_property(const std::string &db) :
	  property()
	, db_(NULL)
{
	std::string uri(db);

	if (sqlite3_open(db.c_str(), &db_) != SQLITE_OK) {
		std::string error ("Couldn't open database file");
		error += sqlite3_errmsg(db_);
		sqlite3_close(db_);
		throw std::runtime_error(error);
	}

//	sqlite3_extended_result_codes(db_, true);
	const char *sql = "CREATE TABLE IF NOT EXISTS property_table(key STRING PRIMARY KEY, value BLOB, type INTEGER);";

	if (sqlite3_exec(db_, sql, 0, 0, 0) != SQLITE_OK) {
		std::string error ("Couldn't create table");
		error += sqlite3_errmsg(db_);
		sqlite3_close(db_);
		throw std::runtime_error(error);
	}
}

sqlite_property::~sqlite_property()
{
	sqlite3_close(db_);
}

prop_status sqlite_property::get(const std::string &key, void *value, value_type type)
{
	std::string sql = "SELECT value, type FROM " + property_table_ + " WHERE key = \'" + key + "\'";
	int ret;
	char *errmsg = NULL;
	req_value rsp;

	ret = sqlite3_exec(db_, sql.c_str(), select_exec_cb, &rsp, &errmsg);

	if (ret == SQLITE_OK) {
		if (rsp.found) {
			if (type != rsp.type)
				return PROP_STATUS_INVALID_TYPE;
			else {
				switch (type) {
				case VALUE_TYPE_STRING: {
					std::string *val = reinterpret_cast<std::string *>(value);
					*val = rsp.blob;
					break;
				}
				case VALUE_TYPE_INT: {
					int32_t *val = reinterpret_cast<int32_t *>(value);
					*val = std::stoi(rsp.blob);
					break;
				}
				case VALUE_TYPE_INT64: {
					int64_t *val = reinterpret_cast<int64_t *>(value);
					*val = std::stoll(rsp.blob);
					break;
				}
				case VALUE_TYPE_FLOAT: {
					float *val = reinterpret_cast<float *>(value);
					*val = std::stof(rsp.blob);
					break;
				}
				case VALUE_TYPE_DOUBLE: {
					double *val = reinterpret_cast<double *>(value);
					*val = std::stod(rsp.blob);
					break;
				}
				case VALUE_TYPE_BOOL: {
					bool *val = reinterpret_cast<bool *>(value);

					if (rsp.blob.compare("0") == 0)
						*val = false;
					else
						*val = true;

					break;
				}
				}
			}
		} else {
			return PROP_STATUS_NOT_FOUND;
		}

	} else {
		sqlite3_free(errmsg);
		std::cout << "Error: " << ret << std::endl;
		return PROP_STATUS_UNKNOWN_ERROR;
	}

	return PROP_STATUS_OK;
}

prop_status sqlite_property::set(const std::string &key, void *val, value_type type, bool update)
{
	prop_status ret = PROP_STATUS_OK;

	std::string sql = "INSERT INTO " + property_table_ + "(key, value, type) values (?,?,?)";
	sqlite3_stmt *stmt;


	std::string value;

	switch (type) {
	case VALUE_TYPE_STRING: {
		value = *(reinterpret_cast<std::string *>(val));
		break;
	}
	case VALUE_TYPE_INT: {
		int32_t *v = reinterpret_cast<int32_t *>(val);
		value = std::to_string(*v);
		break;
	}
	case VALUE_TYPE_INT64: {
		int64_t *v = reinterpret_cast<int64_t *>(val);
		value = std::to_string(*v);
		break;
	}
	case VALUE_TYPE_FLOAT: {
		float *v = reinterpret_cast<float *>(val);
		value = std::to_string(*v);
		break;
	}
	case VALUE_TYPE_DOUBLE: {
		double *v = reinterpret_cast<double *>(val);
		value = std::to_string(*v);
		break;
	}
	case VALUE_TYPE_BOOL: {
		bool *v = reinterpret_cast<bool *>(val);
		value = (*v) == true ? "1" : "0";
		break;
	}
	default:
		break;
	}

	int rc = sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &stmt, NULL);

	if (rc == SQLITE_OK) {
		// bind values

		sqlite3_bind_text(stmt, 1, key.c_str(), key.size(), 0);
		sqlite3_bind_blob(stmt, 2, value.c_str(), value.size(), NULL);
		sqlite3_bind_int(stmt, 3, (int)type);

		// commit
		sqlite3_step(stmt);
		rc = sqlite3_finalize(stmt);
		if (rc != SQLITE_OK) {
			if (rc == SQLITE_CONSTRAINT) {
				ret = PROP_STATUS_ALREADY_EXISTS;
			} else {
				ret = PROP_STATUS_UNKNOWN_ERROR;
			}
		}
	} else {
		std::cout << "Error commiting: " << sqlite3_errmsg(db_) << std::endl;
	}

	return ret;
}

prop_status sqlite_property::del(const std::string &key)
{
	prop_status ret = PROP_STATUS_OK;

	std::string sql = "DELETE FROM " + property_table_ + " WHERE key = \'" + key + "\'";

	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &stmt, NULL) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_DONE) {}
		ret = PROP_STATUS_OK;
	} else {
		ret = PROP_STATUS_UNKNOWN_ERROR;
	}

	sqlite3_finalize(stmt);

	return ret;
}
} // namespace property
