/* Copyright (C) 2010-2019, The Regents of The University of Michigan.
 All rights reserved.

 This software was developed as part of the The Vulcan project in the Intelligent Robotics Lab
 under the direction of Benjamin Kuipers, kuipers@umich.edu. Use of this code is governed by an
 MIT-style License that can be found at "https://github.com/h2ssh/Vulcan".
*/


/**
* \file     point.h
* \author   Collin Johnson
*
* Definition of a simple Point class.
*
* Includes, PointHash struct that provides a simple hash for points.
*/

#ifndef CORE_POINT_H
#define CORE_POINT_H

#include <core/float_comparison.h>
#include <type_traits>
#include <cmath>

namespace vulcan
{

/**
* Point<T> represents a simple 2D Cartesian point.
*
* NOTE: This struct name violates the naming conventions for the project. This issue is causing me some level of
*       philosophical duress as I try to rectify the gap in my naming conventions because point_t<T> seems very wrong.
*/
template<typename T>
struct Point
{
    T x;
    T y;

    /**
    * Default constructor for Point.
    */
    Point(void)
    : x(0)
    , y(0)
    {
    }

    /**
    * Constructor for Point.
    */
    explicit Point(T xPos, T yPos)
    : x(xPos)
    , y(yPos)
    {
        // Nothing to do here
    }

    /**
    * Copy constructor for a Point with a different type.
    */
    template <typename U>
    Point(const Point<U>& copy)
    : x(copy.x)
    , y(copy.y)
    {
    }

