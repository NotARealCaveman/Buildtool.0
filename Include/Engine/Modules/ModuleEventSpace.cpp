#include "ModuleEventSpace.h"

using namespace ReignEngine;
using namespace Manifest_Communication;
using namespace Reign;

ModuleEventSpace::ModuleEventSpace()
{  
}

void ModuleEventSpace::NotifyObservers(const GameEvent& gameEvent)
{	

	for (const Message& message : gameEvent.messages)
	{
		const EventType eventType{ message.messageToken };
		assert(moduleObservers.contains(eventType) && "INVALID MESSAGE TOKEN SUPPLIED");
		const EventDispatcher dispatcher{ eventDisptachers.at(eventType) };
		std::vector<Observer*>& observers{ moduleObservers.at(eventType) }; 
		ForwardFunction(dispatcher, message, observers);		  
	}
}

MFbool ModuleEventSpace::RegisterModuleObserver(Observer& observer)
{
	MFu64 observedEventTypes{ observer.observationToken };
	MFu64 eventMask{ 1 };
	while (observedEventTypes)
	{
		if (eventMask & observedEventTypes)
			moduleObservers[EventType{ eventMask }].emplace_back(&observer);
		observedEventTypes &= ~eventMask;
		eventMask <<= 1;
	}

	return true;
}