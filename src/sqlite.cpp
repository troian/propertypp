//
// Created by Artur Troian on 10/20/16.
//

#include <cstdlib>
#include <iostream>
#include <exception>
#include <stdexcept>

#include <propertypp/sqlite.hpp>
#include <tools/base64.hpp>

namespace property {

int sqlite::select_exec_cb(void *ptr, int argc, char **argv, char **names)
{
	// suppress: warning: unused parameter 'names' [-Wunused-parameter]
	(void)names;

	req_value *value = reinterpret_cast<req_value *>(ptr);

	value->found = true;

	if (argc == 2) {
		value->blob  = std::string(argv[0]);
		value->type  = static_cast<value_type>(atoi(argv[1]));
		value->valid = true;
	} else {
		value->valid = false;
	}

	return 0;
}

int sqlite::type_exec_cb(void *ptr, int argc, char **argv, char **names)
{
	// suppress: warning: unused parameter 'names' [-Wunused-parameter]
	(void)names;

	req_value *value = reinterpret_cast<req_value *>(ptr);

	value->found = true;

	if (argc == 1) {
		value->type  = static_cast<value_type>(atoi(argv[0]));
		value->valid = true;
	} else {
		value->valid = false;
	}

	return 0;
}

const char sqlite::property_table_[] = "property_table";

sqlite::sqlite(const std::string &db) :
	  prop()
	, db_(NULL)
{
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

sqlite::~sqlite()
{
	sqlite3_close(db_);
}

status sqlite::get(const std::string &key, void *value, value_type type)
{
	std::string sql = "SELECT value, type FROM " + std::string(property_table_) + " WHERE key = \'" + key + "\'";
	int ret;
	char *errmsg = NULL;
	req_value rsp;

	ret = sqlite3_exec(db_, sql.c_str(), select_exec_cb, &rsp, &errmsg);

	if (ret == SQLITE_OK) {
		if (rsp.found) {
			if (type != rsp.type) {
				return status::INVALID_TYPE;
			} else {
				switch (type) {
				case value_type::STRING: {
					std::string *val = reinterpret_cast<std::string *>(value);
					*val = rsp.blob;
					break;
				}
				case value_type::INT: {
					int32_t *val = reinterpret_cast<int32_t *>(value);
					*val = std::stoi(rsp.blob);
					break;
				}
				case value_type::INT64: {
					int64_t *val = reinterpret_cast<int64_t *>(value);
					*val = std::stoll(rsp.blob);
					break;
				}
				case value_type::DOUBLE: {
					double *val = reinterpret_cast<double *>(value);
					*val = std::stod(rsp.blob);
					break;
				}
				case value_type::BOOL: {
					bool *val = reinterpret_cast<bool *>(value);

					*val = rsp.blob.compare("false") != 0;

					break;
				}
				case value_type::BLOB: {
					prop::blob_type *val = reinterpret_cast<prop::blob_type *>(value);

					prop::blob_type tmp = tools::base64::decode<prop::blob_type>(rsp.blob);
					*val = std::move(tmp);
					break;
				}
				}
			}
		} else {
			return status::NOT_FOUND;
		}

	} else {
		sqlite3_free(errmsg);
		std::cout << "Error: " << ret << std::endl;
		return status::UNKNOWN_ERROR;
	}

	return status::OK;
}

status sqlite::set(const std::string &key, const void * const val, value_type type, bool update)
{
	status ret = status::OK;

	std::string sql = "INSERT INTO " + std::string(property_table_) + "(key, value, type) values (?,?,?)";
	sqlite3_stmt *stmt;

	std::string value;

	switch (type) {
	case value_type::STRING: {
		value = *(reinterpret_cast<const std::string * const>(val));
		break;
	}
	case value_type::INT: {
		const int32_t * const v = reinterpret_cast<const int32_t * const>(val);
		value = std::to_string(*v);
		break;
	}
	case value_type::INT64: {
		const int64_t * const v = reinterpret_cast<const int64_t * const>(val);
		value = std::to_string(*v);
		break;
	}
	case value_type::DOUBLE: {
		const double * const v = reinterpret_cast<const double * const>(val);
		value = std::to_string(*v);
		break;
	}
	case value_type::BOOL: {
		const bool * const v = reinterpret_cast<const bool * const>(val);
		value = *v ? "true" : "false";
		break;
	}
	case value_type::BLOB: {
		const prop::blob_type * const v = reinterpret_cast<const prop::blob_type * const>(val);
		tools::base64::encode(value, v);
		break;
	}
	}

	int rc = sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &stmt, NULL);

	if (rc == SQLITE_OK) {
		// bind values

		sqlite3_bind_text(stmt, 1, key.c_str(), key.size(), 0);
		sqlite3_bind_blob(stmt, 2, value.c_str(), value.size(), NULL);
		sqlite3_bind_int(stmt, 3, static_cast<int>(type));

		// commit
		sqlite3_step(stmt);
		rc = sqlite3_finalize(stmt);

		if (rc != SQLITE_OK) {
			if (rc == SQLITE_CONSTRAINT) {
				if (!update) {
					ret = status::ALREADY_EXISTS;
				} else {
					value_type prop_type;

					ret = sqlite::type(key, prop_type);
					if (ret == status::OK) {
						if (prop_type != type) {
							ret = status::INVALID_TYPE;
						} else {
							sql = "UPDATE " + std::string(property_table_) + " SET value = ? WHERE key = \'" + key + "\'";
							rc = sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &stmt, NULL);
							if (rc == SQLITE_OK) {
								sqlite3_bind_blob(stmt, 1, value.c_str(), value.size(), NULL);

								sqlite3_step(stmt);
								rc = sqlite3_finalize(stmt);

								if (rc != SQLITE_OK) {
									ret = status::UNKNOWN_ERROR;
									std::cerr << "Error commiting: " << sqlite3_errmsg(db_) << std::endl;
								} else {
									ret = status::OK;
								}
							} else {
								std::cerr << "Error commiting: " << sqlite3_errmsg(db_) << std::endl;
								ret = status::UNKNOWN_ERROR;
							}
						}
					} else {
						std::cerr << "Error commiting: " << sqlite3_errmsg(db_) << std::endl;
					}
				}
			} else {
				ret = status::UNKNOWN_ERROR;
			}
		}
	} else {
		std::cout << "Error commiting: " << sqlite3_errmsg(db_) << std::endl;
	}

