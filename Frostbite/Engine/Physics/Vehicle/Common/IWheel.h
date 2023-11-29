#pragma once


namespace fb
{

class IWheel
{
public:

	enum WheelStatus
	{
		NoContact,
		GripContact,
		SlipContact
	};

	enum StateLod
	{
		LodUncompressed,
		LodPrediction,
		LodReplication,
		LodSavePrediction,
		LodCount
	};

	enum NetworkSide
	{
		Client,
		Server
	};

protected:
	void** __vftable;
};

}