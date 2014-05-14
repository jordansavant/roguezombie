#pragma once
#ifndef RZ_INTERIOR_H
#define RZ_INTERIOR_H

class Interior
{
public:

    enum Spawn
    {
        Wall = 1,
        Zombie = 2,
        Ogre = 3,
        Light = 4,
        Door = 5,
        Chest = 6,
        Hunter = 7,
        PortalTo2 = 8,
        PortalTo1 = 9,
    };

    static int level_01_rows;
    static int level_01_cols;
    static int level_01[];

    static int level_02_rows;
    static int level_02_cols;
    static int level_02[];
};

#endif
