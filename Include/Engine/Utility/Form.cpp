#include "Form.h"
using namespace ReignEngine;

Console::Console(MFint32 consoleWidth, MFint32 consoleHeight)
	:width{consoleWidth}, height{consoleHeight}
{
	if (!AllocConsole()) {
		MessageBoxA(nullptr, "Failed to create console!", "Error", MB_OK | MB_ICONERROR);
		return;
	}
	// Redirect standard output/input streams to the new console
	FILE* pCout;
	freopen_s(&pCout, "CONOUT$", "w", stdout);
	FILE* pCin;
	freopen_s(&pCin, "CONIN$", "r", stdin);

	handle = GetStdHandle(STD_OUTPUT_HANDLE);
	window = GetConsoleWindow();	
	// 1. Set buffer size
	COORD bufferSize = { (SHORT)width, (SHORT)height };
	SetConsoleScreenBufferSize(handle, bufferSize);

	// 2. Set window size
	SMALL_RECT windowSize = { 0, 0, (SHORT)(width - 1), (SHORT)(height - 1) };
	SetConsoleWindowInfo(handle, TRUE, &windowSize);

	// 3. Move window on screen
	RECT rect;
	GetWindowRect(window, &rect);
	MoveWindow(window, NULL, NULL, rect.right - rect.left, rect.bottom - rect.top, TRUE);

	// 4. Remove resize + maximize buttons
	LONG style = GetWindowLong(window, GWL_STYLE);
	style &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX);
	SetWindowLong(window, GWL_STYLE, style);

	Enable_ASCII_Color();
}

void Console::WriteToBuffer(const std::string& displayString)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hConsole == INVALID_HANDLE_VALUE) return;
	
	COORD coord{ (SHORT)0, (SHORT)currentLineCount };	
	const MFsize lineCount{ (displayString.size() / width)+1 };
	const MFsize clearBufferSize{ width - (displayString.size() % width) };
	const char clear{ ' ' };	
	const MFstring writeString{ displayString + MFstring(clearBufferSize,clear) };
	currentLineCount += lineCount;
	SetConsoleCursorPosition(hConsole, coord);  // move cursor
	WriteConsoleA(hConsole, writeString.c_str(), writeString.size(), nullptr, nullptr);
}

void Form::DisplayForm(Console& console)
{
	console.currentLineCount = 0;
	MFstring formName{"\t" + headerLine.BuildFormLine(false) };
	console.WriteToBuffer(formName);
	for (const FormLine& bodyLine : bodyLines)
	{
		MFstring lineString{ bodyLine.BuildFormLine(console.isCursorVisible) };
		console.WriteToBuffer(lineString);
	}

	if (console.currentLineCount < console.previousLineCount)
	{
		const MFsize count{ (console.previousLineCount - console.currentLineCount) * console.width };
		MFsize lineCount = console.currentLineCount;
		const char clear{ ' ' };
		const MFstring clearString(count, clear);
		console.WriteToBuffer(clearString);		
		console.currentLineCount = lineCount;//dont carry updates to line count 
	}
	console.previousLineCount = console.currentLineCount;
}

void ReignEngine::CreateEntryLine(const MFsize lineIndex, const MFstring lineName, const std::string defaultValue, std::unique_ptr<Form>& currentForm, const MFbool isEditable, const FormFunction lineFunction)
{	
	currentForm->bodyLines.insert(std::make_pair(lineIndex, FormLine{}));
	currentForm->bodyLines.at(lineIndex).ConfigureLine(lineName, lineIndex, defaultValue, lineFunction, isEditable);
}