#pragma once

#include "Form.h"

namespace ReignEngine
{
	struct ExpansionForm : public Form
	{
		ExpansionForm() = default;
		void ConfigureForm(const MFstring& formName, TextBuffer& resultBuffer, const MFsize maxEntries);

		MFsize maxEntries;
	};

	FormFunction CreateExpansionForm(const MFstring& formName, const MFsize lineIndex, std::stack<std::unique_ptr<Form>>& formStack, const MFsize maxEntries = std::numeric_limits<MFsize>::max());
}