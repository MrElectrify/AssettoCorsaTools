#ifndef FRAMEWORK_CURVE_H_
#define FRAMEWORK_CURVE_H_

/*
 *	Curve
 *	9/9/19 13:27
 */

#include <Framework/Error.h>

#include <map>
#include <string>

namespace Framework
{
	/*
	 *	Curve is an interpolated curve that receives curve data from
	 *	a LUT file. It functions in the same way as Kunos' curve, except
	 *	with a different API
	 */
	class Curve
	{
	public:
		// no explicit constructor necessary

		// parses the LUT file. throws ErrorCode on error
		void ParseLUT(const std::string& LUTBuf);
		// parses the LUT file. returns ErrorCode in ec on error
		void ParseLUT(const std::string& LUTBuf, ErrorCode& ec) noexcept;
	private:
		// need order for interpolation, it makes it easier
		std::map<int, int> m_values;
	};
}

#endif