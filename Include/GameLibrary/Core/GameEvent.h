#pragma once
#include <unordered_set>
#include <unordered_map>

#include <ManifestCommunication/Observer.h>
#include <ManifestUtility/Typenames.h>


using namespace Manifest_Utility;

namespace Reign
{
	enum class EventType : MFu64
	{ 
	};	
	 
		
	using GameEvent = Manifest_Communication::Event; 
}
