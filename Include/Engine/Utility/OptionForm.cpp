#include "OptionForm.h"

using namespace ReignEngine;

void OptionForm::ConfigureForm(const MFstring& formName, const OptionList& optionList, const ReturnValue returnValue, TextBuffer& resultBuffer, const MFsize maxSelections)
{
	this->returnValue = returnValue;
	this->maxSelections = maxSelections;
	headerLine.lineName = formName;
	//TODO:place in footer/update x/x value
	headerLine.lineName += +" MAX SELECTIONS:" + std::to_string(maxSelections);

	exitFunction = [this, &resultBuffer](MFint32)->void
	{
		resultBuffer.buffer = BuildReturnString(this->selectedLines, this->bodyLines, this->returnValue);
	};

	for (const auto& listOption : optionList)
	{
		const auto [ID, name] { listOption};
		const MFsize lineIndex{ bodyLines.size() };
		FormLine optionLine;
		optionLine.ConfigureLine(name, lineIndex, std::to_string(ID), [this, lineIndex](MFint32)->void
		{
			const MFsize selectionCount{ selectedLines.size() };
			const auto selectedLine{ std::ranges::find(selectedLines,lineIndex) };
			if (selectedLine == selectedLines.end() && selectionCount < this->maxSelections)
			{
				selectedLines.emplace_back(lineIndex);
				MFstring& lineName{ bodyLines.at(lineIndex).lineName };
				lineName.insert(lineName.end(), '*');
			}
			else if (selectedLine != selectedLines.end())
			{
				std::_Erase_remove(selectedLines, lineIndex);
				MFstring& lineName{ bodyLines.at(lineIndex).lineName };
				lineName.erase(lineName.end() - 1);
			}

		}, false);
		bodyLines.insert(std::make_pair(lineIndex, std::move(optionLine)));
	}
	if (bodyLines.size())
		bodyLines.at(0).isActive = true;
}
FormFunction ReignEngine::CreateOptionForm(const MFstring& formName, const OptionList& optionList, const ReturnValue returnValue, const MFsize lineIndex, std::stack<std::unique_ptr<Form>>& formStack, const MFsize maxSelections)
{
	return FormFunction{ [&formStack,&optionList,formName, returnValue, lineIndex,maxSelections](MFint32)->void
	{
		FormLine& optionLine{ formStack.top()->bodyLines.at(lineIndex) };
		std::unique_ptr<OptionForm> optionForm{ std::make_unique<OptionForm>() };
		optionForm->ConfigureForm(formName, optionList, returnValue, optionLine.textBuffer, maxSelections);
		formStack.push(std::move(optionForm));
	} };
}