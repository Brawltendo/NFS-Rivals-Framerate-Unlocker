#pragma once


namespace EA
{
namespace Audio
{
namespace Core
{

enum ParameterType
{
	PARAMETERTYPE_NANPATTERN                  = 0x7ff7fff0,

	PARAMETERTYPE_ENUM                        = 0,
	PARAMETERTYPE_NANPATTERN_ENUM             = PARAMETERTYPE_ENUM | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_FLOAT32                     = 1,
	PARAMETERTYPE_NANPATTERN_FLOAT32          = PARAMETERTYPE_FLOAT32 | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_FLOAT64                     = 2,
	PARAMETERTYPE_NANPATTERN_FLOAT64          = PARAMETERTYPE_FLOAT64 | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_POINTERTOSTRING             = 3,
	PARAMETERTYPE_NANPATTERN_POINTERTOSTRING  = PARAMETERTYPE_POINTERTOSTRING | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_POINTER                     = 4,
	PARAMETERTYPE_NANPATTERN_POINTER          = PARAMETERTYPE_POINTER | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_UINT32                      = 5,
	PARAMETERTYPE_NANPATTERN_UINT32           = PARAMETERTYPE_UINT32 | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_INT32                       = 6,
	PARAMETERTYPE_NANPATTERN_INT32            = PARAMETERTYPE_INT32 | PARAMETERTYPE_NANPATTERN,

	PARAMETERTYPE_BOOL                        = 7,
	PARAMETERTYPE_NANPATTERN_BOOL             = PARAMETERTYPE_BOOL | PARAMETERTYPE_NANPATTERN

};


class Param
{
public:

	void SetFloat(float value) { mSafeFloat.value = value; }
	float GetFloat() const { return mSafeFloat.value; }

	void SetDouble(double value) { mDouble = value; }
	double GetDouble() const { return mDouble; }

	void SetPString(const char* value) { mSafePString.value = value; }
	const char* GetPString() const { return mSafePString.value; }

	void SetPointer(void* value) { mSafePointer.value = value; }
	void* GetPointer() const { return mSafePointer.value; }

	void SetInt(int value) { mSafeInt.value = value; }
	int GetInt() const { return mSafeInt.value; }

	void SetUInt(unsigned int value) { mSafeUInt.value = value; }
	unsigned int GetUInt() const { return mSafeUInt.value; }

	void SetBool(bool value) { mSafeBool.value = value; }
	bool GetBool() const { return mSafeBool.value; }

	ParameterType GetTypeInfo() const { return mSafeFloat.paramType; }


	template<class T>
	struct SafePointerValue
	{
		T value;
	};

	template<class T>
	struct SafeValue
	{
		ParameterType paramType;
		T value;
	};


private:
	union
	{
		SafePointerValue<void*> mSafePointer;
		SafePointerValue<const char*> mSafePString;
		SafeValue<float> mSafeFloat;
		SafeValue<int> mSafeInt;
		SafeValue<unsigned int> mSafeUInt;
		SafeValue<bool> mSafeBool;
		double mDouble;
	};
};
static_assert(sizeof(Param) == 0x8, "Wrong size!");

} // namespace Core
} // namespace Audio
} // namespace EA