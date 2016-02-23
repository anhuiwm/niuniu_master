#include "pch.h"
#include "tinyxml_extend.h"



bool tixml_value(TiXmlNode* node, string& value)
{
	if (node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if (temp == nullptr)
		return false;
	value = temp->Value();
	return true;
}

bool tixml_value(TiXmlNode* node, int& value)
{
	if (node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if (temp == nullptr)
		return false;
	value = atoi(temp->Value());
	return true;
}

bool tixml_value(TiXmlNode* node, float& value)
{
	if (node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if (temp == nullptr)
		return false;
	value = (float)atof(temp->Value());
	return true;
}

bool tixml_value(TiXmlNode* node, unsigned& value)
{
	if (node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if (temp == nullptr)
		return false;
	value = (unsigned)atoi(temp->Value());
	return true;
}

bool tixml_value(TiXmlNode* node, double& value)
{
	if (node == nullptr)
		return false;
	auto temp = node->FirstChild();
	if (temp == nullptr)
		return false;
	value = atof(temp->Value());
	return true;
}