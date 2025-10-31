#pragma once
#include <functional>
#include <vector>	
#include <unordered_map>

#include <ManifestCommunication/Observer.h>
#include <ManifestUtility/Typenames.h>
#include <GameLibrary/Core/GameEvent.h>

using namespace Manifest_Utility;

namespace ReignEngine
{
	//Inspired by observer pattern over a non concurrent EventSpace that allows for multiple observations on the same event type
	//observers are repurposed and no longer give *exclusive* access
	struct ModuleEventSpace
	{
		ModuleEventSpace();

		void NotifyObservers(const Reign::GameEvent& gameEvent);
		MFbool RegisterModuleObserver(Manifest_Communication::Observer& observer);
		template<typename Content>			
		void PushObservations(const Content& content,const Manifest_Communication::ObservationToken messageToken,  std::vector<Manifest_Communication::Observer*> observers)
		{
			for (Manifest_Communication::Observer* observer : observers)
				observer->ObserveEvent(Manifest_Communication::Message{ messageToken,content });
		}

		using Observer = Manifest_Communication::Observer;
		std::unordered_map<Reign::EventType, std::vector<Observer*>> moduleObservers;
		using EventDispatcher = std::function<void(const Manifest_Communication::Message&, std::vector<Observer*>& observers)>;
		std::unordered_map<Reign::EventType, EventDispatcher> eventDisptachers;
	};
}