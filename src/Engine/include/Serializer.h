/*
	Simple structure-binary serializer for C++, version 1.
	
	Copyright 2025 Tom-Andre Barstad.

	This software is provided "as is", without any express or implied warranties,
	including but not limited to the implied warranties of merchantability and
	fitness for a particular purpose.  In no event will the authors or contributors
	be held liable for any direct, indirect, incidental, special, exemplary, or
	consequential damages however caused and on any theory of liability, whether in
	contract, strict liability, or tort (including negligence or otherwise),
	arising in any way out of the use of this software, even if advised of the
	possibility of such damage.

	Permission is granted to anyone to use this software for any purpose, including
	commercial applications, and to alter and distribute it freely in any form,
	provided that the following conditions are met:

	1. The origin of this software must not be misrepresented; you must not claim
	   that you wrote the original software. If you use this software in a product,
	   an acknowledgment in the product documentation would be appreciated but is
	   not required.

	2. Altered source versions may not be misrepresented as being the original
	   software, and neither the name of Tom-Andre Barstad nor the names of
	   authors or contributors may be used to endorse or promote products derived
	   from this software without specific prior written permission.

	3. This notice must be included, unaltered, with any source distribution.
*/

#pragma once

#include <vector>
#include <cstdint>
#include <type_traits>

/*
 * Usage
 *
 * struct SomeContract
 * {
 *     // Note: No semi-colon after this:
 *     SERIALIZED_MEMBERS(x, y, whatever)
 *     int x;
 *     int y;
 *     float whatever;
 * };
 *
 *
 * std::vector<std::uint8_t> makeContractAsBytes()
 * {
 *     SomeContract contract;
 *	   contract.x = 123;
 *     contract.y = 456;
 *     contract.whatever = 3.14;
 *
 *     // contract.serializedSize() will hold total amount of bytes that will go into your buffer.
 *
 *     return contract.serialize();
 * }
 *
 * SomeContract bufferToContract(const std::vector<std::uint8_t>& data)
 * {
 *     SomeContract contract{};
 *     contract.deserialize(data);
 *
 *     // Optionally if your data is at an offset, you can specify it:
 *     // contract.deserialize(data, 16);
 *
 *     return contract;
 * }
*/

namespace serializer_detail
{
    auto toRaw(auto value)
    {
        auto p = reinterpret_cast<void*>(&value);

        if constexpr (sizeof(value) == 1)
        {
            return *reinterpret_cast<std::uint8_t*>(p);
        }
        else if constexpr (sizeof(value) == 2)
        {
            return *reinterpret_cast<std::uint16_t*>(p);
        }
        else if constexpr (sizeof(value) == 4)
        {
            return *reinterpret_cast<std::uint32_t*>(p);
        }
        else
        {
            return *reinterpret_cast<std::uint64_t*>(p);
        }
    }

    template<typename T>
    T fromRaw(auto rawValue)
    {
        auto p = reinterpret_cast<void*>(&rawValue);
        return *reinterpret_cast<T*>(p);
    }

    void serializeValue(auto value, std::vector<std::uint8_t>& vec)
    {
        auto raw = toRaw(value);
        decltype(raw) mask = 0;
        for (int i = 0; i < sizeof(raw); ++i)
        {
            auto b = static_cast<std::uint8_t>(raw & 0xFF);
            raw >>= 8;
            vec.emplace_back(b);
        }
    }

    template<typename T>
    T deserializeValue(int32_t offset, const auto& vec)
    {
        if (offset + sizeof(T) > vec.size())
        {
            return {};
        }

        auto raw = toRaw((T)0);
        for (int i = offset + sizeof(T) - 1; i >= offset; --i)
        {
            if (i < offset + sizeof(T) - 1)
                raw <<= 8;

            raw |= vec[i];
        }

        return fromRaw<T>(raw);
    }

    std::int32_t sumMemberSizes(const auto&...members)
    {
        return (sizeof(members) + ...);
    }

    void serializeMembers(std::vector<std::uint8_t>& out, const auto&...members)
    {
        ( serializer_detail::serializeValue(members, out), ... );
    }

    std::int32_t deserializeSingleMember(std::int32_t offset, const auto& vec, auto& member)
    {
        using memberT = std::remove_reference_t<decltype(member)>;
        member = serializer_detail::deserializeValue<memberT>(offset, vec);
        return offset + sizeof(member);
    }

    void deserializeMembers(const auto& vec, std::int32_t offset, auto&...members)
    {
        ( (offset = serializer_detail::deserializeSingleMember(offset, vec, members)), ... );
    }
}

#define SERIALIZED_MEMBERS(...)                                                         \
    std::vector<std::uint8_t> serialize() const                                         \
    {                                                                                   \
        std::vector<std::uint8_t> serializer_output;                                    \
        serializer_output.reserve( serializer_detail::sumMemberSizes(__VA_ARGS__) );    \
        serializer_detail::serializeMembers(serializer_output, __VA_ARGS__);            \
        return serializer_output;                                                       \
    }                                                                                   \
    void deserialize(const auto& serializer_input, std::int32_t offset = 0)             \
    {                                                                                   \
        serializer_detail::deserializeMembers(serializer_input, offset, __VA_ARGS__);   \
    }                                                                                   \
    std::int32_t serializedSize() const                                                 \
    {                                                                                   \
        return serializer_detail::sumMemberSizes(__VA_ARGS__);                          \
    }
