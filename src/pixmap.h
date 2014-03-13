#pragma once
#include "map.h"
#include <vector>
#include <string>

namespace Chthon { /// @defgroup Pixmap Pixmap
/// @{

/// @cond INTERNAL
struct XPMData {
	XPMData();
};
/// @endcond

/// Represents RGB color with optional transparency flag.
typedef uint32_t Color;
typedef uint8_t ColorComponent;
/// Makes colour from three RGB components.
Color from_rgb(ColorComponent r, ColorComponent g, ColorComponent b);
/// Returns red component of the colour.
uint8_t get_red(Color color);
/// Returns green component of the colour.
uint8_t get_green(Color color);
/// Returns blue component of the colour.
uint8_t get_blue(Color color);
/// Returns true if colour is transparent.
bool is_transparent(Color color);
/// Converts RGB color to an opaque ARGB quardruplet.
Color rgb_to_argb(Color color);

/** Stores pixmap and its palette.
 * Allows basic manipulation with pixels and palette colors.
 * Also loading and saving XPM files.
 */
class Pixmap {
public:
	/// Basic exception.
	struct Exception {
		std::string what;
		/// Constructs exception with given reason text.
		Exception(const std::string & reason) : what(reason) {}
	};

	Map<unsigned> pixels;
	std::vector<Color> palette;

	/// Constructs pixmap with given size, pallette of one black color, and filled with this color.
	Pixmap(unsigned w = 1, unsigned h = 1, unsigned palette_size = 1);

	/// Loads Pixmap from string which contains loaded XPM data.
	/// This way Pixmap can be later saved with as few differences as possible.
	void load(const std::string & xpm_data);
	/// Loads Pixmap from XPM lines.
	/// This way Pixmap can be later saved with as few differences as possible.
	void load(const std::vector<std::string> & xpm_lines);
	/// Saves pixmap as an XPM data string.
	std::string save() const;
private:
	unsigned color_count;
	unsigned row_count;
	std::vector<std::string> interspaces;
	std::vector<std::string> colors;
	std::vector<std::string> values_interspaces;
	std::vector<std::pair<std::string, std::pair<std::string, std::string> > > colors_interspaces;
	void clear_xpm_data();
	friend void recreate_xpm_data(Pixmap * pixmap);
};

/// @}
}
