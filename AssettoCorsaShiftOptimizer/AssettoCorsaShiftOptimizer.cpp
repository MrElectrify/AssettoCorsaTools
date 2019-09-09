#include <Framework/Files/FileDecrypter.h>

#include "inipp.h"

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>

/*
 *	Shift Optimizer
	9/8/19 22:40
 */

using Framework::ErrorCode;
using Framework::Files::File;
using Framework::Files::FileDecrypter;

std::string GetWorkingDirectory()
{
	// get the current folder name, if not specified by the 
	std::string folderName;

	folderName = std::filesystem::current_path().string();

	// make sure no slashes are in the folder name
	auto lastSlash = folderName.find_last_of("/\\", -1, 2);

	if (lastSlash != std::string::npos)
	{
		folderName = folderName.substr(lastSlash + 1);
	}

	return folderName;
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

	// get power.lut
	File power = decrypter.DecryptFile("power.lut", ec);

	if (ec != Framework::ErrorCode_SUCCESS)
	{
		std::cout << "Unable to find power.lut in file: " << ec.GetMessage() << " (" << ec.GetRawCode() << "\n";
		return 1;
	}

	// get drivetrain.ini
	File drivetrain = decrypter.DecryptFile("drivetrain.ini", ec);

	if (ec != Framework::ErrorCode_SUCCESS)
	{
		std::cout << "Unable to find drivetrain.ini in file: " << ec.GetMessage() << " (" << ec.GetRawCode() << "\n";
		return 1;
	}

	inipp::Ini<char> ini;
	// put the string in a stream for the ini parser to work properly
	std::stringstream drivetrainContents(drivetrain.GetContents());

	ini.parse(drivetrainContents);

	// fetch the number of gears
	int gearCount = -1;

	auto gears = ini.sections["GEARS"];
	
	if (inipp::extract(gears["COUNT"], gearCount) == false || gearCount < 0)
	{
		std::cout << "Failed to parse number of gears\n";
		return 1;
	}

	// fetch our final drive ratio
	float finalDrive = -1.f;

	if (inipp::extract(gears["FINAL"], finalDrive) == false || finalDrive < 0.f)
	{
		std::cout << "Failed to parse final drive ratio\n";
		return 1;
	}

	// create our vector of ratios
	std::vector<float> gearRatios(gearCount);

	// create our prefix for all of the gear ratios
	std::string gearKey = "GEAR_";

	for (size_t i = 1; i <= gearCount; ++i)
	{
		if (inipp::extract(gears[gearKey + std::to_string(i)], gearRatios[i - 1]) == false)
		{
			std::cout << "Failed to extract gear ratio " << i << '\n';
			return 1;
		}

		std::cout << "Ratio for " << i << ": " << gearRatios[i - 1] << '\n';
	}

	std::cout << "Gear count: " << gearCount << '\n';
	std::cout << "Final drive ratio: " << finalDrive << '\n';

	return 0;
}