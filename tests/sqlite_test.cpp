//
// Created by Artur Troian on 10/21/16.
//

#include <gtest/gtest.h>

#include <cstdint>
#include <cmath>
#include <limits>

#include <propertypp/sqlite.hpp>
#include <propertypp/property.hpp>

using property;

static const char db_file[] = "test.db";

bool compare_doubles(double a, double b) {
	if (std::fabs(a - b) < std::numeric_limits<double>::epsilon())
		return true;
	else
		return false;
}

TEST(Database, create_close_drop)
{
	{
		// Try to open the non-existing database
		EXPECT_NO_THROW(sqlite_property prop(db_file));
	}
	remove(db_file.c_str());
}

TEST(Database, create_close_reopen)
{
	remove(db_file.c_str());
	{
		// Try to open the non-existing database
		EXPECT_NO_THROW(sqlite_property prop(db_file));
	} // Close DB test.db
	{
		// Reopen the database file
		EXPECT_NO_THROW(sqlite_property prop(db_file));
	}

	remove(db_file.c_str());
}

TEST(Database, set_get_del)
{
	remove(db_file.c_str());

	{
		sqlite_property prop(db_file);

		// Check string
		std::string val_str;
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key_str", std::string("val1")));
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key_str", val_str));
		EXPECT_EQ(0, val_str.compare("val1"));

		// Check int32
		int32_t val_int;
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key_int", (int32_t)1024));
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key_int", val_int));
		EXPECT_EQ(true, val_int == 1024);

		// Check int64
		int64_t val_int64;
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key_int64", (int64_t)0x5EBFAFFF77FFBFF));
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key_int64", val_int64));
		EXPECT_EQ(true, val_int64 == 0x5EBFAFFF77FFBFF);

		// Check bool
		bool val_bool;
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key_bool", static_cast<bool>(true)));
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key_bool", val_bool));
		EXPECT_EQ(false, val_bool == false);

		// Check blob
		prop::blob_type val_blob = { 1, 2, 3, 4, 5, 6 };
		prop::blob_type val_blob1;

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key_blob", val_blob));
		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key_blob", val_blob1));
		EXPECT_EQ(true, val_blob == val_blob1);
	}

	remove(db_file.c_str());
}

TEST(Database, set_unique)
{
	remove(db_file.c_str());
	{
		sqlite_property prop("test.db");

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key1", std::string("val1")));

		EXPECT_EQ(prop_status::PROP_STATUS_ALREADY_EXISTS, prop.set("key1", std::string("val1")));
	}
	remove(db_file.c_str());
}

TEST(Database, set_update_get)
{
	remove(db_file.c_str());

	{
		sqlite_property prop("test.db");

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key1", std::string("val1")));

		EXPECT_EQ(prop_status::PROP_STATUS_ALREADY_EXISTS, prop.set("key1", std::string("val2")));

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key1", std::string("val2"), true));

		std::string val_str;

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.get("key1", val_str));

		EXPECT_EQ(true, val_str.compare("val2") == 0);
	}

	remove(db_file.c_str());
}

TEST(Database, set_check_type)
{
	remove(db_file.c_str());
	{
		sqlite_property prop("test.db");

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key1", std::string("val1")));

		int32_t     val_int;
		int64_t     val_int64;
		bool        val_bool = false;
		float       val_float;
		double      val_double;
		std::string val_str;

		EXPECT_EQ(prop_status::PROP_STATUS_INVALID_TYPE, prop.get("key1", val_int));

		EXPECT_EQ(prop_status::PROP_STATUS_OK, prop.set("key1", std::string("val1"), true));

	}
	remove(db_file.c_str());
}
