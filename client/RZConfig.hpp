#pragma once
#ifndef RZ_RZCONFIG_H
#define RZ_RZCONFIG_H

class RZConfig
{
public:

    static int zrenderRange;

    static float zrenderGameplayLowerbound;
    static float zrenderGameplayUpperbound;

    static float zrenderInterfaceLowerbound;
    static float zrenderInterfaceUpperbound;

    static float getDrawDepthForGameplay(float yPosition);
};

#endif