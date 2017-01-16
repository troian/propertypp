//
// Created by Artur Troian on 10/21/16.
//
#pragma once

#include <propertypp/property.hpp>

#include <vector>
#include <string>
#include <sqlite3.h>

namespace property {

/**
 * \brief
 */
class sqlite_property final : public prop {
private:
	/**
	 * \brief
	 */
	class req_value {
	public:
		req_value() :
			  found(false)
			, valid(false)
			, blob()
		{}

		bool        found;
		bool        valid;
		std::string blob;
		value_type  type;
	};

public:
	/**
	 * \brief
	 *
	 * \param[in]  db
	 */
	explicit sqlite_property(const std::string &db);

	virtual ~sqlite_property();

public: // getters
	virtual status get(const std::string &key, std::string &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_STRING);
	}

	virtual status get(const std::string &key, int32_t &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_INT);
	}

	virtual status get(const std::string &key, int64_t &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_INT64);
	}

	virtual status get(const std::string &key, double &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_DOUBLE);
	}

	virtual status get(const std::string &key, bool &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_BOOL);
	}

	virtual status get(const std::string &key, prop::blob_type &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_BLOB);
	}

public: // setters
	virtual status set(const std::string &key, const std::string &value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_STRING, update);
	}

	virtual status set(const std::string &key, double value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_DOUBLE, update);
	}

	virtual status set(const std::string &key, bool value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_BOOL, update);
	}

	virtual status set(const std::string &key, int32_t value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_INT, update);
	}

	virtual status set(const std::string &key, int64_t value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_INT64, update);
	}

	virtual status set(const std::string &key, const prop::blob_type &value, bool update = false) {
		return set(key, (const void *)&value, value_type::VALUE_TYPE_BLOB, update);
	}

public:
	virtual status type(const std::string &key, value_type &type) const;

	virtual status type(const std::string &key, value_type &type);

public:
	virtual status del(const std::string &key);

private:
	status get(const std::string &key, void *value, value_type type);

	status set(const std::string &key, const void * const val, value_type type, bool update = false);

private:
	static int select_exec_cb(void *ptr, int argc, char **argv, char **names);

	static int type_exec_cb(void *ptr, int argc, char **argv, char **names);

private:
	sqlite3    *db_;

private:
	static const std::string property_table_;
};

} // namespace property
