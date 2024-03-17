/*
Copyright (C) 1997-2001 Id Software, Inc.

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
#include <array>

constexpr std::array<std::array<float, 3>, NUMVERTEXNORMALS> createVertTexNormal()
{
    return {
        std::array<float, 3>{-0.525731, 0.000000, 0.850651},   std::array<float, 3>{-0.442863, 0.238856, 0.864188},
        std::array<float, 3>{-0.295242, 0.000000, 0.955423},   std::array<float, 3>{-0.309017, 0.500000, 0.809017},
        std::array<float, 3>{-0.162460, 0.262866, 0.951056},   std::array<float, 3>{0.000000, 0.000000, 1.000000},
        std::array<float, 3>{0.000000, 0.850651, 0.525731},    std::array<float, 3>{-0.147621, 0.716567, 0.681718},
        std::array<float, 3>{0.147621, 0.716567, 0.681718},    std::array<float, 3>{0.000000, 0.525731, 0.850651},
        std::array<float, 3>{0.309017, 0.500000, 0.809017},    std::array<float, 3>{0.525731, 0.000000, 0.850651},
        std::array<float, 3>{0.295242, 0.000000, 0.955423},    std::array<float, 3>{0.442863, 0.238856, 0.864188},
        std::array<float, 3>{0.162460, 0.262866, 0.951056},    std::array<float, 3>{-0.681718, 0.147621, 0.716567},
        std::array<float, 3>{-0.809017, 0.309017, 0.500000},   std::array<float, 3>{-0.587785, 0.425325, 0.688191},
        std::array<float, 3>{-0.850651, 0.525731, 0.000000},   std::array<float, 3>{-0.864188, 0.442863, 0.238856},
        std::array<float, 3>{-0.716567, 0.681718, 0.147621},   std::array<float, 3>{-0.688191, 0.587785, 0.425325},
        std::array<float, 3>{-0.500000, 0.809017, 0.309017},   std::array<float, 3>{-0.238856, 0.864188, 0.442863},
        std::array<float, 3>{-0.425325, 0.688191, 0.587785},   std::array<float, 3>{-0.716567, 0.681718, -0.147621},
        std::array<float, 3>{-0.500000, 0.809017, -0.309017},  std::array<float, 3>{-0.525731, 0.850651, 0.000000},
        std::array<float, 3>{0.000000, 0.850651, -0.525731},   std::array<float, 3>{-0.238856, 0.864188, -0.442863},
        std::array<float, 3>{0.000000, 0.955423, -0.295242},   std::array<float, 3>{-0.262866, 0.951056, -0.162460},
        std::array<float, 3>{0.000000, 1.000000, 0.000000},    std::array<float, 3>{0.000000, 0.955423, 0.295242},
        std::array<float, 3>{-0.262866, 0.951056, 0.162460},   std::array<float, 3>{0.238856, 0.864188, 0.442863},
        std::array<float, 3>{0.262866, 0.951056, 0.162460},    std::array<float, 3>{0.500000, 0.809017, 0.309017},
        std::array<float, 3>{0.238856, 0.864188, -0.442863},   std::array<float, 3>{0.262866, 0.951056, -0.162460},
        std::array<float, 3>{0.500000, 0.809017, -0.309017},   std::array<float, 3>{0.850651, 0.525731, 0.000000},
        std::array<float, 3>{0.716567, 0.681718, 0.147621},    std::array<float, 3>{0.716567, 0.681718, -0.147621},
        std::array<float, 3>{0.525731, 0.850651, 0.000000},    std::array<float, 3>{0.425325, 0.688191, 0.587785},
        std::array<float, 3>{0.864188, 0.442863, 0.238856},    std::array<float, 3>{0.688191, 0.587785, 0.425325},
        std::array<float, 3>{0.809017, 0.309017, 0.500000},    std::array<float, 3>{0.681718, 0.147621, 0.716567},
        std::array<float, 3>{0.587785, 0.425325, 0.688191},    std::array<float, 3>{0.955423, 0.295242, 0.000000},
        std::array<float, 3>{1.000000, 0.000000, 0.000000},    std::array<float, 3>{0.951056, 0.162460, 0.262866},
        std::array<float, 3>{0.850651, -0.525731, 0.000000},   std::array<float, 3>{0.955423, -0.295242, 0.000000},
        std::array<float, 3>{0.864188, -0.442863, 0.238856},   std::array<float, 3>{0.951056, -0.162460, 0.262866},
        std::array<float, 3>{0.809017, -0.309017, 0.500000},   std::array<float, 3>{0.681718, -0.147621, 0.716567},
        std::array<float, 3>{0.850651, 0.000000, 0.525731},    std::array<float, 3>{0.864188, 0.442863, -0.238856},
        std::array<float, 3>{0.809017, 0.309017, -0.500000},   std::array<float, 3>{0.951056, 0.162460, -0.262866},
        std::array<float, 3>{0.525731, 0.000000, -0.850651},   std::array<float, 3>{0.681718, 0.147621, -0.716567},
        std::array<float, 3>{0.681718, -0.147621, -0.716567},  std::array<float, 3>{0.850651, 0.000000, -0.525731},
        std::array<float, 3>{0.809017, -0.309017, -0.500000},  std::array<float, 3>{0.864188, -0.442863, -0.238856},
        std::array<float, 3>{0.951056, -0.162460, -0.262866},  std::array<float, 3>{0.147621, 0.716567, -0.681718},
        std::array<float, 3>{0.309017, 0.500000, -0.809017},   std::array<float, 3>{0.425325, 0.688191, -0.587785},
        std::array<float, 3>{0.442863, 0.238856, -0.864188},   std::array<float, 3>{0.587785, 0.425325, -0.688191},
        std::array<float, 3>{0.688191, 0.587785, -0.425325},   std::array<float, 3>{-0.147621, 0.716567, -0.681718},
        std::array<float, 3>{-0.309017, 0.500000, -0.809017},  std::array<float, 3>{0.000000, 0.525731, -0.850651},
        std::array<float, 3>{-0.525731, 0.000000, -0.850651},  std::array<float, 3>{-0.442863, 0.238856, -0.864188},
        std::array<float, 3>{-0.295242, 0.000000, -0.955423},  std::array<float, 3>{-0.162460, 0.262866, -0.951056},
        std::array<float, 3>{0.000000, 0.000000, -1.000000},   std::array<float, 3>{0.295242, 0.000000, -0.955423},
        std::array<float, 3>{0.162460, 0.262866, -0.951056},   std::array<float, 3>{-0.442863, -0.238856, -0.864188},
        std::array<float, 3>{-0.309017, -0.500000, -0.809017}, std::array<float, 3>{-0.162460, -0.262866, -0.951056},
        std::array<float, 3>{0.000000, -0.850651, -0.525731},  std::array<float, 3>{-0.147621, -0.716567, -0.681718},
        std::array<float, 3>{0.147621, -0.716567, -0.681718},  std::array<float, 3>{0.000000, -0.525731, -0.850651},
        std::array<float, 3>{0.309017, -0.500000, -0.809017},  std::array<float, 3>{0.442863, -0.238856, -0.864188},
        std::array<float, 3>{0.162460, -0.262866, -0.951056},  std::array<float, 3>{0.238856, -0.864188, -0.442863},
        std::array<float, 3>{0.500000, -0.809017, -0.309017},  std::array<float, 3>{0.425325, -0.688191, -0.587785},
        std::array<float, 3>{0.716567, -0.681718, -0.147621},  std::array<float, 3>{0.688191, -0.587785, -0.425325},
        std::array<float, 3>{0.587785, -0.425325, -0.688191},  std::array<float, 3>{0.000000, -0.955423, -0.295242},
        std::array<float, 3>{0.000000, -1.000000, 0.000000},   std::array<float, 3>{0.262866, -0.951056, -0.162460},
        std::array<float, 3>{0.000000, -0.850651, 0.525731},   std::array<float, 3>{0.000000, -0.955423, 0.295242},
        std::array<float, 3>{0.238856, -0.864188, 0.442863},   std::array<float, 3>{0.262866, -0.951056, 0.162460},
        std::array<float, 3>{0.500000, -0.809017, 0.309017},   std::array<float, 3>{0.716567, -0.681718, 0.147621},
        std::array<float, 3>{0.525731, -0.850651, 0.000000},   std::array<float, 3>{-0.238856, -0.864188, -0.442863},
        std::array<float, 3>{-0.500000, -0.809017, -0.309017}, std::array<float, 3>{-0.262866, -0.951056, -0.162460},
        std::array<float, 3>{-0.850651, -0.525731, 0.000000},  std::array<float, 3>{-0.716567, -0.681718, -0.147621},
        std::array<float, 3>{-0.716567, -0.681718, 0.147621},  std::array<float, 3>{-0.525731, -0.850651, 0.000000},
        std::array<float, 3>{-0.500000, -0.809017, 0.309017},  std::array<float, 3>{-0.238856, -0.864188, 0.442863},
        std::array<float, 3>{-0.262866, -0.951056, 0.162460},  std::array<float, 3>{-0.864188, -0.442863, 0.238856},
        std::array<float, 3>{-0.809017, -0.309017, 0.500000},  std::array<float, 3>{-0.688191, -0.587785, 0.425325},
        std::array<float, 3>{-0.681718, -0.147621, 0.716567},  std::array<float, 3>{-0.442863, -0.238856, 0.864188},
        std::array<float, 3>{-0.587785, -0.425325, 0.688191},  std::array<float, 3>{-0.309017, -0.500000, 0.809017},
        std::array<float, 3>{-0.147621, -0.716567, 0.681718},  std::array<float, 3>{-0.425325, -0.688191, 0.587785},
        std::array<float, 3>{-0.162460, -0.262866, 0.951056},  std::array<float, 3>{0.442863, -0.238856, 0.864188},
        std::array<float, 3>{0.162460, -0.262866, 0.951056},   std::array<float, 3>{0.309017, -0.500000, 0.809017},
        std::array<float, 3>{0.147621, -0.716567, 0.681718},   std::array<float, 3>{0.000000, -0.525731, 0.850651},
        std::array<float, 3>{0.425325, -0.688191, 0.587785},   std::array<float, 3>{0.587785, -0.425325, 0.688191},
        std::array<float, 3>{0.688191, -0.587785, 0.425325},   std::array<float, 3>{-0.955423, 0.295242, 0.000000},
        std::array<float, 3>{-0.951056, 0.162460, 0.262866},   std::array<float, 3>{-1.000000, 0.000000, 0.000000},
        std::array<float, 3>{-0.850651, 0.000000, 0.525731},   std::array<float, 3>{-0.955423, -0.295242, 0.000000},
        std::array<float, 3>{-0.951056, -0.162460, 0.262866},  std::array<float, 3>{-0.864188, 0.442863, -0.238856},
        std::array<float, 3>{-0.951056, 0.162460, -0.262866},  std::array<float, 3>{-0.809017, 0.309017, -0.500000},
        std::array<float, 3>{-0.864188, -0.442863, -0.238856}, std::array<float, 3>{-0.951056, -0.162460, -0.262866},
        std::array<float, 3>{-0.809017, -0.309017, -0.500000}, std::array<float, 3>{-0.681718, 0.147621, -0.716567},
        std::array<float, 3>{-0.681718, -0.147621, -0.716567}, std::array<float, 3>{-0.850651, 0.000000, -0.525731},
        std::array<float, 3>{-0.688191, 0.587785, -0.425325},  std::array<float, 3>{-0.587785, 0.425325, -0.688191},
        std::array<float, 3>{-0.425325, 0.688191, -0.587785},  std::array<float, 3>{-0.425325, -0.688191, -0.587785},
        std::array<float, 3>{-0.587785, -0.425325, -0.688191}, std::array<float, 3>{-0.688191, -0.587785, -0.425325},
    };
}
