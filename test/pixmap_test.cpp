#include "../src/pixmap.h"
#include "../src/util.h"
#include "../src/test.h"
#include <sstream>
#include <iomanip>

SUITE(pixmap) {
using Chthon::Pixmap;

TEST(should_construct_color_from_rgb)
{
	Chthon::Color c = Chthon::from_rgb(255, 0, 255);
	EQUAL(c, 0xffff00ff);
}

TEST(should_get_red_component)
{
	Chthon::ColorComponent r = Chthon::get_red(0xff00ff);
	EQUAL(r, 0xff);
}

TEST(should_get_green_component)
{
	Chthon::ColorComponent g = Chthon::get_green(0xff0aff);
	EQUAL(g, 0x0a);
}

TEST(should_get_blue_component)
{
	Chthon::ColorComponent b = Chthon::get_blue(0xff00af);
	EQUAL(b, 0xaf);
}

TEST(should_get_transparency)
{
	ASSERT(Chthon::is_transparent(0xff00ff));
	ASSERT(!Chthon::is_transparent(0xffff00ff));
}

TEST(should_convert_from_rgb_to_argb)
{
	Chthon::Color c = Chthon::rgb_to_argb(0x3300ff);
	EQUAL(c, 0xff3300ff);
}

TEST(should_construct_transparent_color_from_default_color)
{
	Chthon::Color c = Chthon::Color();
	ASSERT(Chthon::is_transparent(c));
}


TEST(should_make_palette_with_one_opaque_black_color_by_default)
{
	Pixmap pixmap(2, 2);
	EQUAL(pixmap.palette.size(), 1u);
	EQUAL(pixmap.palette[0], 0xff000000);
}

TEST(should_fill_image_with_default_color_on_create)
{
	Pixmap pixmap(2, 2);
	for(int x = 0; x < 2; ++x) {
		for(int y = 0; y < 2; ++y) {
			EQUAL(pixmap.pixels.cell(x, y), 0u);
		}
	}
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
	Pixmap pixmap;
	pixmap.load(xpm_lines);
	EQUAL(pixmap.palette.size(), 2u);
	EQUAL(pixmap.palette[0], 0xffff0000);
	EQUAL(pixmap.palette[1], 0xff00ff00);
	EQUAL(pixmap.pixels.width(), 3u);
	EQUAL(pixmap.pixels.height(), 2u);
	EQUAL(pixmap.pixels.cell(0, 0), 1u);
	EQUAL(pixmap.pixels.cell(1, 0), 0u);
	EQUAL(pixmap.pixels.cell(2, 0), 1u);
	EQUAL(pixmap.pixels.cell(0, 1), 0u);
	EQUAL(pixmap.pixels.cell(1, 1), 1u);
	EQUAL(pixmap.pixels.cell(2, 1), 0u);
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
	Pixmap pixmap;
	pixmap.load(xpm_lines);
	EQUAL(pixmap.palette.size(), 2u);
	EQUAL(pixmap.palette[0], Chthon::Color());
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
	Pixmap pixmap;
	pixmap.load(xpm_lines);
	EQUAL(pixmap.palette.size(), 2u);
	EQUAL(pixmap.palette[0], 0xff000080);
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
	Pixmap pixmap;
	pixmap.load(xpm_lines);
	EQUAL(pixmap.palette.size(), 2u);
	EQUAL(pixmap.palette[0], 0xff90ee90);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	EQUAL(pixmap.palette.size(), 2u);
	EQUAL(pixmap.palette[0], 0xffff0000);
	EQUAL(pixmap.palette[1], 0xff00ff00);
	EQUAL(pixmap.pixels.width(), 3u);
	EQUAL(pixmap.pixels.height(), 2u);
	EQUAL(pixmap.pixels.cell(0, 0), 1u);
	EQUAL(pixmap.pixels.cell(1, 0), 0u);
	EQUAL(pixmap.pixels.cell(2, 0), 1u);
	EQUAL(pixmap.pixels.cell(0, 1), 0u);
	EQUAL(pixmap.pixels.cell(1, 1), 1u);
	EQUAL(pixmap.pixels.cell(2, 1), 0u);
}

TEST(should_throw_exception_when_value_line_is_missing_in_xpm)
{
	std::vector<std::string> xpm_lines;
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Pixel rows are missing or not enough");
	}
}

TEST(should_throw_exception_when_value_count_is_not_four_in_xpm)
{
	static const char * xpm[] = {
	"3 2",
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
		EQUAL(e.what, "Value line should be in format '<width> <height> <color_count> <char_per_pixel>'");
	}
}

TEST(should_throw_exception_when_values_are_not_integer_in_xpm)
{
	static const char * xpm[] = {
	"3 2 a 1",
	};
	std::vector<std::string> xpm_lines(xpm, xpm + size_of_array(xpm));
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	CATCH(pixmap.load(xpm_lines), const Pixmap::Exception & e) {
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.pixels.resize(3, 3);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.palette[0] =  0xff000000;
	pixmap.palette[1] =  0xffffffff;
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.palette.push_back(0xff0000ff);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.pixels.resize(4, 3);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.pixels.resize(2, 3);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.pixels.resize(3, 3);
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
	Pixmap pixmap;
	pixmap.load(xpm_data);
	pixmap.pixels.resize(3, 1);
	std::string save_data = pixmap.save();
	EQUAL(save_data, std::string(xpm_result));
}

}

