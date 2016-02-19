#include "output_stream.h"


byte output_stream::cache_buffer[CACHE_BUFFER_LEN] = {0};

output_stream::output_stream(u_short protocal_id):pos(2)
{
	write_ushort(protocal_id);
}


output_stream::~output_stream(void)
{
}


bool output_stream::write_int(int value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(int*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}


bool output_stream::write_uint(unsigned value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(unsigned*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_bool(bool value)
{
	if(pos + sizeof(byte) > CACHE_BUFFER_LEN)
		return false;
	*(byte*)(cache_buffer+pos) = value?1:0;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_float(float value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(float*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_double(double& value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(double*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}
bool output_stream::write_ushort(u_short value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(u_short*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_short(short value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(short*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_char(char value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(char*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_byte(byte value)
{
	if(pos + sizeof(value) > CACHE_BUFFER_LEN)
		return false;
	*(byte*)(cache_buffer+pos) = value;
	pos += sizeof(value);
	return true;
}

bool output_stream::write_string(const string& value)
{
	auto len = value.size();
	if(pos + 2 + len > CACHE_BUFFER_LEN)
		return false;
	*(u_short*)(cache_buffer+pos) = (u_short)len;
	//memcpy_s(cache_buffer+pos+2, len, &value[0], len);
	memcpy(cache_buffer+pos+2, &value[0], len);
	pos += 2+len;
	return true;
}

bool output_stream::write_data(const char* data, size_t data_len, int offset)
{
	if(pos + data_len > CACHE_BUFFER_LEN)
		return false;
	memcpy(cache_buffer+(offset==-1?pos:offset), data, data_len);
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
	*(u_short*)(cache_buffer) = (u_short)pos;
}