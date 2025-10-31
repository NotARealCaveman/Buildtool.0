#pragma once
#include <functional>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <ranges>

#include <ManifestMemory/Containers/FlatMap.h>
#include <ManifestUtility/SymbolHash.h> 

using Manifest_Memory::FlatMap;

namespace ReignEngine
{
	using FormFunction = std::function<void(const MFint32)>;
	using FormKeyMap = std::unordered_map<MFint32, FormFunction>;
	using FormMenuMap = std::unordered_map<MFu32, FormFunction>;

	struct TextBuffer
	{
		void Insert(const std::string text);
		void Insert(const MFint32 keyCode);
		void Backspace();
		void Delete();

		MFstring buffer{};
		MFsize bufferPosition{ 0 };
	};

	struct FormLine
	{
		void ConfigureLine(const MFstring& lineName, const MFsize lineIndex, const MFstring  defaultBuffer = "", FormFunction lineFunction = [](const MFint32)->void {}, const MFbool isEditable = true);
		MFstring BuildFormLine(const MFbool isCursorVisible) const;
		MFstring value() const { return textBuffer.buffer; };
	
		FormFunction lineFunction{ [](auto)->void {} };	
		MFstring lineName;
		TextBuffer textBuffer;
		MFbool isEditable{ true };
		MFbool isActive{ false };
	};

	using EntryID = SymbolHash;
	using OptionList = std::map<EntryID, MFstring>;
	std::string BuildListString(const OptionList& optionList);
	enum class ReturnValue
	{
		DEFAULT = 0,
		CSV,
		BITFIELD,
	};
	MFstring BuildReturnString(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines, const ReturnValue returnValue);
	MFstring BuildBitfield(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines);
	MFstring BuildCSV(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines);
	std::vector<MFstring> SplitCSV(MFstring csv, const MFsize stride = 1);
}