#pragma once
#include <Core/Types.h>
#include "RpcTypeTraits.h"


namespace fb
{

class RpcNetworkable;


class InvokerBase
{
protected:
	RpcNetworkable* m_rpcNetworkable;
	u8 m_targetConnection;
	u8 m_flags;
	bool m_hasTarget;
};

template <uint methodId, RpcDirection direction>
class RpcDeclaration
{
public:
	class Invoker : public InvokerBase
	{
	};
};

template <uint methodId, RpcDirection direction, typename Param>
class RpcDeclaration1
{
public:
	class Invoker : public InvokerBase
	{
	};
};

}