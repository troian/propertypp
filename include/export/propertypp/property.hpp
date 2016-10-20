//
// Created by Artur Troian on 10/20/16.
//
#pragma once

#include <cstdint>
#include <string>
#include <memory>

#include <sqlite3.h>

namespace property {

enum class prop_status {
	PROP_STATUS_OK = 0,
	PROP_STATUS_INVALID_TYPE,
	PROP_STATUS_NOT_FOUND,
	PROP_STATUS_ALREADY_EXISTS,
	PROP_STATUS_UNKNOWN_ERROR
};

using sp_property = typename std::shared_ptr<class property>;

class property {
protected:
	enum class value_type {
		VALUE_TYPE_STRING = 0,
		VALUE_TYPE_INT,
		VALUE_TYPE_INT64,
		VALUE_TYPE_FLOAT,
		VALUE_TYPE_DOUBLE,
		VALUE_TYPE_BOOL
	};

public:
	property() {};
	virtual ~property() = 0;

public: // getters
	/**
	 * \brief  Get property
	 *
	 * \param[in]  key
	 * \param[ouy] val
	 *
	 * \retval \ref PROP_STATUS_OK - val contains property value
	 * \retval \ref PROP_STATUS_INVALID_TYPE - property exists but with different type of it's value
	 * \retval \ref PROP_STATUS_NOT_FOUND - property does not exists
	 * \retval \ret PROP_STATUS_UNKNOWN_ERROR - some unexpected shit happend
	 */
	virtual prop_status get(const std::string &key, std::string &val) = 0;
	virtual prop_status get(const std::string &key, int32_t &value) = 0;
	virtual prop_status get(const std::string &key, int64_t &value) = 0;
	virtual prop_status get(const std::string &key, float &value) = 0;
	virtual prop_status get(const std::string &key, double &value) = 0;
	virtual prop_status get(const std::string &key, bool &value) = 0;

public: // setters
	virtual prop_status set(const std::string &key, const std::string &val, bool update = false) = 0;
	virtual prop_status set(const std::string &key, float value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, double value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, bool value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, int32_t value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, int64_t value, bool update = false) = 0;

public:
	virtual prop_status del(const std::string &key) = 0;

};

class sqlite_property final : public property {
private:
	class req_value {
	public:
		req_value() :
			found(false)
		{}

		bool        found;
		bool        valid;
		std::string blob;
		value_type  type;
	};

public:
	sqlite_property(const std::string &db);
	virtual ~sqlite_property();

public: // getters
	virtual prop_status get(const std::string &key, std::string &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_STRING);
	}

	virtual prop_status get(const std::string &key, int32_t &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_INT);
	}

	virtual prop_status get(const std::string &key, int64_t &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_INT64);
	}

	virtual prop_status get(const std::string &key, float &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_FLOAT);
	}

	virtual prop_status get(const std::string &key, double &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_DOUBLE);
	}

	virtual prop_status get(const std::string &key, bool &value) {
		return get(key, (void *)&value, value_type::VALUE_TYPE_BOOL);
	}

private:
	prop_status get(const std::string &key, void *value, value_type type);

	prop_status set(const std::string &key, void *val, value_type type, bool update = false);


public: // setters
	virtual prop_status set(const std::string &key, const std::string &value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_STRING, update);
	}

	virtual prop_status set(const std::string &key, float value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_FLOAT, update);
	}

	virtual prop_status set(const std::string &key, double value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_DOUBLE, update);
	}

	virtual prop_status set(const std::string &key, bool value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_BOOL, update);
	}

	virtual prop_status set(const std::string &key, int32_t value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_INT, update);
	}

	virtual prop_status set(const std::string &key, int64_t value, bool update = false) {
		return set(key, (void *)&value, value_type::VALUE_TYPE_INT64, update);
	}

public:
	virtual prop_status del(const std::string &key);

private:
	static int select_exec_cb(void *ptr, int argc, char **argv, char **names);
	static int select_type_cb(void *ptr, int argc, char **argv, char **names);

private:
	sqlite3    *db_;

private:
	static const std::string property_table_;
};

} // namespace property
