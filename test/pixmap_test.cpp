#include "../src/pixmap.h"
#include "../src/util.h"
#include "../src/test.h"
#include <sstream>
#include <iomanip>

namespace Chthon {

static std::string to_string(const Chthon::Pixmap::Color & color)
{
	if(color.transparent) {
		return "None";
	} else {
		std::ostringstream color_value;
		color_value << std::hex << std::setw(6) << std::setfill('0') << (color.argb() & 0xffffff);
		return "#" + color_value.str();
	}
}

}

SUITE(pixmap) {
using Chthon::Pixmap;

TEST(should_construct_color_from_argb)
{
	Pixmap::Color c = Pixmap::Color::from_argb(0x00ff00ff);
	ASSERT(c.transparent);
	EQUAL((int)c.r, 0);
	EQUAL((int)c.g, 0);
	EQUAL((int)c.b, 0);
}

TEST(should_construct_color_from_rgb)
{
	Pixmap::Color c = Pixmap::Color::from_rgb(0x00ff00ff);
	ASSERT(!c.transparent);
	EQUAL((int)c.r, 255);
	EQUAL((int)c.g, 0);
	EQUAL((int)c.b, 255);
}

TEST(should_construct_transparent_color_from_argb_with_zero_alpha)
{
	Pixmap::Color c = Pixmap::Color::from_argb(0xf0ff00ff);
	ASSERT(!c.transparent);
	EQUAL((int)c.r, 255);
	EQUAL((int)c.g, 0);
	EQUAL((int)c.b, 255);
}

TEST(should_get_transparent_rgba_as_transparent_black)
{
	Pixmap::Color c;
	EQUAL(c.argb(), 0u);
}

TEST(should_get_rgba_from_opaque_color)
{
	Pixmap::Color c(255, 0, 255);
	EQUAL(c.argb(), 0xffff00ff);
}

TEST(should_make_palette_with_one_opaque_black_color_by_default)
{
	Pixmap pixmap(2, 2);
	EQUAL(pixmap.color_count(), 1u);
	EQUAL(pixmap.color(0), Pixmap::Color(0, 0, 0));
}

TEST(should_fill_image_with_default_color_on_create)
{
	Pixmap pixmap(2, 2);
	for(unsigned x = 0; x < 2; ++x) {
		for(unsigned y = 0; y < 2; ++y) {
			EQUAL(pixmap.pixel(x, y), 0u);
		}
	}
}

TEST(should_fill_image_with_color)
{
	Pixmap pixmap(2, 2);
	unsigned color = pixmap.add_color(Pixmap::Color(255, 255, 255));
	pixmap.fill(color);
	for(unsigned x = 0; x < 2; ++x) {
		for(unsigned y = 0; y < 2; ++y) {
			EQUAL(pixmap.pixel(x, y), color);
		}
	}
}

TEST(should_resize_image)
{
	Pixmap pixmap(2, 2);
	pixmap.resize(4, 4);
	EQUAL(pixmap.width(), 4u);
	EQUAL(pixmap.height(), 4u);
}

TEST(should_not_resize_image_if_width_is_zero)
{
	Pixmap pixmap(2, 2);
	pixmap.resize(0, 4);
	EQUAL(pixmap.width(), 2u);
	EQUAL(pixmap.height(), 2u);
}

TEST(should_not_resize_image_if_height_is_zero)
{
	Pixmap pixmap(2, 2);
	pixmap.resize(4, 0);
	EQUAL(pixmap.width(), 2u);
	EQUAL(pixmap.height(), 2u);
}

TEST(should_keep_existing_pixels_when_resizing_image)
{
	Pixmap pixmap(2, 2, 4);
	pixmap.set_pixel(0, 0, 0);
	pixmap.set_pixel(1, 0, 1);
	pixmap.set_pixel(0, 1, 2);
	pixmap.set_pixel(1, 1, 3);
	pixmap.resize(4, 4);
	EQUAL(pixmap.pixel(0, 0), 0u);
	EQUAL(pixmap.pixel(1, 0), 1u);
	EQUAL(pixmap.pixel(0, 1), 2u);
	EQUAL(pixmap.pixel(1, 1), 3u);
}

TEST(should_fill_new_pixels_with_default_index_when_resizing_image)
{
	Pixmap pixmap(2, 2, 2);
	pixmap.set_pixel(0, 0, 1);
	pixmap.set_pixel(1, 0, 1);
	pixmap.set_pixel(0, 1, 1);
	pixmap.set_pixel(1, 1, 1);
	pixmap.resize(2, 3);
	EQUAL(pixmap.pixel(0, 2), 0u);
	EQUAL(pixmap.pixel(1, 2), 0u);
}

TEST(should_get_pixel_index)
{
	Pixmap pixmap(2, 2, 2);
	pixmap.set_pixel(0, 0, 1);
	EQUAL(pixmap.pixel(0, 0), 1u);
}

TEST(should_set_pixel_index)
{
	Pixmap pixmap(2, 2, 2);
	pixmap.set_pixel(0, 0, 1);
	EQUAL(pixmap.pixel(0, 0), 1u);
}

TEST(should_flood_fill_area)
{
	Pixmap pixmap(8, 8, 3);
	unsigned data[64] = {
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 1, 1, 1, 0, 0, 0, 
		0, 1, 0, 0, 0, 1, 0, 0, 
		0, 0, 1, 0, 0, 1, 0, 0, 
		0, 0, 0, 1, 0, 1, 0, 0, 
		0, 0, 1, 0, 0, 0, 1, 0, 
		0, 1, 0, 0, 0, 0, 1, 0, 
		0, 1, 0, 0, 0, 0, 1, 0, 
	};
	for(unsigned i = 0; i < 64; ++i) {
		pixmap.set_pixel(i % 8, i / 8, data[i]);
	}
	pixmap.floodfill(5, 5, 2);
	unsigned result[64] = {
		0, 0, 0, 0, 0, 0, 0, 0, 
		0, 0, 1, 1, 1, 0, 0, 0, 
		0, 1, 2, 2, 2, 1, 0, 0, 
		0, 0, 1, 2, 2, 1, 0, 0, 
		0, 0, 0, 1, 2, 1, 0, 0, 
		0, 0, 1, 2, 2, 2, 1, 0, 
		0, 1, 2, 2, 2, 2, 1, 0, 
		0, 1, 2, 2, 2, 2, 1, 0, 
	};
	for(unsigned i = 0; i < 64; ++i) {
		EQUAL(pixmap.pixel(i % 8, i / 8), result[i]);
	}
}

TEST(should_change_palette_color_value)
{
	Pixmap pixmap(2, 2, 2);
	pixmap.set_color(1, Pixmap::Color(0, 255, 0));
	EQUAL(pixmap.color(1), Pixmap::Color(0, 255, 0));
}

TEST(should_add_new_color_to_palette)
{
	Pixmap pixmap(2, 2, 2);
	unsigned index = pixmap.add_color(Pixmap::Color(0, 255, 0));
	EQUAL(index, 2u);
	EQUAL(pixmap.color(2), Pixmap::Color(0, 255, 0));
}

TEST(should_consider_transparent_color_transparent)
{
	Pixmap pixmap(2, 2, 2);
	Pixmap::Color c(255, 255, 255);
	c.transparent = true;
	pixmap.set_color(0, c);
	ASSERT(pixmap.is_transparent_color(0));
}

TEST(should_consider_default_color_transparent)
{
	Pixmap pixmap(2, 2, 2);
	pixmap.set_color(0, Pixmap::Color());
	ASSERT(pixmap.is_transparent_color(0));
}

}

