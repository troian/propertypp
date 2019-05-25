//
// Created by Artur Troian on 10/21/16.
//
#pragma once

#include <sqlite3.h>

#include <vector>
#include <string>

#include <propertypp/property.hh>

#include <util/types.hh>

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
		req_value()
			: found(false)
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
	explicit sqlite(sqlite_wrap::sp s);

	~sqlite() override = default;

public: // Forbid copy
	sqlite(sqlite const &) = delete;
	sqlite &operator = (sqlite const &) = delete;

public: // getters
	property::status get(const std::string &key, std::string &value) override {
		return get(key, static_cast<void *>(&value), value_type::STRING);
	}

	property::status get(const std::string &key, int32_t &value) override {
		return get(key, static_cast<void *>(&value), value_type::INT);
	}

	property::status get(const std::string &key, int64_t &value) override {
		return get(key, static_cast<void *>(&value), value_type::INT64);
	}

	property::status get(const std::string &key, double &value) override {
		return get(key, static_cast<void *>(&value), value_type::DOUBLE);
	}

	property::status get(const std::string &key, bool &value) override {
		return get(key, static_cast<void *>(&value), value_type::BOOL);
	}

	property::status get(const std::string &key, prop::blob_type &value) override {
		return get(key, static_cast<void *>(&value), value_type::BLOB);
	}

public: // setters
	property::status set(const std::string &key, const std::string &value, bool update = false) override {
		return set(key, static_cast<const void *>(&value), value_type::STRING, update);
	}

	property::status set(const std::string &key, double value, bool update = false) override {
		return set(key, static_cast<const void *>(&value), value_type::DOUBLE, update);
	}

	property::status set(const std::string &key, bool value, bool update = false) override {
		return set(key, static_cast<const void *>(&value), value_type::BOOL, update);
	}

	property::status set(const std::string &key, int32_t value, bool update = false) override {
		return set(key, static_cast<const void *>(&value), value_type::INT, update);
	}

	property::status set(const std::string &key, int64_t value, bool update = false) {
		return set(key, static_cast<const void *>(&value), value_type::INT64, update);
	}

	property::status set(const std::string &key, const prop::blob_type &value, bool update = false) override {
		return set(key, static_cast<const void *>(&value), value_type::BLOB, update);
	}

public:
	property::status type(const std::string &key, value_type &type) const override;

	property::status type(const std::string &key, value_type &type) override;

public:
	property::status del(const std::string &key) override;

public:
	auto shared_from_this() {
		return shared_from(this);
	}

private:
	property::status get(const std::string &key, void *value, value_type type);

	property::status set(const std::string &key, const void *val, value_type type, bool update = false);

private:
	static int select_exec_cb(void *ptr, int argc, char **argv, char **names);

	static int type_exec_cb(void *ptr, int argc, char **argv, char **names);

private:
	sqlite_wrap::sp _sqlite;

private:
	static const char property_table_[];
};

} // namespace property
