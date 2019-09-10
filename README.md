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

`void ParseLUT(std::istream& lutFile, Framework::ErrorCode& ec)` - Parses the LUT file via a stream. returns ErrorCode in ec on error

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
## Framwork::Files::FileDecrypter
#### Location:
`Framework/Files/FileDecrypter.h`
#### Purpose:
The purpose of FileDecrypter is to decrypt the encrypted `.acd` files found in Assetto Corsa.
#### DataTypes:
`File_t` = `std::ifstream`
`Key_t` = `std::string`
#### Member Functions:
`FileDecrypter(const std::string& fileName, const std::string& directory)` - Constructs a decrypter on a certain file, which cannot be changed after initialization. Throws ErrorCode on error

`FileDecrypter(const std::string& fileName, const std::string& directory, Framework::ErorrCode& ec)` - Constructs a decrypter on a certain file, which cannot be changed after initialization. Returns ErrorCode in ec on error

`File DecryptNext()` - Decrypts the next file in the filesystem and returns it. Throws ErrorCode on error

`File DecryptNext()`- Decrypts the next file in the filesystem and returns it. Returns ErrorCode in ec on error

`std::vector<File> DecryptAll()` - Decrypts the entire filesystem and returns it as a vector of Files. Throws ErrorCode on error

`std::vector<File> DecryptAll(Framework::ErrorCode& ec)` - Decrypts the entire filesystem and returns it as a vector of Files. Returns ErrorCode in ec on error

`File DecryptFile(const std::string& fileName)` - Decrypts a file by name and returns it. Throws ErrorCode on error

`File DecryptFile(const std::string& fileName, Framework::ErrorCode& ec)` - Decrypts a file by name and returns it. Returns ErrorCode in ec on error

`~FileDecrypter()` - Cleans up and closes handles