SUITE(load_xpm) {
using Chthon::Pixmap;
using Chthon::size_of_array;

TEST(should_load_pixmap_from_text_lines)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	"# c #00ff00",
	"#.#",
	".#."
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap(xpm_lines);
	EQUAL(pixmap.color_count(), 2u);
	EQUAL(pixmap.color(0), Pixmap::Color(255, 0, 0));
	EQUAL(pixmap.color(1), Pixmap::Color(0, 255, 0));
	EQUAL(pixmap.width(), 3u);
	EQUAL(pixmap.height(), 2u);
	EQUAL(pixmap.pixel(0, 0), 1u);
	EQUAL(pixmap.pixel(1, 0), 0u);
	EQUAL(pixmap.pixel(2, 0), 1u);
	EQUAL(pixmap.pixel(0, 1), 0u);
	EQUAL(pixmap.pixel(1, 1), 1u);
	EQUAL(pixmap.pixel(2, 1), 0u);
}

TEST(should_recognize_none_color)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c None",
	"# c #00ff00",
	"#.#",
	".#."
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap(xpm_lines);
	EQUAL(pixmap.color_count(), 2u);
	EQUAL(pixmap.color(0), Pixmap::Color());
}

TEST(should_recognize_x11_color_names)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c navy",
	"# c #00ff00",
	"#.#",
	".#."
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap(xpm_lines);
	EQUAL(pixmap.color_count(), 2u);
	EQUAL(pixmap.color(0), Pixmap::Color(0, 0, 0x80));
}

