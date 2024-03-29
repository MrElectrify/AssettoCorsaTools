#ifndef FRAMEWORK_CURVE_H_
#define FRAMEWORK_CURVE_H_

/*
 *	Curve
 *	9/9/19 13:27
 */

#include <Framework/Error.h>

#include <cstdint>
#include <istream>
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
		using Data_t = int32_t;
		using Map_t = std::map<Data_t, Data_t>;

		// parses the LUT file via a stream. throws ErrorCode on error
		void ParseLUT(std::istream& lutFile);
		// parses the LUT file via a stream. returns ErrorCode in ec on error
		void ParseLUT(std::istream& lutFile, ErrorCode& ec);

		// returns the min reference
		Data_t GetMinRef() const;
		// returns the max reference
		Data_t GetMaxRef() const;

		// returns the interpolated value at a point. returns 0 if the ref is out of range
		Data_t GetValue(const Data_t ref) const;

		// returns the list of values
		const Map_t& GetValues() const noexcept;
	private:
		// need order for interpolation, it makes it easier
		Map_t m_values;
	};
}

#endif