#include "../src/xmlreader.h"
#include <chthon2/test.h>
#include <sstream>
using Chthon::XMLReader;

SUITE(xml) {

TEST(should_find_first_tag)
{
	std::istringstream stream("Hello<world>");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "world");
}

TEST(should_have_content_before_the_first_tag)
{
	std::istringstream stream("Hello<world>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_content(), "Hello");
}

TEST(should_have_content_after_the_last_tag)
{
	std::istringstream stream("<world>Hello");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "world");
	EQUAL(reader.get_current_content(), "");
	std::string last_tag = reader.to_next_tag();
	EQUAL(last_tag, "");
	EQUAL(reader.get_current_content(), "Hello");
}

TEST(should_find_next_tag)
{
	std::istringstream stream("<Hello><world>");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "Hello");
	std::string next_tag = reader.to_next_tag();
	EQUAL(next_tag, "world");
}

TEST(should_return_empty_tag_when_no_tag)
{
	std::istringstream stream("<Hello>");
	XMLReader reader(stream);
	std::string first_tag = reader.to_next_tag();
	EQUAL(first_tag, "Hello");
	std::string next_tag = reader.to_next_tag();
	ASSERT(next_tag.empty());
}

TEST(should_skip_until_tag_is_found)
{
	std::istringstream stream("<Hello>content<world>");
	XMLReader reader(stream);
	std::string tag = reader.skip_to_tag("world");
	EQUAL(tag, "world");
}

TEST(should_skip_to_the_end_if_tag_is_not_found)
{
	std::istringstream stream("<Hello>content<world>");
	XMLReader reader(stream);
	std::string tag = reader.skip_to_tag("sometag");
	ASSERT(tag.empty());
}

TEST(should_store_intertag_content)
{
	std::istringstream stream("<Hello>content<world>");
	XMLReader reader(stream);
	reader.to_next_tag();
	reader.to_next_tag();
	EQUAL(reader.get_current_content(), "content");
}

TEST(should_skip_whitespace_before_tag_name)
{
	std::istringstream stream("< Hello>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_skip_whitespace_after_tag_name)
{
	std::istringstream stream("<Hello >");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_read_tag_name_as_one_identifier)
{
	std::istringstream stream("<Hello attribute=value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_current_tag(), "Hello");
}

TEST(should_read_attribute)
{
	std::istringstream stream("<Hello attribute=value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_after_attribute_name)
{
	std::istringstream stream("<Hello attribute =value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_before_attribute_value)
{
	std::istringstream stream("<Hello attribute= value>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_skip_whitespace_after_attribute_value)
{
	std::istringstream stream("<Hello attribute=value  >");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_read_attributes_in_self_closed_tag)
{
	std::istringstream stream("<Hello attribute=\"value\" />");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
}

TEST(should_read_more_than_one_attribute)
{
	std::istringstream stream("<Hello attribute=value second=two>");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "value");
	EQUAL(reader.get_attributes()["second"], "two");
}

TEST(should_read_quoted_attribute)
{
	std::istringstream stream("<Hello attribute=\"hello \\\"world\\\"\">");
	XMLReader reader(stream);
	reader.to_next_tag();
	EQUAL(reader.get_attributes()["attribute"], "hello \"world\"");
}

}

