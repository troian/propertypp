//
// Created by Artur Troian on 10/21/16.
//
#pragma once

#include <sqlite3.h>

#include <vector>
#include <string>

#include <propertypp/property.hpp>

namespace property {

/**
 * \brief
 */
class sqlite final : public prop {
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
			, type(value_type::STRING)
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
	explicit sqlite(const std::string &db);

	virtual ~sqlite();

private:
private: // Forbid copy
	sqlite(sqlite const &) = delete;
	sqlite &operator = (sqlite const &) = delete;

public: // getters
	virtual property::status get(const std::string &key, std::string &value) {
		return get(key, static_cast<void *>(&value), value_type::STRING);
	}

	virtual property::status get(const std::string &key, int32_t &value) {
		return get(key, static_cast<void *>(&value), value_type::INT);
	}

	virtual property::status get(const std::string &key, int64_t &value) {
		return get(key, static_cast<void *>(&value), value_type::INT64);
	}

	virtual property::status get(const std::string &key, double &value) {
		return get(key, static_cast<void *>(&value), value_type::DOUBLE);
	}

	virtual property::status get(const std::string &key, bool &value) {
		return get(key, static_cast<void *>(&value), value_type::BOOL);
	}

	virtual property::status get(const std::string &key, prop::blob_type &value) {
		return get(key, static_cast<void *>(&value), value_type::BLOB);
	}

public: // setters
	virtual property::status set(const std::string &key, const std::string &value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::STRING, update);
	}

	virtual property::status set(const std::string &key, double value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::DOUBLE, update);
	}

	virtual property::status set(const std::string &key, bool value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::BOOL, update);
	}

	virtual property::status set(const std::string &key, int32_t value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::INT, update);
	}

	virtual property::status set(const std::string &key, int64_t value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::INT64, update);
	}

	virtual property::status set(const std::string &key, const prop::blob_type &value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::BLOB, update);
	}

public:
	virtual property::status type(const std::string &key, value_type &type) const;

	virtual property::status type(const std::string &key, value_type &type);

public:
	virtual property::status del(const std::string &key);

private:
	property::status get(const std::string &key, void *value, value_type type);

	property::status set(const std::string &key, const void * const val, value_type type, bool update = false);

private:
	static int select_exec_cb(void *ptr, int argc, char **argv, char **names);

	static int type_exec_cb(void *ptr, int argc, char **argv, char **names);

private:
	sqlite3    *db_;

private:
	static const char property_table_[];
};

} // namespace property
