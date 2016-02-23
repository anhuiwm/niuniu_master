#pragma once
#include "config.h"


#define CACHE_BUFFER_LEN ((uint16)-1)

class output_stream
{
	byte* buffer = nullptr;
	size_t pos = 2;
	static array<array<byte, CACHE_BUFFER_LEN>, 5> cache_buffer;
	static uint instance_count;
	void write_head();
public:
	output_stream(u_short protocal_id);
	~output_stream(void);
	size_t length(){return pos;}
	char* get_buffer(){ write_head(); return (char*)buffer; }

	bool write_int(int value);
	bool write_uint(unsigned value);
	bool write_bool(bool value);
	bool write_float(float value);
	bool write_double(double& value);
	bool write_ushort(u_short value);
	bool write_short(short value);
	bool write_char(char value);
	bool write_byte(byte value);
	bool write_string(const string& value);
	bool write_data(const void* data, size_t data_len, int offset = -1);
	//template<class T> bool write_struct(const T& value);

};

