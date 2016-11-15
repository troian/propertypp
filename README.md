# C++ property library
Simple API that may wrap different key/value starges.
Property tree is not supported yet

#### Implemented backends
 - SQLite3

## API Reference
```c++
class prop;
```
Pure abstract class has list of API to be implemented by different backends

## How to use small example

```c++
#include <propertypp/property.hpp>

#include <iostream>
#include <string>
#include <memory>

static const std::string property_file("property.db");

using namespace property;

int main()
{
	sp_property prop;
	try {
		prop = std::make_shared<sqlite_property>(property_file);
	} catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
```

## Tests
