#include "ExpansionForm.h"

using namespace ReignEngine;


void ExpansionForm::ConfigureForm(const MFstring& formName, TextBuffer& resultBuffer, const MFsize maxEntries)
{
	this->maxEntries = maxEntries;
	headerLine.lineName = formName;
	//TODO:place in footer/update x/x value
	headerLine.lineName += +" MAX ENTRIES:" + std::to_string(maxEntries);

	exitFunction = [this, &resultBuffer](MFint32)->void
	{
		//don't include the final line if it's empty 
		const MFsize lineCount{ bodyLines.values.back().value().empty() ? bodyLines.size() - 1 : bodyLines.size() };
		std::vector<MFu32> buildLines;
		std::ranges::generate_n(std::back_inserter(buildLines), lineCount, [lineIndex = 0]() mutable->MFu32 {return lineIndex++; });
		resultBuffer.buffer = BuildReturnString(buildLines, this->bodyLines, ReturnValue::CSV);
	};

	const MFsize lineIndex{ bodyLines.size() };
	MFstring lineName{ "Enter value:" };
	FormFunction newLineFunction = [this, lineName](MFint32)->void
	{
		FormLine& currentExpansionLine{ bodyLines.values.back() };
		if (!currentExpansionLine.isActive || currentExpansionLine.value().empty())
			return;
		currentExpansionLine.isActive = false;
		const FormFunction expansionFunction{ currentExpansionLine.lineFunction };
		FormLine newLine;
		const MFsize newLineIndex{ bodyLines.values.size() };
		newLine.ConfigureLine(lineName, newLineIndex, "", expansionFunction);
		newLine.isActive = true;
		activeLine = bodyLines.size();
		bodyLines.insert(std::make_pair(newLineIndex,newLine));
	};
	FormLine initialLine;
	//TODO: this would ideally be all new lines per buffer ','
	initialLine.ConfigureLine(lineName,0, resultBuffer.buffer, newLineFunction);
	bodyLines.insert(std::make_pair(0, initialLine));
}

FormFunction ReignEngine::CreateExpansionForm(const MFstring& formName, const MFsize lineIndex, std::stack<std::unique_ptr<Form>>& formStack, const MFsize maxEntries)
{
	return FormFunction{ [&formStack,formName, lineIndex,maxEntries](MFint32)->void
	{
		FormLine& expansionLine{ formStack.top()->bodyLines.at(lineIndex) };
		std::unique_ptr<ExpansionForm> expansionForm{ std::make_unique<ExpansionForm>() };
		expansionForm->ConfigureForm(formName, expansionLine.textBuffer, maxEntries);
		formStack.push(std::move(expansionForm));
	} };
}