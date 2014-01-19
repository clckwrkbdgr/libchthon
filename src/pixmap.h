#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>

struct XPMData {
	unsigned color_count;
	unsigned row_count;
	std::vector<std::string> interspaces;
	std::vector<std::string> colors;
	std::vector<std::string> values_interspaces;
	std::vector<std::pair<std::string, std::pair<std::string, std::string> > > colors_interspaces;
	XPMData();
};

class Pixmap {
public:
	struct Color {
		bool transparent;
		uint8_t r, g, b;
		Color();
		Color(uint8_t c_r, uint8_t c_g, uint8_t c_b);
		uint32_t argb() const;
		bool operator==(const Color & other) const;
		static Color from_argb(uint32_t color);
		static Color from_rgb(uint32_t color);
	};
	struct Exception {
		std::string what;
		Exception(const std::string & reason) : what(reason) {}
	};

	Pixmap(const std::string & xpm_data);
	Pixmap(const std::vector<std::string> & xpm_lines);
	Pixmap(unsigned w, unsigned h, unsigned palette_size = 1);
	bool valid(unsigned x, unsigned y) const;
	bool valid_color_index(unsigned color_index) const;
	unsigned width() const;
	unsigned height() const;
	unsigned pixel(unsigned x, unsigned y) const;

	bool resize(unsigned new_width, unsigned new_height);
	bool fill(unsigned index);
	bool set_pixel(unsigned x, unsigned y, unsigned index);
	bool floodfill(unsigned x, unsigned y, unsigned index);

	unsigned color_count() const;
	Color color(unsigned index) const;
	bool is_transparent_color(unsigned index) const;
	unsigned add_color(Color new_color);
	bool set_color(unsigned index, Color new_color);

	std::string save() const;
private:
	unsigned w, h;
	std::vector<unsigned> pixels;
	std::vector<Color> palette;

	XPMData xpm;

	void load_from_xpm_data(const std::string & xpm_data);
	void load_from_xpm_lines(const std::vector<std::string> & xpm_lines);
};
