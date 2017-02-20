//
// Created by Artur Troian on 10/20/16.
//
#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

namespace property {

/**
 * \brief
 */
enum class status {
	OK = 0,
	INVALID_TYPE,
	NOT_FOUND,
	ALREADY_EXISTS,
	UNKNOWN_ERROR
};

using sp_property = typename std::shared_ptr<class prop>;

using up_property = typename std::unique_ptr<class prop>;

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
		STRING = 0,
		INT = 1,
		INT64 = 2,
		DOUBLE = 3,
		BOOL = 4,
		BLOB = 5
	};

public:
	/**
	 * \brief
	 */
	prop() {}

	virtual ~prop() = 0;

private: // Forbid copy
	prop(prop const &) = delete;
	prop &operator = (prop const &) = delete;

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
	virtual status get(const std::string &key, std::string &val) = 0;
	virtual status get(const std::string &key, int32_t &value) = 0;
	virtual status get(const std::string &key, int64_t &value) = 0;
	virtual status get(const std::string &key, double &value) = 0;
	virtual status get(const std::string &key, bool &value) = 0;
	virtual status get(const std::string &key, prop::blob_type &data) = 0;

public: // setters
	virtual status set(const std::string &key, const std::string &val, bool update = false) = 0;
	virtual status set(const std::string &key, double value, bool update = false) = 0;
	virtual status set(const std::string &key, bool value, bool update = false) = 0;
	virtual status set(const std::string &key, int32_t value, bool update = false) = 0;
	virtual status set(const std::string &key, int64_t value, bool update = false) = 0;
	virtual status set(const std::string &key, const prop::blob_type &data, bool update = false) = 0;

public:
	virtual status type(const std::string &key, value_type &type) const = 0;
	virtual status type(const std::string &key, value_type &type) = 0;

public:
	virtual status del(const std::string &key) = 0;
};

} // namespace property
