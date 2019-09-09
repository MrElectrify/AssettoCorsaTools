#include <Framework/Curve.h>

#include <sstream>

using Framework::Curve;
using Framework::ErrorCode;

void Curve::ParseLUT(const std::string& LUTbuf)
{
	// a stringstream makes this a bit cleaner
	std::stringstream LUTss(LUTbuf);

	std::string workingString;

	while (true)
	{
		// if we reach the end, we are done
		if (std::getline(LUTss, workingString).fail() == true)
			break;

		auto splitPos = workingString.find('|');

		// weed out any empty lines and invalid lines
		if (splitPos == std::string::npos)
			continue;

		const auto reference = atoi(workingString.substr(0, splitPos).c_str());
		const auto value = atoi(workingString.substr(splitPos + 1).c_str());

		m_values.emplace(reference, value);
	}
}

void Curve::ParseLUT(const std::string& LUTbuf, ErrorCode& ec)
{
	try
	{
		ParseLUT(LUTbuf);
	}
	catch (const ErrorCode& e)
	{
		ec = e;
	}
}

Curve::Data_t Curve::GetMinRef() const
{
	// make sure we have values
	if (m_values.size() == 0)
		return 0;

	return m_values.cbegin()->first;
}

Curve::Data_t Curve::GetMaxRef() const
{
	// make sure we have values
	if (m_values.size() == 0)
		return 0;

	return (--m_values.cend())->first;
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
			return static_cast<Data_t>((frac * (aboveIt->second - belowIt->second)) + belowIt->second);
		}
	}

	// we didn't find the value
	return 0;
}

const Curve::Map_t& Curve::GetValues() const noexcept
{
	return m_values;
}