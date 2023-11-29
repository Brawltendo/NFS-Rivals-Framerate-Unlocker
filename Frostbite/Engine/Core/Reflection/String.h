#pragma once


namespace fb
{

class String
{
public:
	const char* c_str() const { return m_chars; }

private:
	char* m_chars;

};

}