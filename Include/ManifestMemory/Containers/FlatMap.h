#pragma once
#include <vector>
#include <unordered_map>

#include <ManifestUtility/Typenames.h>

using namespace Manifest_Utility;

namespace Manifest_Memory
{
	template<typename T>
	struct FlatMap
	{
		std::vector<T> values;
		std::vector<MFu64> IDs;
		std::unordered_map<MFu64, MFsize> IDtoIndexMap;

		MFsize IDIndex(const MFu64 ID) const
		{
			assert(contains(ID));
			return IDtoIndexMap.at(ID);
		}

		MFbool contains(const MFu64 ID) const
		{
			return IDtoIndexMap.contains(ID);
		}

		void set(const MFu64 ID, T&& value)
		{
			if (contains(ID))
			{
				const MFsize index{ IDtoIndexMap.at(ID) };
				values.at(index) = std::move(value);
			}
			else
				insert(std::make_pair(ID, std::move(value)));
		}

		T& insert(std::pair<MFu64, T>&& insertion)
		{
			auto&& [ID, value] { std::move(insertion) };
			if (contains(ID))
				return values.at(ID);

			IDtoIndexMap.insert(std::make_pair(ID, values.size()));
			IDs.emplace_back(ID);
			return values.emplace_back(std::move(value));
		}

		void erase(MFu64 removedID)
		{
			auto indexIterator = IDtoIndexMap.find(removedID);
			if (indexIterator == IDtoIndexMap.end())
				return;

			MFsize index = indexIterator->second;
			if (index != values.size() - 1)
			{
				values.at(index) = std::move(values.back());
				IDs.at(index) = IDs.back();
				IDtoIndexMap.at(IDs.at(index)) = index;
			}
			values.pop_back();
			IDs.pop_back();
			IDtoIndexMap.erase(indexIterator);
		}

		MFsize size() const
		{
			assert(values.size() == IDs.size());
			return values.size();
		};

		T& at(const MFu64 ID)
		{
			assert(contains(ID));
			const MFsize index{ IDtoIndexMap.at(ID) };
			return values.at(index);
		}

		const T& at(const MFu64 ID) const
		{
			assert(contains(ID));
			const MFsize index{ IDtoIndexMap.at(ID) };
			return values.at(index);
		}

		typename std::vector<T>::iterator begin() { return values.begin(); }
		typename std::vector<T>::iterator end() { return values.end(); }
		typename std::vector<T>::const_iterator begin() const { return values.begin(); }
		typename std::vector<T>::const_iterator end() const { return values.end(); }
	private:
		T& operator[](const MFu64 ID) = delete;
		const T& operator[](const MFu64 ID) const = delete;
	};
}