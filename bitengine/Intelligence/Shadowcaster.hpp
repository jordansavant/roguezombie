#pragma once
#ifndef BIT_SHADOWCASTER_H
#define BIT_SHADOWCASTER_H

#include <functional>
#include <complex>
#include <vector>
#include "Node.hpp"
#include "../Math/Math.hpp"
#include "../Math/VectorMath.hpp"

namespace bit
{
    class Shadowcaster
    {
    public:

        // Determines which co-ordinates on a 2D grid are visible from a particular co-ordinate.
        // x, y: center of view
        // radius: how far field of view extends
        static void computeFoV(unsigned int x, unsigned int y, unsigned int radius, std::function<void(int, int, int)> setVisible, std::function<bool(int, int)> isBlocked)
        {
            // Always mark base point
            setVisible(x, y, 1);

            // Iterate across the octants and cast
            for (unsigned int i = 0; i < 8; i++)
            {
                castLight(x, y, radius, setVisible, isBlocked, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
            }
        }


    private:

        static int multipliers[4][8];

        // Recursive light casting function
        static void castLight(unsigned int x, unsigned int y, unsigned int radius, std::function<void(int, int, int)> setVisible, std::function<bool(int, int)> isBlocked, unsigned int row, float start_slope, float end_slope, unsigned int xx, unsigned int xy, unsigned int yx,  unsigned int yy)
        {
            // If light start is less than light end, return
            if (start_slope < end_slope)
            {
                return;
            }

            float next_start_slope = start_slope;

            // Inclusive loop across rows
            for (unsigned int i = row; i <= radius; i++)
            {
                bool blocked = false;

                for (int dx = -i, dy = -i; dx <= 0; dx++)
                {
                    // l_slope and r_slope store the slopes of the left and right extremities of the square we're considering:
                    float l_slope = (dx - 0.5f) / (dy + 0.5f);
                    float r_slope = (dx + 0.5f) / (dy - 0.5f);

                    if (start_slope < r_slope)
                    {
                        continue;
                    }
                    else if (end_slope > l_slope)
                    {
                        break;
                    }

                    int sax = dx * xx + dy * xy;
                    int say = dx * yx + dy * yy;
                    if ((sax < 0 && (unsigned int)std::abs(sax) > x) || (say < 0 && (unsigned int)std::abs(say) > y))
                    {
                        continue;
                    }

                    unsigned int ax = x + sax;
                    unsigned int ay = y + say;
                    // Commenting this out to remove dependency on Map
                    // looks like bounds checking for perf boost in edget of map
                    //if (ax >= map.shadowcastGetWidth() || ay >= map.shadowcastGetHeight())
                    //{
                    //    continue;
                    //}

                    // Our light beam is touching this square; light it
                    unsigned int radius2 = radius * radius;
                    if ((unsigned int)(dx * dx + dy * dy) < radius2)
                    {
                        setVisible(ax, ay, i);
                    }

                    if (blocked)
                    {
                        // We're scanning a row of blocked squares
                        if (isBlocked(ax, ay))
                        {
                            next_start_slope = r_slope;
                            continue;
                        }
                        else
                        {
                            blocked = false;
                            start_slope = next_start_slope;
                        }
                    }
                    else if (isBlocked(ax, ay))
                    {
                        blocked = true;
                        next_start_slope = r_slope;
                        castLight(x, y, radius, setVisible, isBlocked, i + 1, start_slope, l_slope, xx, xy, yx, yy);
                    }
                }

                if (blocked)
                {
                    break;
                }
            }
        }

    };
}

#endif;