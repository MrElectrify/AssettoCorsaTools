#include <Framework/Curve.h>
#include <Framework/Files/FileDecrypter.h>

#include "inipp.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

/*
 *	Shift Optimizer
 *	9/8/19 22:40
 */

using Framework::Curve;
using Framework::ErrorCode;
using Framework::Files::File;
using Framework::Files::FileDecrypter;

std::string GetWorkingDirectory()
{
	// get the current folder name, if not specified by the 
	std::string folderName;

	folderName = std::filesystem::current_path().string();

	// make sure no slashes are in the folder name
	auto lastSlash = folderName.find_last_of("/\\", std::string::npos, 2);

	if (lastSlash != std::string::npos)
	{
		folderName = folderName.substr(lastSlash + 1);
	}

	return folderName;
}

// returns false on failure, with the failure in ec
bool GetRedline(FileDecrypter& decrypter, int& redline, ErrorCode& ec)
{
	// find redline in engine.ini
	const auto engine = decrypter.DecryptFile("engine.ini", ec);

	// check for errors
	if (ec != Framework::ErrorCode_SUCCESS)
		return false;

	// place the string in a stringstream to assist the ini parser
	std::stringstream iniFile(engine.GetContents());

	inipp::Ini<char> iniParser;
	
	// parse the ini for ENGINE_DATA->LIMITER
	iniParser.parse(iniFile);

	auto res = inipp::extract(iniParser.sections["ENGINE_DATA"]["LIMITER"], redline);

	if (res == false)
		ec = ErrorCode(Framework::ErrorCode_FORMAT);

	return res;
}

// returns false on failure, with the failure in ec
//					  forward final
bool GetGearRatios(FileDecrypter& decrypter, std::pair<std::vector<float>, float>& gearRatios, ErrorCode& ec)
{
	// find gearing information in drivetrain.ini
	const auto drivetrain = decrypter.DecryptFile("drivetrain.ini", ec);

	// check for errors
	if (ec != Framework::ErrorCode_SUCCESS)
		return false;

	// place the string in a stringstream to assist the ini parser
	std::stringstream iniFile(drivetrain.GetContents());

	inipp::Ini<char> iniParser;

	// parse the ini for GEARS->COUNT, GEARS->GEAR_[1-COUNT], and GEARS->FINAL
	iniParser.parse(iniFile);

	// only call operator[] once, we will be referencing GEARS a lot
	auto gears = iniParser.sections["GEARS"];

	int gearCount = -1;

	// get the number of gears
	if (inipp::extract(gears["COUNT"], gearCount) == false || gearCount < 0)
	{
		ec = ErrorCode(Framework::ErrorCode_FORMAT);
		return false;
	}

	std::string gearPrefix = "GEAR_";
	gearRatios.first.resize(gearCount);
	for (size_t gear = 1; gear <= gearCount; ++gear)
	{
		// extract ratios, and add them to the end
		if (inipp::extract(gears[gearPrefix + std::to_string(gear)], gearRatios.first[gear - 1]) == false)
		{
			ec = ErrorCode(Framework::ErrorCode_FORMAT);
			return false;
		}
	}

	// get the final drive ratio
	if (inipp::extract(gears["FINAL"], gearRatios.second) == false)
	{
		ec = ErrorCode(Framework::ErrorCode_FORMAT);
		return false;
	}

	return true;
}

// returns false on failure, with the failure in ec
bool GetTorqueCurve(FileDecrypter& decrypter, Curve& torqueCurve, ErrorCode& ec)
{
	// get power.lut
	const auto power = decrypter.DecryptFile("power.lut", ec);

	if (ec != Framework::ErrorCode_SUCCESS)
		return false;

	// put the file in a stringstream to assist the LUT parser
	std::stringstream powerFile(power.GetContents());

	torqueCurve.ParseLUT(powerFile, ec);

	return true;
}

int main(int argc, char* argv[])
{
	// make sure we don't have too many args
	if (argc > 3)
	{
		std::cout << "Usage: " << argv[0] << " [dataFile:string:data.acd] [path:string:wd]\n";
		return 1;
	}

	std::string dataFile = (argc >= 2) ? argv[1] : "data.acd";
	std::string directory = (argc >= 3) ? argv[2] : GetWorkingDirectory();

	ErrorCode ec;
	FileDecrypter decrypter(dataFile, directory, ec);

	// make sure we initialized the decrypter properly
	if (ec != Framework::ErrorCode_SUCCESS)
	{
		std::cout << "Error creating decrypter: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
		return 1;
	}

	int redline = -1;

	// get redline information
	if (GetRedline(decrypter, redline, ec) == false || redline < 0)
	{
		std::cout << "Failed to get redline information: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
		return 1;
	}

	std::pair<std::vector<float>, float> gearRatios;

	// get gear ratio information
	if (GetGearRatios(decrypter, gearRatios, ec) == false || gearRatios.first.size() == 0 || gearRatios.second < 0.f)
	{
		std::cout << "Failed to get gear ratio information: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
		return 1;
	}

	Curve torqueCurve;

	// get torque curve
	if (GetTorqueCurve(decrypter, torqueCurve, ec) == false || torqueCurve.GetMaxRef() == 0)
	{
		std::cout << "Failed to get torque curve: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
		return 1;
	}

	const auto redlineTorqueBase = torqueCurve.GetValue(redline);

	for (size_t i = 0; i < (gearRatios.first.size() - 1); ++i)
	{
		// figure out if we should go to redline
		const auto currRatio = gearRatios.first[i];
		const auto nextRatio = gearRatios.first[i + 1];

		auto nextGearRPM = redline * (nextRatio / currRatio);
		const auto nextGearTorqueBase = torqueCurve.GetValue(static_cast<Curve::Data_t>(nextGearRPM));

		const auto redlineTorque = redlineTorqueBase * currRatio;
		const auto nextGearTorque = nextGearTorqueBase * nextRatio;

		if (redlineTorque > nextGearTorque)
		{
			// we should go to redline, torque is greater
			std::cout << "Go to redline for gear " << i + 1 << '\n';
			continue;
		}

		// we could use this if they were in the same interpolation range, 
		// but that is near impossible https://goodaids.club/i/3c1ay529tm.png
		// so lets take a naive approach and work backwards from redline, 
		// finding the first place that the current gear would produce more torque, 
		// which is the optimal shift point
		for (int32_t rpm = redline; rpm > 0; --rpm)
		{
			// get the current torque, and the torque at the RPM of our next gear
			const auto currTorqueBase = torqueCurve.GetValue(rpm);
			nextGearRPM = rpm * (nextRatio / currRatio);
			const auto nextTorqueBase = torqueCurve.GetValue(static_cast<Curve::Data_t>(nextGearRPM));

			// by finding where current torque exceeds next torque (because we are working backwards),
			// we know where to shift at the highest RPM to get the best results.
			const auto currTorque = currTorqueBase * currRatio;
			const auto nextTorque = nextTorqueBase * nextRatio;

			if (currTorque > nextTorque)
			{
				// rpm + 1 because that is where acceleration would be greater in a higher gear
				std::cout << "Shift before " << rpm + 1 << " rpm from gear " << i + 1 << " to gear " << i + 2 << '\n';
				break;
			}
		}
	}

	return 0;
}