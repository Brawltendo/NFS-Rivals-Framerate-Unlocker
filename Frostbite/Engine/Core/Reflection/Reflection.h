#pragma once
#include <Engine/Core/Types.h>


namespace fb
{

class ModuleInfo;


struct AttributeSpec
{
};

struct MemberInfoFlags
{
	u16 flagBits;

	enum
	{
		kMemberTypeMask     = 3,
		kTypeCategoryShift  = 1 << 1,
		kTypeCategoryMask   = 3,
		kTypeCodeShift      = 1 << 2,
		kTypeCodeMask       = 31,
		kMetadata           = 1 << 11,
		kHomogeneous        = 1 << 12,
		kAlwaysPersist      = 1 << 13,
		kExposed            = 1 << 13,
		kLayoutImmutable    = 1 << 14,
		kBlittable          = 1 << 15
	};
};

class MemberInfo
{
public:
	struct MemberInfoData
	{
		const char* name;
		MemberInfoFlags flags;
	};

	const char* getName() { return getMemberInfoData()->name; }

protected:
	const void* getInfoData() { return m_infoData; }
	const MemberInfoData* getMemberInfoData() { return reinterpret_cast<const MemberInfoData*>(m_infoData); }

	const void* m_infoData;
};

class TypeInfo : public MemberInfo
{
public:
	struct TypeInfoData
	{
		const char* name;
		MemberInfoFlags flags;
		u16 totalSize;
		ModuleInfo* module;
		u8 alignment;
		u8 fieldCount;
		u8 pad1;
		u8 pad2;
		const AttributeSpec* attributes;
	};

	u16 getSize() { return getTypeInfoData()->totalSize; }

protected:
	const TypeInfoData* getTypeInfoData() { return reinterpret_cast<const TypeInfoData*>(m_infoData); }

	TypeInfo* m_pNext;

private:
	u16 m_runtimeId;
	u16 m_flags;
};

struct ITypedObject
{
protected:
	void** __vftable;
};

}