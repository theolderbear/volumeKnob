#ifndef RGBColor_h
#define RGBColor_h

class RGBColor {
	public:
		RGBColor(int red, int green, int blu);
    int red();
    int green();
    int blu();
	private:
		int _red;
		int _green;
		int _blu;
};
#endif