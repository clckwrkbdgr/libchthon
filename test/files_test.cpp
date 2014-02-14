#include "../src/files.h"
#include "../src/map.h"
#include "../src/format.h"
#include "../src/test.h"
#include <sstream>

SUITE(files) {

TEST(should_read_quoted_string)
{
	std::istringstream in("\"hello world\"");
	std::string s = Chthon::read_string(in);
	EQUAL(s, "hello world");
}

TEST(should_read_quoted_string_with_escaped_symbols)
{
	std::istringstream in("\"hello \\\"world\\\"\"");
	std::string s = Chthon::read_string(in);
	EQUAL(s, "hello \"world\"");
}

TEST(should_read_word_as_a_string_if_not_quoted)
{
	std::istringstream in("hello world");
	std::string s = Chthon::read_string(in);
	EQUAL(s, "hello");
}

TEST(should_read_word_until_nonword_symbol)
{
	std::istringstream in("hello>world ");
	std::string s = Chthon::read_string(in, '"', '>');
	EQUAL(s, "hello");
}


using Chthon::Reader;

TEST(reader_should_skip_newline)
{
	std::istringstream in("\n");
	Reader reader(in);
	reader.newline();
	EQUAL(int(in.tellg()), 0);
}

TEST(reader_should_throw_exception_when_major_version_is_wrong)
{
	std::istringstream in("1 2 ");
	Reader reader(in);
	CATCH(reader.version(2, 2), const Reader::Exception & e) {
		EQUAL(
				e.message,
				"Savefile has major version 1, which is incompatible with current program savefile major version 2."
			 );
	}
}

TEST(reader_should_throw_exception_when_minor_version_is_less)
{
	std::istringstream in("1 2 ");
	Reader reader(in);
	CATCH(reader.version(1, 1), const Reader::Exception & e) {
		EQUAL(
				e.message,
				"Savefile has minor version 2, which is incompatible with current program savefile minor version 1."
			 );
	}
}

TEST(reader_should_throw_exception_when_stream_is_bad)
{
	std::istringstream in(" ");
	Reader reader(in);
	int i;
	reader.store(i);
	CATCH(reader.check("test"), const Reader::Exception & e) {
		EQUAL(
				e.message,
				"Error: savefile is corrupted (reading test)."
			 );
	}
}

TEST(reader_should_read_int)
{
	std::istringstream in("-1 ");
	Reader reader(in);
	int i;
	reader.store(i);
	EQUAL(i, -1);
}

TEST(reader_should_read_unsigned)
{
	std::istringstream in("1 ");
	Reader reader(in);
	unsigned int i;
	reader.store(i);
	EQUAL(i, (unsigned)1);
}

TEST(reader_should_read_char_as_int)
{
	std::istringstream in("65 ");
	Reader reader(in);
	char ch;
	reader.store(ch);
	EQUAL(ch, 'A');
}

TEST(reader_should_read_bool_as_int)
{
	std::istringstream in("1 ");
	Reader reader(in);
	bool b;
	reader.store(b);
	EQUAL(b, true);
}

TEST(reader_should_read_string_as_quoted_string)
{
	std::istringstream in("\"hello world\" ");
	Reader reader(in);
	std::string s;
	reader.store(s);
	EQUAL(s, "hello world");
}


using Chthon::Writer;

TEST(writer_should_write_newline)
{
	std::ostringstream out;
	Writer writer(out);
	writer.newline();
	EQUAL(out.str(), "\n");
}

TEST(writer_should_write_major_and_minor_version)
{
	std::ostringstream out;
	Writer writer(out);
	writer.version(1, 2);
	EQUAL(out.str(), "1 2 ");
}

TEST(writer_should_write_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(-1);
	EQUAL(out.str(), "-1 ");
}

TEST(writer_should_write_unsigned_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(unsigned(1));
	EQUAL(out.str(), "1 ");
}

TEST(writer_should_write_char_as_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store('A');
	EQUAL(out.str(), "65 ");
}

TEST(writer_should_write_bool_as_int_and_a_space)
{
	std::ostringstream out;
	Writer writer(out);
	writer.store(true);
	EQUAL(out.str(), "1 ");
}


namespace UserNamespace {

	struct UserDefinedType {
		int i;
		char ch;
		UserDefinedType() : i(0), ch(0) {}
		UserDefinedType(int _i, char _ch) : i(_i), ch(_ch) {}
	};

}

SAVEFILE_STORE(UserNamespace::UserDefinedType, value)
{
	savefile.store(value.i).store(value.ch);
}

TEST(should_write_user_defined_type)
{
	std::ostringstream out;
	Writer writer(out);
	store(writer, UserNamespace::UserDefinedType(1, 'A'));
	EQUAL(out.str(), "1 65 ");
}

TEST(should_read_user_defined_type)
{
	std::istringstream in("1 65 ");
	Reader reader(in);
	UserNamespace::UserDefinedType value;
	store(reader, value);
	EQUAL(value.i, 1);
	EQUAL(value.ch, 'A');
}

TEST(should_throw_exception_when_cannot_read_user_defined_type)
{
	std::istringstream in("1 a ");
	Reader reader(in);
	UserNamespace::UserDefinedType value;
	CATCH(store(reader, value, "user defined type"), const Reader::Exception & e) {
		EQUAL(
				e.message,
				"Error: savefile is corrupted (reading user defined type)."
			 );
	}
}

}