	return ret;
}

status sqlite::del(const std::string &key)
{
	status ret = status::OK;

	std::string sql = "DELETE FROM " + std::string(property_table_) + " WHERE key = \'" + key + "\'";

	sqlite3_stmt *stmt;

	if (sqlite3_prepare_v2(db_, sql.c_str(), sql.size(), &stmt, NULL) == SQLITE_OK) {
		while (sqlite3_step(stmt) == SQLITE_DONE) {}
		ret = status::OK;
	} else {
		ret = status::UNKNOWN_ERROR;
	}

	sqlite3_finalize(stmt);

	return ret;
}

status sqlite::type(const std::string &key, value_type &type) const
{
	status retval = status::OK;

	std::string sql = "SELECT type FROM " + std::string(property_table_) + " WHERE key = \'" + key + "\'";

	int ret;
	char *errmsg = NULL;
	req_value rsp;

	ret = sqlite3_exec(db_, sql.c_str(), type_exec_cb, &rsp, &errmsg);

	if (ret == SQLITE_OK) {
		if (rsp.found) {
	   	    type = rsp.type;
		} else {
			retval = status::NOT_FOUND;
		}
	} else {
		sqlite3_free(errmsg);
		retval = status::UNKNOWN_ERROR;
	}

	return retval;
}

status sqlite::type(const std::string &key, value_type &type)
{
	status retval = status::OK;

	std::string sql = "SELECT type FROM " + std::string(property_table_) + " WHERE key = \'" + key + "\'";

	int ret;
	char *errmsg = NULL;
	req_value rsp;

	ret = sqlite3_exec(db_, sql.c_str(), type_exec_cb, &rsp, &errmsg);

	if (ret == SQLITE_OK) {
		if (rsp.found) {
			type = rsp.type;
		} else {
			retval = status::NOT_FOUND;
		}
	} else {
		sqlite3_free(errmsg);
		retval = status::UNKNOWN_ERROR;
	}

	return retval;
}

} // namespace property
