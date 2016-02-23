#include "pch.h"
#include "output_stream.h"


array<array<byte, CACHE_BUFFER_LEN>, 5> output_stream::cache_buffer{};
uint output_stream::instance_count = 0;

output_stream::output_stream(u_short protocal_id)
{
	assert(instance_count < cache_buffer.size());// ½ûÖ¹¶àÊµÀý
	if (instance_count > cache_buffer.size())
	{
		cout << "fatal error, output_stream out of index." << endl;
	}
	buffer = cache_buffer[instance_count].data();
	write_ushort(protocal_id);
	instance_count++;
}


output_stream::~output_stream(void)
{
	instance_count--;
}


bool output_stream::write_int(int value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(int*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}


bool output_stream::write_uint(unsigned value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(unsigned*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_bool(bool value)
{
	if(pos + sizeof(byte) > CACHE_BUFFER_LEN)
		return false;
	*(byte*)(buffer + pos) = value ? 1 : 0;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_float(float value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(float*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_double(double& value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(double*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_ushort(u_short value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(u_short*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_short(short value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(short*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_char(char value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(char*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_byte(byte value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(byte*)(buffer + pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_string(const string& value)
{
	auto len = value.size();
	if(pos + 2 + len > CACHE_BUFFER_LEN)
		return false;
	*(u_short*)(buffer + pos) = (u_short)len;
	//memcpy_s(cache_buffer+pos+2, len, &value[0], len);
	memcpy(buffer + pos + 2, &value[0], len);
	pos += 2+len;
	return true;
}

bool output_stream::write_data(const void* data, size_t data_len, int offset)
{
	if(pos + data_len > CACHE_BUFFER_LEN)
		return false;
	memcpy(buffer + (offset == -1 ? pos : offset), data, data_len);
	if(offset==-1)pos += data_len;
	return true;
}


//template<class T> bool output_stream::write_struct(const T& value)
//{
//	if(pos + sizeof(value) > len)
//		return false;
//	*(T*)(cache_buffer+pos) = value;
//	pos += sizeof(value);
//
//	return true;
//}


void output_stream::write_head()
{
	*(u_short*)(buffer) = (u_short)pos;
}