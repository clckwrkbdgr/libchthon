#include "xmlreader.h"
#include "files.h"
#include "log.h"
#include "format.h"
#include <sstream>
#include <cctype>

namespace Chthon {

XMLReader::XMLReader(std::istream & stream)
	: s(stream)
{
	s.unsetf(std::ios::skipws);
}

const std::string & XMLReader::skip_to_tag(const std::string & tag_name)
{
	to_next_tag();
	while(!current_tag.empty() && current_tag != tag_name) {
		to_next_tag();
	}
	return current_tag;
}

static std::string convert_xml_entity(const std::string & entity)
{
	static std::vector<std::pair<std::string, std::string>> entities_data = {
		{"quot", "\""},
		{"nbsp", " "},
		{"#171", "«"},
		{"#187", "»"},
		{"laquo", "«"},
		{"raquo", "»"},
		{"lt", "<"},
		{"gt", ">"},
		{"amp", "&"},
		{"rarr", "→"},
		{"mdash", "—"},
		{"ndash", "–"},
		{"#8216", "‘"},
		{"#8217", "’"},
		{"hellip", "…"},
		{"rsquo", "’"},
		{"rdquo", "”"},
		{"ldquo", "“"},
		{"#8220", "“"},
		{"#8221", "”"},
		{"#8211", "–"},
		{"#8230", "…"},
		{"#8594", "→"},
		{"apos", "'"},
		{"#58", ":"},
		{"#41", ")"},
		{"rlm", "@"},
	};
	static std::map<std::string, std::string> entities(
			entities_data.begin(), entities_data.end());

	if(entities.count(entity)) {
		return entities[entity];
	} else if(Chthon::starts_with(entity, "#")) {
		int entity_code = atoi(entity.substr(1).c_str());
		if(entity_code < 128) {
			return std::string(1, (char)entity_code);
		}
	}
	return '&' + entity + ';';
}

const std::string & XMLReader::to_next_tag()
{
	current_tag.clear();
	current_content.clear();
	attributes.clear();

	char ch;
	std::string entity;
	bool in_entity = false;
	s >> ch;
	while(s && ch != '<') {
		if(ch == '&') {
			in_entity = true;
		} else if(in_entity) {
			if(ch == ';') {
				current_content += convert_xml_entity(entity);
				entity.clear();
				in_entity = false;
			} else {
				entity += ch;
			}
		} else {
			current_content += ch;
		}
		s >> ch;
	}
	if(in_entity) {
		current_content += '&' + entity;
	}

	enum { ERROR, TAG_NAME, ATTRIBUTE, EQUALS };
	std::string attribute;
	int mode = TAG_NAME;
	s >> ch;
	while(s && ch != '>') {
		switch(mode) {
			case TAG_NAME:
				if(isspace(ch)) {
					if(!current_tag.empty()) {
						mode = ATTRIBUTE;
						attribute.clear();
					}
				} else if(ch == '/') {
					attributes["/"] = "";
					mode = ERROR;
				} else {
					current_tag += ch;
				}
				break;
			case ATTRIBUTE:
				if(isspace(ch)) {
					if(!attribute.empty()) {
						mode = EQUALS;
					}
				} else {
					if(ch == '=') {
						attributes[attribute] = Chthon::read_string(s, '"', '>');
						mode = ATTRIBUTE;
						attribute.clear();
					} else {
						attribute += ch;
					}
				}
				break;
			case EQUALS:
				if(!isspace(ch)) {
					if(ch == '=') {
						attributes[attribute] = Chthon::read_string(s, '"', '>');
						mode = ATTRIBUTE;
						attribute.clear();
					} else {
						if(!attribute.empty()) {
							attributes[attribute] = "";
							attribute.clear();
						}
						mode = ATTRIBUTE;
						attribute = ch;
					}
				}
				break;
			case ERROR: default: break;
		}
		s >> ch;
	}
	if(!attribute.empty()) {
		attributes[attribute] = "";
	}
	return current_tag;
}

}
