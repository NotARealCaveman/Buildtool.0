#pragma once
#include "Form.h"

namespace ReignEngine
{
	struct OptionForm : public Form
	{
		OptionForm() = default;
		void ConfigureForm(const MFstring& formName, const OptionList& optionList, const ReturnValue returnValue, TextBuffer& resultBuffer, const MFsize maxSelections);

		MFsize maxSelections;
		std::vector<MFu32> selectedLines;
		ReturnValue returnValue{ ReturnValue::DEFAULT };
	};

	FormFunction CreateOptionForm(const MFstring& formName, const OptionList& optionList, const ReturnValue returnValue, const MFsize lineIndex, std::stack<std::unique_ptr<Form>>& formStack, const MFsize maxSelections = std::numeric_limits<MFsize>::max());
}