#pragma once


bool tixml_value(TiXmlNode* node, string& value);
bool tixml_value(TiXmlNode* node, int& value);
bool tixml_value(TiXmlNode* node, float& value);
bool tixml_value(TiXmlNode* node, unsigned& value);
bool tixml_value(TiXmlNode* node, double& value);