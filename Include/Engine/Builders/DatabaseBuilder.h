#pragma once
#include <cassert>
#include <fstream>
#include <set>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <ranges>
#include <array>

#include <ManifestMemory/Containers/FlatMap.h>
#include <ManifestUtility/DebugLogger.h>
#include <ManifestUtility/SymbolHash.h>
#include <ManifestUtility/FixedString.h>

#include <Engine/ImportExport.h> 
#include <Engine/Utility/Form.h>
#include "TypeBuilder.h"

using namespace Manifest_Utility;

namespace ReignEngine
{
	//can increase if needed - check naming conventions
	using EntryName = FixedString<23>;

	//to be moved into base builder file
	struct EntryKey
	{
		inline MFbool operator< (const EntryKey& other) const { return entryID < other.entryID; };
		inline MFbool operator== (const EntryKey& other) const { return entryID == other.entryID; };

		EntryID entryID;
		MFsize entryPosition;
		EntryName entryName;
	};

	struct DatabaseHeader
	{
		MFu32 entryCount;
	};

	template<typename T>
	concept DBHeader = std::is_base_of_v<DatabaseHeader, T>&& std::is_trivially_copyable_v<T>;

	template<std::default_initializable T>
	class DatabaseExporter
	{
	public:
		virtual void ExportBinaryData(const T& entry, std::ofstream& questDatabase) const = 0;
	};
	std::streamoff ExportTableOfContents(const MFsize exportIndex, const std::streampos& tableOfContents, EntryKey entryKey, std::streamoff& currentWritePosition, std::ofstream& exportDatabase);
		
	template<std::default_initializable T>
	class DatabaseBuilder
	{
	public:
		virtual std::unique_ptr<Form> CreateEntryForm(const std::string& entryName, const T& entryData, std::stack<std::unique_ptr<Form>>& formStack, std::unordered_map<EntryID, MFstring>& symbols) = 0;
		virtual void CreateEntry(const Form& entryForm, const std::string& entryName, const EntryID entryID, std::unordered_map<EntryID, MFstring>& symbols) = 0;
		virtual MFsize EntrySize(const T& entry) = 0;
		//returns current entry if it exists - stub if not 
		const T& GetEntry(const std::string& entryName, std::unordered_map<EntryID, MFstring>& symbols)
		{
			const EntryID entryID{ entryName.c_str() };
			if (!entries.contains(entryID))
				StoreObject(entryName, entryID, T{}, symbols);

			return entries.at(entryID);
		}
		//handles initial insertion + object modification
		void StoreObject(const std::string& entryName, const EntryID entryID, T&& entry, std::unordered_map<EntryID, MFstring>& symbols)
		{
			EntryKey entryKey
			{
				.entryID { entryID},
				.entryPosition { EntrySize(entry)},
				.entryName { entryName }
			};
			if (keys.contains(entryKey))
			{
				entries.erase(entryID);
				keys.erase(entryKey);
			}			
			keys.insert(entryKey);
			entries.insert(std::make_pair(entryID, std::forward<T>(entry)));
			symbols.insert(std::make_pair(entryID, entryName));
			currentEntrySize += entryKey.entryPosition;
		}
		template<DBHeader Header>
		void ExportDatabase(const DatabaseExporter<T>& databaseExporter, Header header, std::ofstream& exportDatabase) const
		{
			assert(entries.size() == keys.size());
			header.entryCount = entries.size();
			//compute file size
			const MFsize headerSize{ sizeof(header) };
			const MFsize tocSize{ header.entryCount * sizeof(EntryKey) };
			const MFsize totalSize{ headerSize + tocSize + currentEntrySize };

			assert(exportDatabase.is_open());
			//allocate blank binary
			exportDatabase.seekp(totalSize - 1);
			exportDatabase.put('\0');//commit write	
			exportDatabase.seekp(std::ios::beg);

			//export header
			ExportObject(header, exportDatabase);
			const std::streampos tableOfContents{ exportDatabase.tellp() };
			MFu32 index{ 0 };
			std::streamoff currentWritePosition{ static_cast<std::streamoff>(headerSize + tocSize) };
			//export quest and table of contents			
			for (EntryKey entryKey : keys)
			{
				const std::streamoff writePosition{ ExportTableOfContents(index, tableOfContents, entryKey,currentWritePosition,  exportDatabase) };
				exportDatabase.seekp(writePosition);
				const T& databaseEntry{ entries.at(entryKey.entryID) };
				databaseExporter.ExportBinaryData(databaseEntry, exportDatabase);
				++index;
			}
		}

		FormFunction CreateEntryOnClose(const std::unique_ptr<Form>& entryForm, const std::stack<std::unique_ptr<Form>>& formStack, const MFstring& entryName, std::unordered_map<EntryID, MFstring>& symbols)
		{
			Form* const form{ entryForm.get() };

			return FormFunction
			{
				[this, &formStack, &symbols, form, entryName](MFint32)->void
				{//presumably the entry form is the top form 	
					const EntryID entryID{ SymbolHash{entryName.c_str()} };
					CreateEntry(*form, entryName, entryID, symbols);
				}
			};
		}

		std::unordered_map<EntryID, T> entries;
		std::set<EntryKey> keys;
		MFsize currentEntrySize{ 0 };		
	};
}