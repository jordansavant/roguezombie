#pragma once
#ifndef BIT_FIELDOFVIEW_H
#define BIT_FIELDOFVIEW_H

#include <functional>
#include <vector>
#include <list>
#include <complex>
#include <cmath>
#include "Node.hpp"
#include "../Math/Math.hpp"
#include "../Math/VectorMath.hpp"

namespace bit
{
    class FieldOfView
    {
    public:

        static int multipliers[4][8];
        
        // Determines which co-ordinates on a 2D grid are visible from a particular co-ordinate.
        // x, y: center of view
        // radius: how far field of view extends
        template<class Map>
        static void do_fov(Map& map, unsigned int x, unsigned int y, unsigned int radius)
        {
            // Iterate across the octants and cast
            for (unsigned int i = 0; i < 8; i++)
            {
                cast_light(map, x, y, radius, 1, 1.0, 0.0, multipliers[0][i], multipliers[1][i], multipliers[2][i], multipliers[3][i]);
            }
        }

        // Recursive light casting function
        template<class Map>
        static void cast_light(Map& map, unsigned int x, unsigned int y, unsigned int radius, unsigned int row, float start_slope, float end_slope, unsigned int xx, unsigned int xy, unsigned int yx,  unsigned int yy)
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
                    // Translate the dx, dy co-ordinates into map co-ordinates
                    // ??? TODO ???
                    // mx, my = cx + dx * xx + dy * xy, cy + dx * yx + dy * yy

                    // l_slope and r_slope store the slopes of the left and right extremities of the square we're considering:
                    float l_slope = (dx - 0.5) / (dy + 0.5);
                    float r_slope = (dx + 0.5) / (dy - 0.5);
                    
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
                    if (ax >= map.get_width() || ay >= map.get_height()) // TODO: map
                    {
                        continue;
                    }

                    // Our light beam is touching this square; light it
                    unsigned int radius2 = radius * radius;
                    if ((unsigned int)(dx * dx + dy * dy) < radius2)
                    {
                        map.set_visible(ax, ay);//, true); // TODO: marks as visible
                    }

                    if (blocked)
                    {
                        // We're scanning a row of blocked squares
                        if (map.is_opaque(ax, ay)) // TODO: map.isBlocked(mx, my) ?
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
                    else if (map.is_opaque(ax, ay)) // TODO: map.isBlocked(mx, my) ?
                    {
                        blocked = true;
                        next_start_slope = r_slope;
                        cast_light(map, x, y, radius, i + 1, start_slope, l_slope, xx, xy, yx, yy);
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