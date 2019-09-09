/*
 *	Car Data Dumper
 *	9/8/19 20:50
 */

#include <Framework/Files/FileDecrypter.h>

#include <filesystem>
#include <iostream>

using Framework::ErrorCode;
using Framework::Files::File;
using Framework::Files::FileDecrypter;

std::string GetWorkingDirectory()
{
	// get the current folder name, if not specified by the 
	std::string folderName;

	folderName = std::filesystem::current_path().string();

	// make sure no slashes are in the folder name
	auto lastSlash = folderName.find_last_of("/\\", 0, 2);

	if (lastSlash != std::string::npos)
	{
		folderName = folderName.substr(lastSlash + 1);
	}

	return folderName;
}

int main(int argc, char* argv[])
{
	if (argc > 5)
	{
		std::cout << "Usage: " << argv[0] << " [dataFile:string:data.acd] [path:string:wd] [outPath:string:dataFileMinusExt] [fileName:string[OPT]]\n";
		return 1;
	}

	std::string dataFile = (argc >= 2) ? argv[1] : "data.acd";
	std::string directory = (argc >= 3) ? argv[2] : GetWorkingDirectory();
	
	// if they specified an output folder path, use that instead of dataFileMinusExt
	std::string outPath;
	if (argc >= 4)
	{
		outPath = argv[3];
	}
	else
	{
		auto ext = dataFile.find_first_of('.');
		if (ext != std::string::npos)
			outPath = dataFile.substr(0, ext);
		else
			outPath = dataFile;
	}
	// append a slash for use later when appending file name for output
	outPath += '/';

	ErrorCode ec;
	FileDecrypter decrypter(dataFile, directory, ec);

	if (ec != Framework::ErrorCode_SUCCESS)
	{
		std::cout << "Error: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
		return 1;
	}

	// create the output directory if it does not exist
	if (std::filesystem::is_directory(outPath) == false)
	{
		if (std::filesystem::create_directory(outPath) == false)
		{
			std::cout << "Failed to create data directory\n";
			return 1;
		}
	}

	std::ofstream outFile;

	if (argc < 4)
	{
		// user did not specify a specific file to decrypt, decrypting and saving all to path
		auto files = decrypter.DecryptAll(ec);

		if (ec != Framework::ErrorCode_SUCCESS)
		{
			std::cout << "Error decrypting files: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
			return 1;
		}

		for (const auto& file : files)
		{
			outFile.open(outPath + file.GetName());

			if (outFile.good() == false)
			{
				std::cout << "Failed to open output file\n";
				return 1;
			}

			outFile << file.GetContents();
			outFile.close();
		}
	}
	else
	{
		// user specified a specific file to decrypt

		std::string fileName = argv[4];
		
		const auto file = decrypter.DecryptFile(fileName, ec);

		// can we find the file?
		if (ec != Framework::ErrorCode_SUCCESS)
		{
			std::cout << "Error decrypting file: " << ec.GetMessage() << " (" << ec.GetRawCode() << ")\n";
			return 1;
		}

		outFile.open(outPath + fileName);

		if (outFile.good() == false)
		{
			std::cout << "Failed to open output file\n";
			return 1;
		}

		outFile << file.GetContents();
		outFile.close();
	}

	return 0;
}