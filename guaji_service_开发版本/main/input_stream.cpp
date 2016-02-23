#include "pch.h"
#include "input_stream.h"


input_stream::input_stream(const char* buffer, size_t len):pos(0), buffer(buffer), len(len)
{
}
input_stream::input_stream(const byte* buffer, size_t len):pos(0), buffer((char*)buffer), len(len)
{
}


input_stream::~input_stream(void)
{
}

bool input_stream::read_int(int& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(int*)(buffer+pos);
	pos += sizeof(value);
	return true;
}


bool input_stream::read_uint(unsigned& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(unsigned*)(buffer+pos);
	pos += sizeof(value);
	return true;
}
bool input_stream::read_bool(bool& value)
{
	if(pos + sizeof(byte) > len)
		return false;
	value = *(byte*)(buffer+pos) > 0;
	pos += sizeof(value);
	return true;
}
bool input_stream::read_float(float& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(float*)(buffer+pos);
	pos += sizeof(value);
	return true;
}
bool input_stream::read_double(double& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(double*)(buffer+pos);
	pos += sizeof(value);
	return true;
}
bool input_stream::read_ushort(u_short& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(u_short*)(buffer+pos);
	pos += sizeof(value);
	return true;
}

bool input_stream::read_short(short& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(short*)(buffer+pos);
	pos += sizeof(value);
	return true;
}

bool input_stream::read_char(char& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(char*)(buffer+pos);
	pos += sizeof(value);
	return true;
}

bool input_stream::read_byte(byte& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(byte*)(buffer+pos);
	pos += sizeof(value);
	return true;
}

bool input_stream::read_string(string& value)
{
	if(pos + 2 > len)
		return false;
	u_short str_len = *(u_short*)(buffer+pos);
	if(pos + 2 + str_len > len)
		return false;
	value = string(buffer+pos+2, str_len);
	pos += 2+str_len;
	return true;
}


template<class T> bool input_stream::read_struct(T& value)
{
	if(pos + sizeof(value) > len)
		return false;
	value = *(T*)(buffer+pos);
	pos += sizeof(value);
	return true;
}