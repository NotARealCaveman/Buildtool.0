#include "ImportExport.h"

using namespace ReignEngine;

MFsize ReignEngine::ComputeStringSize(const std::string& string)
{
	return COUNT_OFFSET + string.size();
}

void ReignEngine::ExportString(const std::string& string, std::ofstream& exportDatabase)
{
	DLOG({ CONSOLE_BG_YELLOW }, "ADDRESS:", &exportDatabase);
	//write string size
	const MFsize size{ static_cast<MFu32>(string.size()) };
	assert(exportDatabase.is_open());
	DLOG({ CONSOLE_BG_GREEN }, "POSITION BEFORE:", exportDatabase.tellp());
	exportDatabase.write(reinterpret_cast<const char*>(&size), COUNT_OFFSET);
	DLOG({ CONSOLE_BG_GREEN }, "POSITION AFTER:", exportDatabase.tellp());
	//write string contents
	exportDatabase.write(string.data(), size);
}

void ReignEngine::ImportString(std::string& string, std::ifstream& importDatabase)
{
	MFsize stringLength{ 0 };
	importDatabase.read(reinterpret_cast<char*>(&stringLength), COUNT_OFFSET);
	DLOG({ CONSOLE_BG_BLUE }, "IMPORTING", stringLength, "CHARACTER STRING");
	string.resize(stringLength);
	importDatabase.read(string.data(), stringLength);
}