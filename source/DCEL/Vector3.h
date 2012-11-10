///////////////////////////////////////////////////////////////////////////////
//
//  Copyright Leonardo Fischer 2011 - http://coderender.blogspot.com
//
//  Distributed under the licence available in the accompanying file 
//  LICENCE.txt. Please read it before use this code.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef VECTOR_3_H
#define VECTOR_3_H

#include <cmath>

template <class T>
class Vector3
{
public:
    Vector3()
    {}

    Vector3(T x, T y, T z)
        :x(x)
        ,y(y)
        ,z(z)
    {}

    Vector3(const Vector3<T>& v)
        :x(v.x)
        ,y(v.y)
        ,z(v.z)
    {}

    Vector3(const Vector3<T>* v)
        :x(v->x)
        ,y(v->y)
        ,z(v->z)
    {}

    inline void set(T x, T y, T z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    inline void set(const Vector3<T>& other)
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
    }

    inline Vector3<T> operator=(const Vector3<T>& other) 
    {
        this->x = other.x;
        this->y = other.y;
        this->z = other.z;
        return *this;
    }

    /**
    	Explicitly, but easily, converts this Vector instance to one of the given type.

        Vector3<float> u(0,0,0);
        Vector3<double> v = u.cast<double>();
    */
    template <class U>
    Vector3<U> cast() const
    {
        return Vector3<U>( (U)x, (U)y, (U)z );
    }

    inline bool operator==(const Vector3<T>& other) const 
    {
        return this->x == other.x && this->y == other.y && this->z==other.z;
    }

    inline bool operator!=(const Vector3<T>& other) const 
    {
        return this->x!=other.x || this->y!=other.y || this->z!=other.z;
    }

    inline Vector3<T> operator+(const Vector3<T>& other) const 
    {
        return Vector3<T>(this->x + other.x, this->y + other.y, this->z + other.z);
    }

    inline void operator+=(const Vector3<T>& other)
    {
        this->x+=other.x;
        this->y+=other.y;
        this->z+=other.z;
    }

    inline Vector3<T> operator-() const 
    {
        return Vector3<T>(-this->x, -this->y, -this->z);
    }

    inline Vector3<T> operator-(const Vector3<T>& other) const 
    {
        return Vector3<T>(this->x - other.x, this->y - other.y, this->z - other.z);
    }

    inline void operator-=(const Vector3<T>& other)
    {
        this->x-=other.x;
        this->y-=other.y;
        this->z-=other.z;
    }

    inline Vector3<T> operator*(T s) const 
    {
        return Vector3<T>(this->x*s, this->y*s, this->z*s);
    }

    inline void operator*=(T s)
    {
        this->x*=s;
        this->y*=s;
        this->z*=s;
    }

    inline Vector3<T> operator/(T s) const 
    {
        return Vector3<T>(this->x/s, this->y/s, this->z/s);
    }

    inline void operator/=(T s)
    {
        this->x/=s;
        this->y/=s;
        this->z/=s;
    }

    inline Vector3<T> cross( const Vector3<T>& other ) const
    {
        return Vector3<T>( 
            this->y*other.z - this->z*other.y,
            this->z*other.x - this->x*other.z,
            this->x*other.y - this->y*other.x );
    }

    inline T dot( const Vector3<T>& other ) const
    {
        return this->x*other.x + this->y*other.y + this->z*other.z;
    }

    inline T length() const
    {
        return( sqrt( this->x*this->x + this->y*this->y + this->z*this->z ) );
    }

    /**
    	Returns the square of the length of the vector.
        
        Use this if you just want to compare legths of vectors, as this is faster to 
        compute than lenght()
    */
    inline T length2() const
    {
        return this->x*this->x + this->y*this->y + this->z*this->z;
    }

    /**
    	Resizes the vector.
    */
    inline void setLength(const T newLenght)
    {
        const T scale = ( newLenght / this->length() );
        this->x *= scale;
        this->y *= scale;
        this->z *= scale;
    }

