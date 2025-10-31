#pragma once
#include <array>
#include <functional>
#include <ranges>

#include <ManifestMath/Vector3.h>

#include <ManifestUtility/Typenames.h>
#include <ManifestUtility/DebugLogger.h>
using namespace Manifest_Utility;

namespace Manifest_Memory
{
	using Descriptor = MFu64;
	using Representation = Descriptor;

	//large enough to store a quaternion
	constexpr MFsize QUTARENION_SIZE{ 16 };
	constexpr MFsize MAX_STATE_DATA_SIZE{ QUTARENION_SIZE };
	template<typename StateType>
	concept ValidState = sizeof(StateType) <= MAX_STATE_DATA_SIZE;

	struct StateBuffer
	{
		StateBuffer() = default;

		template<ValidState StateType>
		StateBuffer(StateType&& state)
		{
			this->Write(std::forward<StateType>(state));
		};
		StateBuffer(const StateBuffer& other)
		{
			this->data = other.data;
		};
		StateBuffer(StateBuffer&& other) noexcept
		{//buffer is preallocated does this really matter?
			data = std::move(other.data);
		};
		StateBuffer& operator=(const StateBuffer& other)
		{
			if (this != &other)
				data = other.data;

			return *this;
		}

		StateBuffer& operator=(StateBuffer&& other) noexcept
		{
			if (this != &other)
				data = std::move(other.data);

			return *this;
		}
		MFbool operator==(const StateBuffer& other) const
		{
			MFu8 const* const a{ data.data() };
			MFu8 const* const b{ other.data.data() };

			return std::memcmp(a, b, MAX_STATE_DATA_SIZE) == 0;
		}
		template<ValidState StateType>
		const StateType& Read() const
		{
			return reinterpret_cast<const StateType&>(*data.data());
		}
	private:
		template<ValidState StateType>
		void Write(StateType&& objective)
		{
			std::memset(data.data(), 0, MAX_STATE_DATA_SIZE);
			std::memcpy(data.data(), &objective, sizeof(objective));
		};

		std::array<MFu8, MAX_STATE_DATA_SIZE> data;
	};


}