#pragma once


namespace fb
{

enum SubRealm
{
    SubRealm_All,
    SubRealm_LocalPlayer,
    SubRealm_RemotePlayer
};

enum EntityCreatorType
{
    EntityCreatorType_Unknown,
    EntityCreatorType_Level,
    EntityCreatorType_Spawner,
    EntityCreatorType_Owner,
    EntityCreatorType_Ghost
};

enum UpdatePass
{
    UpdatePass_PreSim,
    UpdatePass_PostSim,
    UpdatePass_PostFrame,
    UpdatePass_FrameInterpolation,
    UpdatePass_PreInput,
    UpdatePass_PreFrame,
    UpdatePass_PreAnimation,
    UpdatePass_AnimationAttach,
    UpdatePass_PostAnimation,
    UpdatePass_Count
};

}