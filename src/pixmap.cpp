#include "pixmap.h"
#include "util.h"
#include "log.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>

namespace Chthon {

Color from_rgb(uint8_t r, uint8_t g, uint8_t b)
{
	return uint32_t(0xff << 24) | uint32_t(r << 16) | uint32_t(g << 8) | b;
}

uint8_t get_red(Color color)
{
	return (color >> 16) & 0xff;
}

uint8_t get_green(Color color)
{
	return (color >> 8) & 0xff;
}

uint8_t get_blue(Color color)
{
	return color & 0xff;
}

bool is_transparent(Color color)
{
	return (color >> 24) == 0;
}

Color rgb_to_argb(Color color)
{
	return color | uint32_t(0xff << 24);
}

}

namespace Chthon {

void Pixmap::clear_xpm_data()
{
	color_count = 0;
	row_count = 0;
	interspaces.clear();
	colors.clear();
	values_interspaces.clear();
	colors_interspaces.clear();
}

Pixmap::Pixmap(unsigned pixmap_width, unsigned pixmap_height, unsigned palette_size)
	: pixels(pixmap_width, pixmap_height, 0), palette(palette_size > 1 ? palette_size : 1, from_rgb(0, 0, 0))
{
	clear_xpm_data();
}

/// @cond INTERNAL
namespace Global {
static const std::map<std::string, uint32_t> x11_colors = {
#include "x11_colors.inc"
};

}
/// @endcond

void Pixmap::load(const std::string & xpm_data)
{
	std::vector<std::string> lines;

	clear_xpm_data();

	size_t interspace_start = 0;
	size_t line_start = xpm_data.find('"');
	size_t line_stop = xpm_data.find('"', line_start + 1);
	while(line_start != std::string::npos && line_stop != std::string::npos) {
		interspaces.push_back(xpm_data.substr(interspace_start, line_start - interspace_start));
		interspace_start = line_stop + 1;

		lines.push_back(xpm_data.substr(line_start + 1, line_stop - line_start - 1));

		line_start = xpm_data.find('"', line_stop + 1);
		line_stop = xpm_data.find('"', line_start + 1);
	}
	interspaces.push_back(xpm_data.substr(interspace_start));

	load(lines);
}

void Pixmap::load(const std::vector<std::string> & xpm_lines)
{
	if(xpm_lines.empty()) {
		throw Exception("Value line is missing");
	}
	std::vector<std::string>::const_iterator line = xpm_lines.begin();
	std::vector<std::string> values;
	values_interspaces.clear();
	values_interspaces.push_back("");
	bool last_was_space_in_value = true;
	for(unsigned i = 0; i < line->size(); ++i) {
		const char & ch = (*line)[i];
		if(isspace(ch)) {
			if(!last_was_space_in_value) {
				values_interspaces.push_back("");
			}
			values_interspaces.back() += ch;
			last_was_space_in_value = true;
		} else {
			if(last_was_space_in_value) {
				values.push_back("");
			}
			values.back() += ch;
			last_was_space_in_value = false;
		}
	}
	++line;
	if(values.size() != 4) {
		throw Exception("Value line should be in format '<width> <height> <color_count> <char_per_pixel>'");
	}

	unsigned w = static_cast<unsigned>(strtoul(values[0].c_str(), nullptr, 10));
	unsigned h = static_cast<unsigned>(strtoul(values[1].c_str(), nullptr, 10));
	color_count = static_cast<unsigned>(strtoul(values[2].c_str(), nullptr, 10));
	unsigned cpp = static_cast<unsigned>(strtoul(values[3].c_str(), nullptr, 10));
	if(w == 0 || h == 0 || color_count == 0 || cpp == 0) {
		throw Exception("Values in value line should be integers and non-zero.");
	}

	std::map<std::string, unsigned> color_names;
	palette.clear();
	for(int color = 0; color < int(color_count); ++color) {
		if(line == xpm_lines.end()) {
			throw Exception("Color lines are missing or not enough");
		}

		if(line->size() <= cpp || !isspace((*line)[cpp])) {
			throw Exception("Color char should be followed by space in color table.");
		}
		std::string color_name = line->substr(0, cpp);

		std::vector<std::string> color_parts;
		std::vector<std::string> color_interspaces;
		color_interspaces.push_back("");
		bool last_was_space = true;
		for(unsigned i = cpp; i < line->size(); ++i) {
			const char & ch = (*line)[i];
			if(color_parts.size() < 2 && isspace(ch)) {
				if(!last_was_space) {
					color_interspaces.push_back("");
				}
				color_interspaces.back() += ch;
				last_was_space = true;
			} else {
				if(last_was_space) {
					color_parts.push_back("");
				}
				color_parts.back() += ch;
				last_was_space = false;
			}
		}
		if(!color_parts.empty() && !color_parts.back().empty() && !isspace(color_parts.back()[0])) {
			color_interspaces.push_back("");
			while(isspace(color_parts.back().back())) {
				color_interspaces.back() += color_parts.back().back();
				color_parts.back().erase(color_parts.back().size() - 1, 1);
			}
		}
		if(color_parts.size() < 1) {
			throw Exception("Color key is missing.");
		}
		if(color_parts.size() < 2) {
			throw Exception("Color value is missing.");
		}
		std::string key = color_parts[0];
		std::string value = color_parts[1];
		std::pair<std::string, std::pair<std::string, std::string> > result_pair;
		result_pair.first = color_interspaces[0];
		result_pair.second.first = color_interspaces[1];
		if(color_interspaces.size() > 2) {
			result_pair.second.second = color_interspaces[2];
		}
		colors_interspaces.push_back(result_pair);
		if(key != "c") {
			throw Exception("Only color key 'c' is supported.");
		}
		if(color_names.count(color_name) > 0) {
			throw Exception("Color <" + color_name + "> was found more than once.");
		}
		if(Global::x11_colors.count(value) > 0) {
			color_names[color_name] = static_cast<unsigned>(add_to(palette, Global::x11_colors.at(value)));
		} else if(value[0] == '#') {
			std::string number_value = value.substr(1);
			bool is_zero = true;
			for(std::string::const_iterator it = number_value.begin(); it != number_value.end(); ++it) {
				if(*it != '0') {
					is_zero = false;
					break;
				}
			}
			unsigned color_value = static_cast<unsigned>(strtoul(number_value.c_str(), nullptr, 16));
			if(color_value == 0 && !is_zero) {
				throw Exception("Color value <" + value + "> is invalid.");
			}
			color_names[color_name] = static_cast<unsigned>(add_to(palette, rgb_to_argb(color_value)));
		} else {
			throw Exception("Color value <" + value + "> is invalid.");
		}
		colors.push_back(color_name);
		++line;
	}

	row_count = 0;
	pixels = Map<unsigned>(w, h, 0);
	unsigned index = 0;
	unsigned rows = h;
	while(rows --> 0) {
		if(line == xpm_lines.end()) {
			throw Exception("Pixel rows are missing or not enough");
		}
		if(line->size() % cpp != 0) {
			throw Exception("Pixel value in a row is broken.");
		} else if(line->size() < cpp * w) {
			throw Exception("Pixel row is too small.");
		} else if(line->size() > cpp * w) {
			throw Exception("Pixel row is too large.");
		}
		for(unsigned col = 0; col < w; ++col) {
			std::string current_pixel = line->substr(col * cpp, cpp);
			if(color_names.count(current_pixel) == 0) {
				throw Exception("Pixel value is invalid.");
			}
			pixels.data()[index++] = color_names[current_pixel];
		}
		++row_count;
		++line;
	}
	if(line != xpm_lines.end()) {
		throw Exception("Extra pixel rows are found.");
	}
}

void recreate_xpm_data(Pixmap * pixmap)
{
	pixmap->color_count = 0;
	pixmap->colors.clear();
	pixmap->colors_interspaces.clear();

	pixmap->row_count = pixmap->pixels.height();

	std::vector<std::string> tmp_values_interspaces(4, " ");
	tmp_values_interspaces[0] = "";
	pixmap->values_interspaces.swap(tmp_values_interspaces);

	std::vector<std::string> tmp_interspaces(1 + pixmap->palette.size() + 1 + pixmap->pixels.height(), ",\n");
	tmp_interspaces[0] = 
		"/* XPM */\n"
		"static char * xpm[] = {\n"
		"/* Values */\n"
		;
	tmp_interspaces[1] = 
		",\n"
		"/* Colors */\n"
		;
	tmp_interspaces[1 + pixmap->palette.size()] =
		",\n"
		"/* Pixels */\n"
		;
	tmp_interspaces.back() = "\n};\n";
	pixmap->interspaces.swap(tmp_interspaces);
}

std::string Pixmap::save() const
{
	std::string result;
	std::vector<std::string>::const_iterator interspace = interspaces.begin();
	if(interspace == interspaces.end()) {
		recreate_xpm_data(const_cast<Pixmap*>(this));
		interspace = interspaces.begin();
	}
	result += *interspace;

	std::ostringstream values;
	values << '"';
	values << values_interspaces[0] << pixels.width();
	values << values_interspaces[1] << pixels.height();
	values << values_interspaces[2] << palette.size();
	values << values_interspaces[3] << 1;
	if(values_interspaces.size() >= 5) {
		values << values_interspaces[4];
	}
	values << '"';
	result += values.str();

	++interspace;
	if(interspace == interspaces.end()) {
		return result;
	}
	result += *interspace;

	std::vector<std::string>::const_iterator it_color_key = colors.begin();
	std::vector<std::pair<std::string, std::pair<std::string, std::string> > >::const_iterator it_color_interspace = colors_interspaces.begin();
	char free_color_key = 'a';
	unsigned current_color_index = 0;
	std::map<unsigned, std::string> new_color_names;
	for(std::vector<Color>::const_iterator current_color = palette.begin(); current_color != palette.end(); ++current_color) {
		std::string color_key;
		if(it_color_key != colors.end()) {
			color_key = *it_color_key;
			++it_color_key;
		} else {
			color_key = free_color_key;
			new_color_names[current_color_index] = color_key;
			++free_color_key;
		}

		std::pair<std::string, std::pair<std::string, std::string> > color_interspace;
		if(it_color_interspace != colors_interspaces.end()) {
			color_interspace = *it_color_interspace;
			++it_color_interspace;
		} else {
			color_interspace.first = " ";
			color_interspace.second.first = " ";
		}
		result += std::string("\"") + color_key + color_interspace.first + "c" + color_interspace.second.first;
		if(is_transparent(*current_color)) {
			result += "None";
		} else {
			std::ostringstream color_value;
			color_value << std::hex << std::setw(6) << std::setfill('0') << (*current_color & 0xffffff);
			result += "#" + color_value.str();
		}
		result += color_interspace.second.second + '"';

		bool print_original_interspace = (current_color_index < color_count - 1) || (current_color_index == palette.size() - 1);
		if(print_original_interspace) {
			++interspace;
			if(interspace == interspaces.end()) {
				return result;
			}
			result += *interspace;
		} else {
			result += ",\n";
		}
		++current_color_index;
	}

	unsigned row_size = 0;
	unsigned current_row = 0;
	for(std::vector<unsigned>::const_iterator current_pixel = pixels.begin(); current_pixel != pixels.end(); ++current_pixel) {
		if(row_size == 0) {
			result += '"';
		}
		if(*current_pixel >= colors.size()) {
			result += new_color_names[*current_pixel];
		} else {
			result += colors[*current_pixel];
		}
		++row_size;
		if(row_size >= pixels.width()) {
			result += '"';
			bool is_last_one = current_row == pixels.height() - 1;
			bool print_original_interspace = (current_row < row_count - 1) || (current_row == pixels.height() - 1);
			if(is_last_one) {
				std::string last_interspace;
				while(interspace != interspaces.end()) {
					last_interspace = *interspace++;
				}
				result += last_interspace;
			} else if(print_original_interspace) {
				++interspace;
				if(interspace == interspaces.end()) {
					return result;
				}
				result += *interspace;
			} else {
				result += ",\n";
			}

			row_size = 0;
			++current_row;
		}
	}
	return result;
}

}