    template <class Archive>
    void serialize(Archive& ar)
    {
        ar( x,
            y );
    }
};


/**
* distance_between_points calculates the Cartesian distance between two points.
*/
template<typename T, typename U>
float distance_between_points(T pointAx, T pointAy, U pointBx, U pointBy)
{
    return std::sqrt((pointAx-pointBx)*(pointAx-pointBx) + (pointAy-pointBy)*(pointAy-pointBy));
}

/**
* distance_between_points calculates the Cartesian distance between two points.
*/
template<typename T, typename U>
float distance_between_points(const Point<T>& pointA, const Point<U>& pointB)
{
    return std::sqrt((pointA.x-pointB.x)*(pointA.x-pointB.x) + (pointA.y-pointB.y)*(pointA.y-pointB.y));
}

/**
* distance_to_point calculates the radial distance from the origin to the specified point.
*/
template<typename T>
float distance_to_point(const Point<T>& point)
{
    return std::sqrt(point.x*point.x + point.y*point.y);
}

template<typename T, typename U>
float squared_point_distance(const Point<T>& pointA, const Point<U>& pointB)
{
    return (pointA.x-pointB.x)*(pointA.x-pointB.x) + (pointA.y-pointB.y)*(pointA.y-pointB.y);
}

/**
* point_norm_squared calculates the squared L2 norm of a point (equivalent to the radial distance from the origin.)
*/
template<typename T>
T point_norm_squared(const Point<T>& point)
{
    return point.x*point.x + point.y*point.y;
}

/**
* point_norm calculates the L2 norm of a point (equivalent to the radial distance from the origin.)
*/
template<typename T>
T point_norm(const Point<T>& point)
{
    return std::sqrt(point_norm_squared(point));
}

/**
* inner_product_between_points calculates simple inner product between points of the same type.
*/
template<typename T>
T inner_product_between_points(const Point<T>& pointA, const Point<T>& pointB)
{
    return pointA.x*pointB.x + pointA.y*pointB.y;
}

/**
* angle_between_points finds the angle between two vectors defined by the three points provided as arguments.
*
* \param    first               Head of one of the vectors
* \param    second              Head of the other vector
* \param    center              Point shared by the two vectors
* \return   Angle between first and second in the range of [0, PI]. NAN if first == center or second == center.
*/
template <typename T>
float angle_between_points(const Point<T>& first, const Point<T>& second, const Point<T>& center)
{
    // Use the vector form here to get a pretty answer:
    //  cos(theta) = a dot b / ||a||*||b||

    if ((first == center) || (second == center))
    {
        return NAN;
    }

    T xA = first.x  - center.x;
    T yA = first.y  - center.y;
    T xB = second.x - center.x;
    T yB = second.y - center.y;

    double acosTerm = (xA*xB + yA*yB) / (std::sqrt(xA*xA + yA*yA) * std::sqrt(xB*xB + yB*yB));
    if (acosTerm < -1.0)
    {
        acosTerm = 1.0;
    }
    else if(acosTerm > 1.0)
    {
        acosTerm = 1.0;
    }

    return std::acos(acosTerm);
}


/**
* angle_to_point finds the angle of a vector from head to tail.
*
* \param    from                Start of the vector
* \param    to                  End of the vector
* \return   Angle of vector atan2(to - from).
*/
template <typename T, typename U>
float angle_to_point(const Point<T>& from, const Point<U>& to)
{
    return std::atan2(to.y-from.y, to.x-from.x);
}


/**
* rotate applies a rotation matrix to the point, rotating the point by the specified angle.
*
* \param    point       Point to be rotated
* \param    angle       Angle by which to rotate
* \return   Rotated point.
*/
template<typename T>
Point<T> rotate(const Point<T>& point, float angle)
{
    return Point<T>(point.x*std::cos(angle) - point.y*std::sin(angle),
                    point.x*std::sin(angle) + point.y*std::cos(angle));
}


/**
* transform applies a transform to the point. The transform first adds the x and y values, then applies
* a rotation of the specified angle.
*
* \param    point       Point to be transformed
* \param    x           x shift
* \param    y           y shift
* \param    angle       Angle to rotate
* \return   Transformed point.
*/
template <typename T, typename U>
Point<T> transform(const Point<T>& point, U x, U y, U angle)
{
    Point<T> transformed(point.x+x, point.y+y);

    return rotate(transformed, angle);
}


/**
* homogeneous_transform takes a point, applies a rotation to it, then translates it by the specified (x, y)
*
* \param    point       Point to be rotated and translated
* \param    x           x shift
* \param    y           y shift
* \param    angle       Angle to rotate
* \return   Point after rotation and translation.
*/
template <typename T, typename U>
Point<T> homogeneous_transform(const Point<T>& point, U x, U y, U angle)
{
    Point<T> rotated = rotate(point, angle);
    rotated.x += x;
    rotated.y += y;

    return rotated;
}


// Various useful operator overloads
template <typename T, typename U>
bool operator==(const Point<T>& lhs, const Point<U>& rhs)
{
    // If floating point, then use the fuzzy floating point comparison
    if(std::is_floating_point<T>::value)
    {
        return absolute_fuzzy_equal(lhs.x, rhs.x) && absolute_fuzzy_equal(lhs.y, rhs.y);
    }
    else
    {
        return (lhs.x == rhs.x) && (lhs.y == rhs.y);
    }
}

template <typename T, typename U>
bool operator!=(const Point<T>& lhs, const Point<U>& rhs)
{
    // Go from bottom left to top right
    return !(lhs == rhs);
}

template <typename T, typename U>
bool operator<(const Point<T>& lhs, const Point<U>& rhs)
{
    // Go from bottom left to top right
    return (lhs.x < rhs.x) || ((lhs.x == rhs.x) && (lhs.y < rhs.y));
}

template<class ostream, typename T>
ostream& operator<<(ostream& out, const Point<T>& point)
{
    out<<'('<<point.x<<','<<point.y<<')';
    return out;
}

template <typename T>
Point<T> operator-(const Point<T>& rhs)
{
    return Point<T>(-rhs.x,
                          -rhs.y);
}

template <typename T, typename U>
Point<T> operator-(const Point<T>& lhs, const Point<U>& rhs)
{
    return Point<T>(lhs.x - rhs.x,
                          lhs.y - rhs.y);
}

template <typename T, typename U>
Point<T> operator+(const Point<T>& lhs, const Point<U>& rhs)
{
    return Point<T>(lhs.x + rhs.x,
                          lhs.y + rhs.y);
}

template <typename T, typename U>
Point<T> operator*(U lhs, const Point<T>& rhs)
{
    return Point<T>(lhs * rhs.x,
                          lhs * rhs.y);
}

template <typename T, typename U>
Point<T> operator*(const Point<T>& lhs, U rhs)
{
    return Point<T>(lhs.x * rhs,
                          lhs.y * rhs);
}

template <typename T, typename U>
Point<T> operator/(const Point<T>& lhs, U rhs)
{
    return Point<T>(lhs.x / rhs,
                          lhs.y / rhs);
}

template <typename T, typename U>
Point<T>& operator+=(Point<T>& lhs, const Point<U>& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;

    return lhs;
}


template <typename T, typename U>
Point<T>& operator-=(Point<T>& lhs, const Point<U>& rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;

    return lhs;
}

} // namespace vulcan

#endif // CORE_POINT_H
