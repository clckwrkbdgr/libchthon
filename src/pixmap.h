#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <memory>

namespace Chthon { /// @defgroup Pixmap Pixmap
/// @{

/// @cond INTERNAL
struct XPMData {
	unsigned color_count;
	unsigned row_count;
	std::vector<std::string> interspaces;
	std::vector<std::string> colors;
	std::vector<std::string> values_interspaces;
	std::vector<std::pair<std::string, std::pair<std::string, std::string> > > colors_interspaces;
	XPMData();
};
/// @endcond

/** Stores pixmap and its palette.
 * Allows basic manipulation with pixels and palette colors.
 * Also loading and saving XPM files.
 */
class Pixmap {
public:
	/// Represents RGB color with optional transparency flag.
	struct Color {
		bool transparent;
		uint8_t r, g, b;
		/// Constructs transparent black color.
		Color();
		/// Constructs color using its RGB components.
		Color(uint8_t c_r, uint8_t c_g, uint8_t c_b);
		/// Returns ARGB value for color.
		uint32_t argb() const;
		/// Compares color with another.
		bool operator==(const Color & other) const;
		/// Compares color with another.
		bool operator!=(const Color & other) const;
		/// Constructs color from ARGB value.
		static Color from_argb(uint32_t color);
		/// Constructs color from RGB value.
		static Color from_rgb(uint32_t color);
	};
	/// Basic exception.
	struct Exception {
		std::string what;
		/// Constructs exception with given reason text.
		Exception(const std::string & reason) : what(reason) {}
	};

	/// Constructs Pixmap from string which contains loaded XPM data.
	/// This way constructed Pixmap can be later saved with as few differences as possible.
	Pixmap(const std::string & xpm_data);
	/// Constructs Pixmap from XPM lines.
	Pixmap(const std::vector<std::string> & xpm_lines);
	/// Constructs pixmap with given size, pallette of one black color, and filled with this color.
	Pixmap(unsigned w, unsigned h, unsigned palette_size = 1);
	/// Returns true if point is inside pixmap, otherwise returns false.
	bool valid(unsigned x, unsigned y) const;
	/// Returns true if color index is valid (i.e. is stored in palette).
	bool valid_color_index(unsigned color_index) const;
	/// Returns pixmap width.
	unsigned width() const;
	/// Returns pixmap height.
	unsigned height() const;
	/// Returns pixel color index at given coords. If coords are invalid, color index 0 is returned.
	unsigned pixel(unsigned x, unsigned y) const;

	/** Resizes pixmap to a new size.
	 * If new width or height is greater than the old one, new pixels are filled with color index 0.
	 */
	bool resize(unsigned new_width, unsigned new_height);
	/// Fills pixmap with given color index.
	bool fill(unsigned index);
	/// Sets pixel index at given coords.
	bool set_pixel(unsigned x, unsigned y, unsigned index);
	/** Fills area using flood fill algorithm using specified color index.
	 */
	bool floodfill(unsigned x, unsigned y, unsigned index);

	/// Returns palette size.
	unsigned color_count() const;
	/// Return color value for its index.
	Color color(unsigned index) const;
	/// Returns true if color at the index is transparent.
	bool is_transparent_color(unsigned index) const;
	/// Appends color to palette and returns its index.
	unsigned add_color(Color new_color);
	/// Update stored color.
	bool set_color(unsigned index, Color new_color);

	/// Saves pixmap as an XPM data string.
	std::string save() const;
private:
	unsigned w, h;
	std::vector<unsigned> pixels;
	std::vector<Color> palette;

	XPMData xpm;

	void load_from_xpm_data(const std::string & xpm_data);
	void load_from_xpm_lines(const std::vector<std::string> & xpm_lines);
};

/// @}
}
