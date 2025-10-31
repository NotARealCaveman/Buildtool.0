#include "FormLine.h"
using namespace ReignEngine;

void TextBuffer::Insert(const std::string text)
{
	if (text.empty())
		return;

	buffer += text;
	bufferPosition += text.size();
}

void TextBuffer::Insert(const MFint32 keyCode)
{
	buffer.insert(bufferPosition, 1, static_cast<char>(keyCode));
	++bufferPosition;
}

void TextBuffer::Backspace()
{
	if (bufferPosition > 0 && buffer.size())
		buffer.erase(--bufferPosition, 1);
}

void TextBuffer::Delete()
{
	if (bufferPosition > 0 && buffer.size())
		buffer.erase(bufferPosition, 1);	
}

void FormLine::ConfigureLine(const MFstring& lineName, const MFsize lineIndex, const MFstring  defaultBuffer, FormFunction lineFunction, const MFbool isEditable )
{
	this->lineName = lineName;
	textBuffer.Insert(defaultBuffer);
	this->lineFunction = lineFunction;
	this->isEditable = isEditable;	
	if (lineIndex == 0)
		this->isActive = true;
}

MFstring FormLine::BuildFormLine(const MFbool isCursorVisible) const
{
	MFstring result;
	const MFstring activeLine{ "[x]" };
	const MFstring inactiveLine{ "[ ]" };
	result += isActive ? activeLine : inactiveLine;
	result += "    ";
	result += lineName;
	result += "    " ;
	MFstring buffer{ textBuffer.buffer };
	if (isCursorVisible && isActive && isEditable)
	{
		if (buffer.empty())	
			buffer = '_';
		else if(textBuffer.bufferPosition == buffer.size())	
			buffer += '_';
		else 
			buffer.at(textBuffer.bufferPosition) = '_';
	}
	result += buffer;

	return result;
}

std::string ReignEngine::BuildListString(const OptionList& optionList)
{
	std::string result;

	for (const auto& listOption : optionList)
	{
		const auto& [optionID, optionName] {listOption};
		result += std::to_string(optionID) + " " + optionName + " ";
	}

	return result;
}

MFstring ReignEngine::BuildReturnString(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines, const ReturnValue returnValue)
{
	assert(returnValue != ReturnValue::DEFAULT);

	return returnValue == ReturnValue::CSV ? BuildCSV(buildLines, bodyLines) : BuildBitfield(buildLines, bodyLines);
}

MFstring ReignEngine::BuildCSV(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines)
{
	MFstring result;
	std::ranges::for_each(buildLines, [&bodyLines, &result](const MFu32 lineIndex)->void
	{
		const FormLine& selectedLine{ bodyLines.at(lineIndex) };
		const TextBuffer& buffer{ selectedLine.textBuffer };
		const MFstring value{ buffer.buffer };
		result += value;
		result += ",";
	});

	return	result;
}

std::vector<MFstring> ReignEngine::SplitCSV(MFstring csv, const MFsize stride)
{
	std::vector<std::string> result;
	while (csv.size())
	{
		MFsize offset{ 0 };
		for (int i = 0; i < stride; ++i)
			offset = csv.find_first_of(',', offset) + 1;
		assert(offset != std::string::npos);//TODO: default fill
		result.emplace_back(csv.substr(0, offset - 1));
		csv.erase(0, offset);
	}

	return result;
}

MFstring ReignEngine::BuildBitfield(const std::vector<MFu32>& buildLines, const FlatMap<FormLine>& bodyLines)
{
	MFu64 mask{ 0 };
	std::ranges::for_each(buildLines, [&bodyLines, &mask](const MFu32 lineIndex)->void
	{
		const FormLine& selectedLine{ bodyLines.at(lineIndex) };
		const TextBuffer& buffer{ selectedLine.textBuffer };
		const MFstring value{ buffer.buffer };
		mask |= std::stoull(value);
	});

	return std::to_string(mask);
}
