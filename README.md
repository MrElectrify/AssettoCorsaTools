# AssettoCorsaTools
AssettoCorsaTools is a set of tools for use in modification of AssettoCorsa as well as an example providing a practical non-modding use-case of the library.
# AssettoCorsaCarDataDumper
Usage: `AssettoCorsaCarDataDumper [dataFile:string:data.acd] [directory:string:wd] [outDirectory:string:dataFileMinusExt] [fileName:string[OPT]]`

Purpose: AssettoCorsaCarDataDumper demonstrates the use of `FileDecrypter` by decrypting and outputting the virtual filesystem contained in the `.acd` files. These contain all aspects of a car's performance, from aerodynamics to suspension, to engine torque/power, the presence of turbochargers, electronics, and more.

# AssettoCorsaShiftOptimizer
Usage: `AssettoCorsaShiftOptimizer [dataFile:string:data.acd] [directory:string:wd]`

Purpose: AssettoCorsaShiftOptimizer calculates the optimal shift points for a car for the highest acceleration, as torque falls off at higher engine speeds and some gears are close enough for torque to actually be higher in the next gear.

# AssettoCorsaToolFramework
Purpose: AssettoCorsaToolFramework is a library that contains APIs to manipulate the encrypted virtual file system.

## Framework::Curve
#### Location:
`Framework/Curve.h`
#### Purpose:
The purpose of Curve is to allow for a simple wrapper for a container which interpolates based on known values.
#### DataTypes: 
`Data_t` = `int32_t`

`Map_t` = `std::map<Data_t, Data_t>`
#### Member functions:
`void ParseLUT(std::istream& lutFile)` - Parses the LUT file via a stream. throws ErrorCode on error

`void ParseLUT(std::istream& lutFile, Framework::ErrorCode& ec)` - Parses the LUT file via a stream. Returns ErrorCode in ec on error

`Data_t GetMinRef() const` - Returns the smallest reference value

`Data_t GetMaxRef() const` - Returns the largest reference value

`Data_t GetValue(const Data_t ref) const` - Returns the interpolated value at a point. Returns 0 if the ref is out of range

`const Map_t& GetValues() const noexcept` - Returns the map of values
## Framework::ErrorCode
#### Location:
`Framework/Error.h`
#### Purpose:
The purpose of ErrorCode is to wrap certain errors in a way that allows the user to get raw error codes (and compare as ab enumeration, _ErrorCode) as well as formatted string errors.
#### DataTypes:
`RawCode_t` = `_ErrorCode`
#### Member Functions:
`ErrorCode()` - Constructs an empty ErrorCode, default intialized to success.

`ErrorCode(RawCode_t code)` - Constructs with a raw _ErrorCode

`RawCode_t GetRawCode() const` - Returns the raw ErrorCode

`std::string GetMessage() const` - Returns the error message

`operator RawCode_t() const` - Implicit conversion to the enumeration for comparison
## Framework::Files::File
#### Location:
`Framework/Files/File.h`
#### Purpose:
The purpose of File is to wrap a file's name and contents together, immutable
#### DataTypes:
`Data_t` = `std::string`
#### Member Functions:
`File(const Data_t& name, const Data_t& contents)` - Constructs a file with the specified name and contents

`File(File&& other)` - Move constructor for efficiently moving large files

`Data_t GetName() const noexcept` - Returns the name of the file

`Data_t GetContents() const noexcept` - Returns the contents of the file
## Framwork::Files::FileManager
#### Enum MODE:
`MODE_READ` - The manager is in read mode and reads from a file to populate the internal file buffer, and does not support outputting
`MODE_WRITE` - The manager is in write mode, and does not support reading, and can output to a buffer
`MODE_READWRITE` - The manager is able to both read and write files
#### Location:
`Framework/Files/FileManager.h`
#### Purpose:
The purpose of FileManager is to decrypt and decrypt `.acd` files found in Assetto Corsa.
#### DataTypes:
`File_t` = `std::ifstream`
`Key_t` = `std::string`
`Mode_t` = `MODE`
#### Member Functions:
`FileManager(const std::string& fileName, const std::string& directory, Mode_t mode)` - Default constructor, throws ErrorCode on error. Assumes directory only includes the name of the directory, and no other part of the path

`FileManager(const std::string& fileName, const std::string& directory, Mode_t mode Framework::ErorrCode& ec)` - Overload that does not throw, stores ErrorCode in ec on error. Assumes directory only includes the name of the directory, and no other part of the path

`File GetFile(const std::string& fileName)` - Gets a single file by name. Throws ErrorCode on error

`File GetFile(const std::string& fileName, ErrorCode& ec)` - Gets a single file by name. Stores ErrorCode in ec on error

`Vec_t GetFiles()` - Gets all files`