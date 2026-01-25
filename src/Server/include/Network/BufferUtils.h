#pragma once

//#include "../stdafx.h"

#include <stdint.h>
#include <vector>

typedef unsigned int Remove;

#define Remove_Byte			1
#define Remove_CMD			1
#define Remove_LENGTH		2
#define Remove_IS_SERVER	1
#define Remove_UDP_ID		2
#define Remove_Float		sizeof(float)
#define Remove_Int32		sizeof(int32_t)
#define Remove_UInt32		sizeof(uint32_t)
#define Remove_Int64		sizeof(int64_t)
#define Remove_UInt64		sizeof(uint64_t)

class BufferUtils {
public:
	static std::vector<uint8_t> AddLength(std::vector<uint8_t> const & data) {
		uint16_t len = data.size();
		uint8_t* len_buff = (uint8_t*)&len;
		uint8_t lo = len_buff[0];// len & 0xFF;
		uint8_t hi = len_buff[1];// len >> 8;
		std::vector<uint8_t> len_v;// {lo, hi};
		len_v.push_back(lo);
		len_v.push_back(hi);

		return Add(len_v, data);
	}

	static std::vector<uint8_t> Add_Short(uint16_t val, std::vector<uint8_t> const & data) {
		//uint16_t len = data.size();
		uint8_t* udp_id_buff = (uint8_t*)&val;
		uint8_t lo = udp_id_buff[0];// len & 0xFF;
		uint8_t hi = udp_id_buff[1];// len >> 8;
		std::vector<uint8_t> id_v;// {lo, hi};
		id_v.push_back(lo);
		id_v.push_back(hi);

		return Add(id_v, data);
	}

	static std::vector<uint8_t> Add_Int(int32_t val, std::vector<uint8_t> const& data)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(int32_t));
		return Add(val_data, data);
	}

	static std::vector<uint8_t> Add_UInt(uint32_t val, std::vector<uint8_t> const& data)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(uint32_t));
		return Add(val_data, data);
	}

	static std::vector<uint8_t> Add_UDP_ID(uint16_t udp_id, std::vector<uint8_t> const & data) {
		return Add_Short(udp_id, data);
	}

	static std::vector<uint8_t> RemoveFront(Remove numToRemove, std::vector<uint8_t> const & origin)
	{
		std::vector<uint8_t> res;
		if ((int)numToRemove > origin.size())
		{
			return origin;
			//throw new Exception(string.Format("RemoveFront: received remove length ({0}) longer than buffer: {1}", (int)numToRemove, BitConverter.ToString(origin)));
		}

		/*res.assign(origin.begin() + numToRemove, origin.end());
		return res;*/

		const uint8_t* data = origin.data();

		return std::vector<uint8_t>(data + numToRemove, data + origin.size());
	}

	static std::vector<uint8_t> AddFirst(uint8_t byteToAdd, std::vector<uint8_t> const & origin)
	{
		std::vector<uint8_t> add_f;
		add_f.push_back(byteToAdd);

		return Add(add_f, origin);

		//origin.insert(origin.begin(), byteToAdd);
		//return origin;
	}

	static std::vector<uint8_t> AppendFloat(std::vector<uint8_t> const & data, float val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(float));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> AppendByte(std::vector<uint8_t> const & data, uint8_t val)
	{
		std::vector<uint8_t> val_data;
		val_data.push_back(val);
		return Add(data, val_data);
	}

	static std::vector<uint8_t> AppendUShort(std::vector<uint8_t> const& data, uint16_t val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(uint16_t));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> AppendShort(std::vector<uint8_t> const& data, int16_t val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(int16_t));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> Append_UInt32(std::vector<uint8_t> const& data, uint32_t val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(uint32_t));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> Append_Int32(std::vector<uint8_t> const& data, int32_t val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(int32_t));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> Append_UInt64(std::vector<uint8_t> const & data, uint64_t val)
	{
		std::vector<uint8_t> val_data((uint8_t*)&val, ((uint8_t*)&val) + sizeof(uint64_t));
		return Add(data, val_data);
	}

	static std::vector<uint8_t> Add(std::vector<uint8_t> buffer_1, std::vector<uint8_t> buffer_2) {
		int full_size = buffer_1.size() + buffer_2.size();
		uint8_t* res_buf = new uint8_t[full_size];
		memcpy(res_buf, buffer_1.data(), buffer_1.size());
		memcpy(&res_buf[buffer_1.size()], buffer_2.data(), buffer_2.size());
		std::vector<uint8_t> res(res_buf, res_buf + full_size);
		delete[] res_buf;
		return res;

		//std::vector<std::vector<uint8_t>> buffers;
		//buffers.push_back(buffer_1);
		//buffers.push_back(buffer_2);
		//return Add(buffers);
	}

	static std::vector<uint8_t> Add(std::vector<std::vector<uint8_t>>& buffers) {
		std::vector<uint8_t> res;
		for (int i = 0; i < buffers.size(); i++)
		{
			res.insert(res.end(), buffers[i].begin(), buffers[i].end());
		}
		return res;
	}

	static std::vector<uint8_t> Add(std::vector<uint8_t>* buffers_arr, int num) {

		std::vector<std::vector<uint8_t>> buffers;
		for (int i = 0; i < num; i++) {
			buffers.push_back(buffers_arr[i]);
		}
		return Add(buffers);
	}
};