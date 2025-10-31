#include "TypeBuilder.h"

using namespace ReignEngine;
using namespace Manifest_Math;
 
MFbool ReignEngine::BuildBoolean(const MFstring value)
{
	return static_cast<MFbool>(std::stoi(value));
}


MFfloat ReignEngine::BuildFloat(const MFstring value)
{  
	return std::stof(value);
}

MFu32 ReignEngine::BuildU32(const MFstring value)
{
	return std::stoul(value);
}

MFint32 ReignEngine::BuildInt32(const MFstring value)
{ 
	return std::stoi(value);
}

MFu64 ReignEngine::BuildU64(const MFstring value)
{  
	return std::stoull(value);
}

SymbolHash ReignEngine::BuildSymbolHash(const MFstring value)
{
	if (value.empty())
		return SymbolHash{ 0ull };

	return SymbolHash{ value.c_str() };
}

MFstring  ReignEngine::BuildString(const MFstring value)
{
	return value;
}

Manifest_Math::MFvec3 ReignEngine::BuildVec3(const MFstring value)
{
	MFpoint3 result;

	std::vector<MFfloat> axisValues;
	ParseCSV(value, axisValues);
	assert(axisValues.size() == 3 && "INCORRECT NUMBER OF AXIS VALUES FOR VEC3 TYPE");
	result.x = axisValues.at(0);
	result.y = axisValues.at(1);
	result.z = axisValues.at(2);

	return result;
}

Manifest_Math::MFpoint3 ReignEngine::BuildPoint3(const MFstring value)
{
	return BuildVec3(value);
}

const MFstring ReignEngine::to_string(const Manifest_Math::MFvec3& vec3)
{
	return std::to_string(vec3.x) + "," + std::to_string(vec3.y) + "," + std::to_string(vec3.z);
}