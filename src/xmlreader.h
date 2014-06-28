#pragma once
#include <string>
#include <map>

namespace Chthon {

/** Iterator class for XML document.
 * Takes stream on the input.
 */
struct XMLReader {
	/// Creates reader with the stream.
	XMLReader(std::istream & stream);

	/** Seeks to the end of the next tag.
	 * If there is not tag, seeks up until EOF.
	 * After this method invoking there will be known and stored current tag
	 * name, tag attributes and text content between last and current tags.
	 * @see get_current_tag()
	 * @see get_current_content()
	 * @see get_attributes()
	 */
	const std::string & to_next_tag();
	/** Reads fast-forward till finds specified tag, or to the EOF if tag could
	 * not be found.
	 * Equivalent to calling to_next_tag() in the loop.
	 * @see to_next_tag();
	 */
	const std::string & skip_to_tag(const std::string & tag_name);

	/// Get current (last found) tag.
	const std::string & get_current_tag() const { return current_tag; }
	/// Get current (last found) content - between last and current tag.
	const std::string & get_current_content() const { return current_content; }
	/// Get attributes for current tag.
	std::map<std::string, std::string> get_attributes() const { return attributes; }
private:
	std::istream & s;
	std::string current_tag;
	std::string current_content;
	std::map<std::string, std::string> attributes;
};

}