TEST(should_recognize_x11_color_names_with_spaces)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c light green",
	"# c #00ff00",
	"#.#",
	".#."
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap(xpm_lines);
	EQUAL(pixmap.color_count(), 2u);
	EQUAL(pixmap.color(0), Pixmap::Color(0x90, 0xee, 0x90));
}

TEST(should_load_pixmap_from_xpm_file)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"#	c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	EQUAL(pixmap.color_count(), 2u);
	EQUAL(pixmap.color(0), Pixmap::Color(255, 0, 0));
	EQUAL(pixmap.color(1), Pixmap::Color(0, 255, 0));
	EQUAL(pixmap.width(), 3u);
	EQUAL(pixmap.height(), 2u);
	EQUAL(pixmap.pixel(0, 0), 1u);
	EQUAL(pixmap.pixel(1, 0), 0u);
	EQUAL(pixmap.pixel(2, 0), 1u);
	EQUAL(pixmap.pixel(0, 1), 0u);
	EQUAL(pixmap.pixel(1, 1), 1u);
	EQUAL(pixmap.pixel(2, 1), 0u);
}

TEST(should_throw_exception_when_value_line_is_missing_in_xpm)
{
	std::vector<std::string> xpm_lines;
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Value line is missing");
	}
}

TEST(should_throw_exception_when_colour_lines_are_missing_or_not_enough_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color lines are missing or not enough");
	}
}

TEST(should_throw_exception_when_pixel_lines_are_missing_or_not_enough_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	"# c #00ff00",
	"#.#"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel rows are missing or not enough");
	}
}

TEST(should_throw_exception_when_value_count_is_not_four_in_xpm)
{
	static const char * xpm[] = {
	"3 2",
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Value line should be in format '<width> <height> <color_count> <char_per_pixel>'");
	}
}

TEST(should_throw_exception_when_values_are_not_integer_in_xpm)
{
	static const char * xpm[] = {
	"3 2 a 1",
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Values in value line should be integers and non-zero.");
	}
}

TEST(should_throw_exception_when_colours_are_repeated_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	". c #00ff00",
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color <.> was found more than once.");
	}
}

TEST(should_throw_exception_when_there_is_no_space_after_colour_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	".c #ff0000"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color char should be followed by space in color table.");
	}
}

TEST(should_throw_exception_when_colour_key_is_not_c_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". s #ff0000"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Only color key 'c' is supported.");
	}
}

TEST(should_throw_exception_when_colour_key_is_missing_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". "
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color key is missing.");
	}
}

TEST(should_throw_exception_when_colour_value_is_missing_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color value is missing.");
	}
}

TEST(should_throw_exception_when_colour_value_is_invalid_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c invalid"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Color value <invalid> is invalid.");
	}
}

