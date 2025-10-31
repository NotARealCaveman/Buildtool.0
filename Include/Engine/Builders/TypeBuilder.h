#pragma once
#include <concepts>

#include <ManifestMath/Point3.h>
#include <ManifestUtility/DebugLogger.h>
#include <ManifestUtility/SymbolHash.h>

using namespace Manifest_Utility;

namespace ReignEngine
{	
    MFbool BuildBoolean(const MFstring value);
	MFfloat BuildFloat(const MFstring value);
	MFu32 BuildU32(const MFstring value);
	MFint32 BuildInt32(const MFstring value);
	MFu64 BuildU64(const MFstring value);
	SymbolHash BuildSymbolHash(const MFstring value);
	MFstring  BuildString(const MFstring value);
    Manifest_Math::MFvec3 BuildVec3(const MFstring value);
	Manifest_Math::MFpoint3 BuildPoint3(const MFstring value);   

    const MFstring to_string(const Manifest_Math::MFvec3& vec3);

    template<std::integral T>
    T Convert(const std::string& s) 
    {
        return static_cast<T>(std::stoll(s));
    }

    template<std::floating_point T>
    T Convert(const std::string& s) 
    {
        return static_cast<T>(std::stod(s));
    }

    template<typename T>
        requires (std::integral<T> || std::floating_point<T>)
    void ParseCSV(const std::string& csv, std::vector<T>& values)
    {
        std::stringstream ss(csv);
        std::string value;

        //don't attempt to push empty values
        while (std::getline(ss, value, ',') && !value.empty())            
            values.push_back(Convert<T>(value));       
    }
}