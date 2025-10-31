#pragma once
#include <functional>
#include <unordered_map>

#include <ManifestUtility/TypeAssist.h>
#include <Engine/Builders/DatabaseBuilder.h>
#include <GameLibrary/Core/GameObject.h>

#include "ModuleEventSpace.h"

using namespace Manifest_Utility;
 
using Reign::ObjectInstance ;

namespace ReignEngine
{ 
	template<DBHeader Header>
	void LoadHeader(Header& header, std::ifstream& importDatabase)
	{
		assert(importDatabase.is_open());
		importDatabase.seekg(std::ios::beg);
		importDatabase.read(reinterpret_cast<char*>(&header), sizeof(header));
	}
	MFbool FindDatabaseEntry(const SymbolHash databaseID, const std::streamoff headerSize, std::ifstream& database); 

	template<std::default_initializable T>
	struct DatabaseImporter
	{
		template<DBHeader Header>
		void ImportDatabase(Header& header, DatabaseBuilder<T>& databaseBuilder, std::ifstream& importDatabase, std::unordered_map<EntryID,MFstring>& symbols) const
		{
			//import header
			LoadHeader(header, importDatabase);
			//import and construct each object
			const std::streampos tableOfContents{ importDatabase.tellg() };
			EntryKey entryKey;
			for (MFsize entryIndex{ 0 }; entryIndex < header.entryCount; ++entryIndex)
			{
				const std::streamoff keyOffet{ static_cast<std::streamoff>(entryIndex * sizeof(entryKey)) };
				importDatabase.seekg(tableOfContents + keyOffet);
				ImportObject(entryKey, importDatabase); 
				const std::streamoff entryPosition{ static_cast<std::streamoff>(entryKey.entryPosition) };
				importDatabase.seekg(entryPosition);
				T databaseObject{ Construct(entryKey.entryID,importDatabase) };
				std::string entryIdentifier{entryKey.entryName.view()};
				databaseBuilder.StoreObject(entryIdentifier, entryKey.entryID, std::move(databaseObject),symbols);
			}
		}
		T LoadEntry(const SymbolHash importID, const std::string databaseName, const MFsize headerOffset = sizeof(DatabaseHeader)) const
		{
			const std::string databasePath{ "Databases\\" + databaseName };
			std::ifstream importDatabase{ databasePath, std::ios::binary };			
			assert(importDatabase.is_open() && "Failed to open database");
			//queries table of contents and moves import database to object construction position
			if(FindDatabaseEntry(importID, headerOffset, importDatabase))
			   	return Construct(importID, importDatabase);
			
			return Stub();
		}
		virtual T Stub() const { return T{}; };
		virtual T Construct(const SymbolHash importID, std::ifstream& database) const = 0;
	};
	void ImportTableOfContents();


	class EngineModule
	{
	public:
		EngineModule(const Manifest_Communication::ObservationToken observationToken) : moduleObserver{ observationToken } {};
		//instantiationIndex is used to read what count/IDs of the game object currently being instantiated in the specific module
		virtual void InstantiateObject(ObjectInstance& objectInstance) = 0;
		virtual void TerminateObject(const ObjectInstance& objectInstance) = 0;		
	protected:					
		//loads data from file into cache and returns cached data
		template<typename T>
		const T& LoadEntry(const DatabaseImporter<T>& importer, const SymbolHash loadID, const std::string& databaseName, const MFsize databaseHeaderOffset = sizeof(DatabaseHeader))
		{			
			static std::unordered_map<MFu64, T> cachedLoads;

			if (!cachedLoads.contains(loadID))
				cachedLoads.insert(std::make_pair(loadID, importer.LoadEntry(loadID, databaseName, databaseHeaderOffset)));

			return cachedLoads.at(loadID);
		};		 

		ModuleEventSpace* moduleEventSpace{ nullptr };		
		Manifest_Communication::Observer moduleObserver;	
	};
}