TEST(shoudl_throw_exception_when_pixel_row_length_is_too_small_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	"# c #00ff00",
	"#.",
	".#"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel row is too small.");
	}
}

TEST(shoudl_throw_exception_when_pixel_row_length_is_too_large_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	"# c #00ff00",
	"#.##",
	".#"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel row is too large.");
	}
}

TEST(shoudl_throw_exception_when_pixel_row_count_is_too_large_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 1",
	". c #ff0000",
	"# c #00ff00",
	"#.#",
	"#.#",
	".##"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Extra pixel rows are found.");
	}
}

TEST(shoudl_throw_exception_when_pixel_is_broken_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 2",
	"a. c #ff0000",
	"b# c #00ff00",
	"b#a.b#",
	"a.b#a"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel value in a row is broken.");
	}
}

TEST(shoudl_throw_exception_when_pixel_is_invalid_in_xpm)
{
	static const char * xpm[] = {
	"3 2 2 2",
	"a. c #ff0000",
	"b# c #00ff00",
	"b#a.b#",
	"a.b#a$"
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	CATCH(Pixmap(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel value is invalid.");
	}
}

}

SUITE(save_xpm) {
using Chthon::Pixmap;
using Chthon::size_of_array;

TEST(should_save_pixmap_exactly_when_intact)
{
	static const char * xpm_data = 
		"/* XPM */\n"
		"static char * xpm[] = {\n"
		"/* Values */\n"
		"\"3 2 2 1\",\n"
		"/* Colors */\n"
		"\". c #ff0000\" /* red */,\n"
		"\"# c #00ff00\",\n"
		"/* Pixels */\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_data));
}

TEST(should_save_none_color)
{
	static const char * xpm_data = 
		"/* XPM */\n"
		"static char * xpm[] = {\n"
		"/* Values */\n"
		"\"3 2 2 1\",\n"
		"/* Colors */\n"
		"\". c None\" /* transparent */,\n"
		"\"# c #00ff00\",\n"
		"/* Pixels */\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_data));
}

TEST(should_keep_format_of_values_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\" 3    2 2  1 \",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\" 3    3 2  1 \",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\",\n"
		"\"...\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.resize(3, 3);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_keep_format_of_colours_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\".   c  #ff0000\",\n"
		"\"# c #00ff00   \",\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\".   c  #000000\",\n"
		"\"# c #ffffff   \",\n"
		"\"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.set_color(0, Pixmap::Color(0, 0, 0));
	pixmap.set_color(1, Pixmap::Color(255, 255, 255));
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_insert_line_breaks_when_colours_are_added_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\", \"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"3 2 3 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"a c #0000ff\", \"#.#\",\n"
		"\".#.\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.add_color(Pixmap::Color(0, 0, 255));
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_lengthen_pixel_lines_when_width_is_increased_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 3 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\"...\",\n"
		"\".#.\"\n"
		"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"4 3 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#.\",\"....\",\n"
		"\".#..\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.resize(4, 3);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_shorten_pixel_lines_when_width_is_decreased_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 3 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\"...\",\n"
		"\".#.\"\n"
		"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"2 3 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.\",\"..\",\n"
		"\".#\"\n"
		"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.resize(2, 3);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_add_new_lines_when_height_is_increased_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"3 3 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\",\n"
		"\"...\"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.resize(3, 3);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

TEST(should_remove_text_constants_when_height_is_decreased_when_saving_xpm)
{
	static const char * xpm_data = 
		"static char * xpm[] = {\n"
		"\"3 2 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\",\n"
		"\".#.\"};\n"
		;
	static const char * xpm_result = 
		"static char * xpm[] = {\n"
		"\"3 1 2 1\",\n"
		"\". c #ff0000\",\n"
		"\"# c #00ff00\",\n"
		"\"#.#\"};\n"
		;
	Pixmap pixmap(xpm_data);
	pixmap.resize(3, 1);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

}

