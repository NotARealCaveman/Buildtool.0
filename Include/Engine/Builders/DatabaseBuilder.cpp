#include "DatabaseBuilder.h"

using namespace ReignEngine;

std::streamoff ReignEngine::ExportTableOfContents(const MFsize exportIndex, const std::streampos& tableOfContents, EntryKey entryKey, std::streamoff& currentWritePosition, std::ofstream& exportDatabase)
{
	const std::streamoff writePosition{ currentWritePosition };
	currentWritePosition += entryKey.entryPosition;	
	entryKey.entryPosition = writePosition;
	const std::streampos keyPosition{ tableOfContents + static_cast<std::streamoff>(exportIndex * sizeof(EntryKey)) };	
	exportDatabase.seekp(keyPosition);
	ExportObject(entryKey, exportDatabase);

	return writePosition;
}

