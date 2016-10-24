//
// Created by Artur Troian on 10/20/16.
//
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <sqlite3.h>

namespace property {

/**
 * \brief
 */
enum class prop_status {
	PROP_STATUS_OK = 0,
	PROP_STATUS_INVALID_TYPE,
	PROP_STATUS_NOT_FOUND,
	PROP_STATUS_ALREADY_EXISTS,
	PROP_STATUS_UNKNOWN_ERROR
};

using sp_property = typename std::shared_ptr<class prop>;

/**
 * \brief
 */
class prop {
public:
	typedef std::vector<uint8_t> blob_type;

	/**
	 * \brief
	 */
	enum class value_type {
		VALUE_TYPE_STRING = 0,
		VALUE_TYPE_INT = 1,
		VALUE_TYPE_INT64 = 2,
		VALUE_TYPE_DOUBLE = 3,
		VALUE_TYPE_BOOL = 4,
		VALUE_TYPE_BLOB = 5
	};

public:
	/**
	 * \brief
	 */
	prop() {};

	virtual ~prop() = 0;

public: // getters
	/**
	 * \brief  Get property
	 *
	 * \param[in]  key
	 * \param[out] val
	 *
	 * \retval \ref PROP_STATUS_OK - val contains property value
	 * \retval \ref PROP_STATUS_INVALID_TYPE - property exists but with different type of it's value
	 * \retval \ref PROP_STATUS_NOT_FOUND - property does not exists
	 * \retval \ret PROP_STATUS_UNKNOWN_ERROR - some unexpected shit happend
	 */
	virtual prop_status get(const std::string &key, std::string &val) = 0;
	virtual prop_status get(const std::string &key, int32_t &value) = 0;
	virtual prop_status get(const std::string &key, int64_t &value) = 0;
	virtual prop_status get(const std::string &key, double &value) = 0;
	virtual prop_status get(const std::string &key, bool &value) = 0;
	virtual prop_status get(const std::string &key, prop::blob_type &data, bool update = false) = 0;

public: // setters
	virtual prop_status set(const std::string &key, const std::string &val, bool update = false) = 0;
	virtual prop_status set(const std::string &key, double value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, bool value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, int32_t value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, int64_t value, bool update = false) = 0;
	virtual prop_status set(const std::string &key, const prop::blob_type &data, bool update = false) = 0;

public:
	virtual prop_status type(const std::string &key, value_type &type) const = 0;
	virtual prop_status type(const std::string &key, value_type &type) = 0;

public:
	virtual prop_status del(const std::string &key) = 0;
};

} // namespace property
