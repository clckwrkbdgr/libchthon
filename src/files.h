#pragma once
#include "monsters.h"
#include "cell.h"
#include "objects.h"
#include "util.h"
#include <fstream>
#include <vector>
#include <map>

namespace Chthon { /// @defgroup Files File utilities
/// @{

/// Returns true if file with given filename exists.
bool file_exists(const std::string & filename);
/** Reads string from in stream, considering it being quoted.
 * @see escaped()
 */
std::string read_string(std::istream & in, char quote = '"');
/** Returns escaped version of given string, using given quote symbol.
 * E. g. <Hello, world> -> <"Hello, world">.
 * If quote symbol is found in string, it is escaped using backslash:
 * <Hello, "world"> -> <"Hello, \"world\"">
 */
std::string escaped(const std::string & s, char quote = '"');

/// Forward-declares store functions.
/// Declaration and definition should be in the same file.
#define FORWARD_DECLARE_SAVEFILE_STORE(Type) \
	template<class Savefile> void store(Savefile &, Type &, const char * section = nullptr); \
	template<class Savefile> void store(Savefile &, const Type &, const char * section = nullptr)

/** Defines store functions for reading and writing.
 * Variable name should be a valid C identifier only.
 * Following templated functions are produced:
 * - `void store(Savefile & savefile, Type & variable, const char * section = nullptr);`
 *   It is used to read variable from savefile.
 * - `void store(Savefile & savefile, const Type & variable, const char * section = nullptr);`
 *   It is used to write variable to savefile.
 * Optional section argument is treated as usual savefile section name.
 */
#define SAVEFILE_STORE(Type, variable) \
	template<class Savefile, class T> void store_ext_##variable(Savefile &, T &); \
	template<class Savefile> void store(Savefile & savefile, Type & variable, const char * section = nullptr) \
		{ store_ext_##variable(savefile, variable); if(section) { savefile.check(section); } } \
	template<class Savefile> void store(Savefile & savefile, const Type & variable, const char * section = nullptr) \
		{ store_ext_##variable(savefile, variable); if(section) { savefile.check(section); } } \
	template<class Savefile, class T> \
	void store_ext_##variable(Savefile & savefile, T & variable)

/// @cond INTERNAL
SAVEFILE_STORE(int, int_value) { savefile.store(int_value); }
SAVEFILE_STORE(unsigned int, unsigned_int_value) { savefile.store(unsigned_int_value); }
SAVEFILE_STORE(char, char_value) { savefile.store(char_value); }
SAVEFILE_STORE(bool, bool_value) { savefile.store(bool_value); }
SAVEFILE_STORE(std::string, string_value) { savefile.store(string_value); }
/// @endcond

/// Implements reading from text stream.
class Reader {
public:
	/// Basic Reader exception.
	struct Exception {
		std::string message;
		/// Constructs exception instance with given text.
		Exception(const std::string & text) : message(text) {}
	};
	/// Constructs Reader using specified in_stream.
	Reader(std::istream & in_stream);

	/// Reads newline.
	Reader & newline();
	/** Reads major and minor versions from file and compares it with specified ones.
	 * If major versions are not equal, exception is thrown.
	 * If minor version of the file is greater than current application savefile version, exception is thrown.
	 */
	Reader & version(int major_version, int minor_version);
	/// Returns file minor version (should be called only after versions reading).
	int version() const { return actual_minor_version; }
	/// Checks whether read stream is valid, throws Exception otherwise using given section name.
	Reader & check(const std::string & section);
	
	/// Reads int value. Use store(savefile, value) function instead.
	Reader & store(int & value);
	/// Reads unsigned int value. Use store(savefile, value) function instead.
	Reader & store(unsigned int & value);
	/// Reads char value (as its integer value). Use store(savefile, value) function instead.
	Reader & store(char & value);
	/// Reads bool value (as its integer value). Use store(savefile, value) function instead.
	Reader & store(bool & value);
	/// Reads string value (as its escaped version). Use store(savefile, value) function instead.
	Reader & store(std::string & value);
private:
	int actual_minor_version;
	std::istream & in;
};

/// Implements writing to text stream.
class Writer {
public:
	/// Basic Writer exception.
	struct Exception {
		std::string message;
		/// Constructs exception instance with given text.
		Exception(const std::string & text) : message(text) {}
	};
	/// Constructs Writer using specified out_stream.
	Writer(std::ostream & out_stream);

	/// Writes newline to stream.
	Writer & newline();
	/// Writes major and minor version to stream, separated and followed by space.
	Writer & version(int major_version, int minor_version);
	/// Returns current minor version. Should be called only after versions storing.
	int version() const { return actual_minor_version; }
	/// Checks whether write stream is valid, throws Exception otherwise using given section name.
	Writer & check(const std::string & section);

	/// Reads bool value (as its integer value). Use store(savefile, value) function instead.
	Writer & store(int value);
	/// Reads char value (as its integer value). Use store(savefile, value) function instead.
	Writer & store(unsigned int value);
	/// Reads int value. Use store(savefile, value) function instead.
	Writer & store(char value);
	/// Reads string value (as its escaped version). Use store(savefile, value) function instead.
	Writer & store(bool value);
	/// Reads unsigned int value. Use store(savefile, value) function instead.
	Writer & store(const std::string & value);
private:
	int actual_minor_version;
	std::ostream & out;
};

/// @}
}