    inline T distance(const Vector3<T>& other) const
    {
        const T xDelta = this->x-other.x;
        const T yDelta = this->y-other.y;
        const T zDelta = this->z-other.z;
        return sqrt( xDelta*xDelta + yDelta*yDelta + zDelta*zDelta);
    }

    /**
    	Returns the square of the distance to the other vector

        Use this if you just want to compare distances, as the square of 
        the distance is faster to compute than the distance itself.
    */
    inline T distance2(const Vector3<T>& other) const
    {
        const T xDelta = this->x-other.x;
        const T yDelta = this->y-other.y;
        const T zDelta = this->z-other.z;
        return xDelta*xDelta + yDelta*yDelta + zDelta*zDelta;
    }

    inline void normalize()
    {
        const T len = this->length();
        this->x /= len;
        this->y /= len;
        this->z /= len;
    }

    /**
    	Returns a normalized copy of the vector.

        Use if you need a normalized version of the vector AND
        the vector itself.
    */
    inline Vector3<T> normalizedCopy()
    {
        Vector3<T> v = this;
        const T len = v.length(); //inline normalize
        v.x /= len;
        v.y /= len;
        v.z /= len;
        return v;
    }

    inline void rotateAroundY(T radians)
    {
        const double cs = cos(radians);
        const double sn = sin(radians);
        set((T)(x*cs - z*sn), y, (T)(x*sn + z*cs));
    }

    /**
    This method will rotate the point represented by this vector
    arround a given axis.

    This is the most general form for the methods rotateAroundX,
    Y and Z, but is more expensive.

    \note the axis MUST be normalized

    \note that the axis passes through the point (0,0,0).

    Math from http://local.wasp.uwa.edu.au/~pbourke/geometry/rotate/
    */
    inline void rotateAround(const Vector3<T>& axis, T radians)
    {
        T costheta = cos(radians);
        T sintheta = sin(radians);

        Vector3<T> q;
        q.x = (costheta + (1 - costheta) * axis.x * axis.x) * this->x;
        q.x += ((1 - costheta) * axis.x * axis.y - axis.z * sintheta) * this->y;
        q.x += ((1 - costheta) * axis.x * axis.z + axis.y * sintheta) * this->z;

        q.y = ((1 - costheta) * axis.x * axis.y + axis.z * sintheta) * this->x;
        q.y += (costheta + (1 - costheta) * axis.y * axis.y) * this->y;
        q.y += ((1 - costheta) * axis.y * axis.z - axis.x * sintheta) * this->z;

        q.z = ((1 - costheta) * axis.x * axis.z - axis.y * sintheta) * this->x;
        q.z += ((1 - costheta) * axis.y * axis.z + axis.x * sintheta) * this->y;
        q.z += (costheta + (1 - costheta) * axis.z * axis.z) * this->z;

        this->set(q);
    }

    inline void rotateAround(const Vector3<T>& p1, const Vector3<T>& p2, T radians)
    {
        Vector3<T> r;
        r.x = p2.x - p1.x;
        r.y = p2.y - p1.y;
        r.z = p2.z - p1.z;
        this->x -= p1.x;
        this->y -= p1.y;
        this->z -= p1.z;
        r.normalize();

        T costheta, sintheta;
        costheta = cos(radians);
        sintheta = sin(radians);

        Vector3<T> q;
        q.x = (costheta + (1 - costheta) * r.x * r.x) * this->x;
        q.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * this->y;
        q.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * this->z;

        q.y = ((1 - costheta) * r.x * r.y + r.z * sintheta) * this->x;
        q.y += (costheta + (1 - costheta) * r.y * r.y) * this->y;
        q.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * this->z;

        q.z = ((1 - costheta) * r.x * r.z - r.y * sintheta) * this->x;
        q.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * this->y;
        q.z += (costheta + (1 - costheta) * r.z * r.z) * this->z;

        q.x += p1.x;
        q.y += p1.y;
        q.z += p1.z;

        this->set(q);
    }

    bool isNaN() const
    {
        return x!=x || y!=y || z!=z;
    }

    T x;
    T y;
    T z;
};

typedef Vector3<int> Vector3i;
typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;

#endif//VECTOR_3_H
