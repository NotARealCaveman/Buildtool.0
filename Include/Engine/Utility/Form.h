#pragma once 
#define NOMINMAX
#include <Windows.h>
#include <map>
#include <algorithm>
#include <stack>
#include <functional>

#include <ManifestUtility/TypeNames.h>
#include <ManifestUtility/Console_Color.h>

#include "FormLine.h"

using namespace Manifest_Utility;

namespace ReignEngine
{
	struct Console
	{
		Console(const MFint32 consoleWidth, const MFint32 consoleHeight);
		void WriteToBuffer(const std::string& displayString);
				
		const MFint32 width{ 0 };
		const MFint32 height{ 0 };
		MFbool isCursorVisible{ true };
		HANDLE handle;
		HWND window;
		CONSOLE_SCREEN_BUFFER_INFO bufferInfo;
		MFu32 currentLineCount{ 0 };
		MFu32 previousLineCount{ 0 };
	};

	struct Form
	{
		void DisplayForm(Console& console);		
		FormLine headerLine;
		FlatMap<FormLine> bodyLines;
		//TODO: footer line for direction
		MFint8 activeLine{ 0 };
		FormFunction exitFunction{ [](MFint32)->void {} };
	};	
	using FormGenerationFunction = std::function<std::unique_ptr<Form>(const std::string entryName)>;

	inline MFstring BuildHeaderString(const MFstring entryName)
	{
		return entryName + '(' + std::to_string(SymbolHash{ entryName.c_str() }) + ')';
	}

	struct BuildLineDetails
	{
		MFsize index;
		MFstring name;
	};

	void CreateEntryLine(const MFsize lineIndex, const MFstring lineName, const std::string defaultValue, std::unique_ptr<Form>& currentForm,  const MFbool isEditable = true, const FormFunction lineFunction = [](MFint32)->void {});
} 