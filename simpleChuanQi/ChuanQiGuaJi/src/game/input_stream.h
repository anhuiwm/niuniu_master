#pragma once
#include "config.h"

class input_stream
{
	size_t pos;
	const char* buffer;
	size_t len;
public:
	input_stream(const char* buffer, size_t len);
	input_stream(const byte* buffer, size_t len);
	~input_stream(void);

	bool read_int(int& value);
	bool read_uint(unsigned& value);
	bool read_bool(bool& value);
	bool read_float(float& value);
	bool read_double(double& value);
	bool read_ushort(u_short& value);
	bool read_short(short& value);
	bool read_char(char& value);
	bool read_byte(byte& value);
	bool read_string(string& value);
	template<class T> bool read_struct(T& value);
};

