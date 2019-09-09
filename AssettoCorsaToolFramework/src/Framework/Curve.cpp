#include <Framework/Curve.h>

using Framework::Curve;
using Framework::ErrorCode;

void Curve::ParseLUT(const std::string& LUTBuf)
{
	
}

void Curve::ParseLUT(const std::string& LUTBuf, ErrorCode& ec)
{
	try
	{
		ParseLUT(LUTBuf);
	}
	catch (const ErrorCode& e)
	{
		ec = e;
	}
}

Curve::Data_t Curve::GetValue(const Data_t ref) const
{
	Map_t::iterator::value_type below, above;

	// search the map for the value above and below
	for (auto it = m_values.cbegin(); it != m_values.cend();)
	{
		auto belowIt = it++;

		// first check to see if the ref exactly matches
		if (ref == belowIt->first)
			return belowIt->second;

		// no need to make a copy here
		const auto& aboveIt = it;

		// if there is not an above value, we cannot interpolate.
		if (aboveIt == m_values.cend())
			return 0;

		if (belowIt->first < ref &&
			aboveIt->first > ref)
		{
			// bingo, use linear interpolation to find the value
			auto frac = (ref - belowIt->first) / static_cast<float>(aboveIt->first - belowIt->first); // https://goodaids.club/i/f2oi94wa30.png
			return (frac * (aboveIt->second - belowIt->second)) + belowIt->second;
		}
	}

	// we didn't find the value
	return 0;
}

const Curve::Map_t& Curve::GetValues() const noexcept
{
	return m_values;
}