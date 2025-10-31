#pragma once
#include <fstream>

#include <ManifestUtility/DebugLogger.h>
#include <ManifestUtility/Typenames.h>

using namespace Manifest_Utility;

namespace ReignEngine
{
	template<typename T>
	MFsize ComputeObjectSize(const T t) { return sizeof(t); };
	template<typename T>
	void ExportObject(const T t, std::ofstream& exportDatabase)
	{
		exportDatabase.write(reinterpret_cast<const char*>(&t), sizeof(t));
	}
	template<typename T>
	void ImportObject(T& t, std::ifstream& importDatabase)
	{
		importDatabase.read(reinterpret_cast<char*>(&t), sizeof(t));
	}

	//return storage requirement for element count + elements
	MFsize ComputeStringSize(const std::string& string);
	void ExportString(const std::string& string, std::ofstream& exportDatabase);
	void ImportString(std::string& string, std::ifstream& importDatabase);
	//return storage requirement for element count + elements
	static constexpr MFsize COUNT_OFFSET{ sizeof(MFsize) };
	template<typename T>
	MFsize ComputeVectorSize(const std::vector<T>& elements)
	{
		return COUNT_OFFSET + sizeof(T) * elements.size();
	}
	template<typename T>
	void ExportVector(const std::vector<T>& elements, std::ofstream& exportDatabase)
	{
		//write vector size
		const MFsize size{ elements.size() };
		exportDatabase.write(reinterpret_cast<const char*>(&size), COUNT_OFFSET);		
		//write vector contents
		exportDatabase.write(reinterpret_cast<const char*>(elements.data()), size * sizeof(T));
	}
	template<typename T>
	void ImportVector(std::vector<T>& elements, std::ifstream& importDatabase)
	{
		//get element count - database already at count
		MFsize elementCount;
		importDatabase.read(reinterpret_cast<char*>(&elementCount), COUNT_OFFSET);
		//if no elements just return				
		if (!elementCount)
			return;
		//create storage for each element
		elements.resize(elementCount);
		//copy objective IDs and counts out of database	
		importDatabase.read(reinterpret_cast<char*>(elements.data()), elementCount * sizeof(T));
	}
}