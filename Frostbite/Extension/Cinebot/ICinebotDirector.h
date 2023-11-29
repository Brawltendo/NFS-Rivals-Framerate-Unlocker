#pragma once


namespace fb
{

class ICinebotDirector
{
public:
	void setTimeScale(float timeScale)
	{
		typedef void(__fastcall* native)(ICinebotDirector*, float);
		reinterpret_cast<native>(__vftable[3])(this, timeScale);
	}

protected:
	void** __vftable;
};

}