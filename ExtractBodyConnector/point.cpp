#include "point.hpp"
#include "macros.hpp"

namespace op
{
    template<typename T>
    Point<T>::Point(const T x_, const T y_) :
        x{x_},
        y{y_}
    {
    }

    template<typename T>
    Point<T>::Point(const Point<T>& point)
    {
        try
        {
            x = point.x;
            y = point.y;
        }
        catch (const std::exception& e)
        {
            throw e.what();
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator=(const Point<T>& point)
    {
        try
        {
            x = point.x;
            y = point.y;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T>::Point(Point<T>&& point)
    {
        try
        {
            x = point.x;
            y = point.y;
        }
        catch (const std::exception& e)
        {
            throw e.what();
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator=(Point<T>&& point)
    {
        try
        {
            x = point.x;
            y = point.y;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    std::string Point<T>::toString() const
    {
        try
        {
            return '[' + std::to_string(x) + ", " + std::to_string(y) + ']';
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return "";
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator+=(const Point<T>& point)
    {
        try
        {
            x += point.x;
            y += point.y;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator+(const Point<T>& point) const
    {
        try
        {
            return Point<T>{T(x + point.x), T(y + point.y)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator+=(const T value)
    {
        try
        {
            x += value;
            y += value;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator+(const T value) const
    {
        try
        {
            return Point<T>{T(x + value), T(y + value)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator-=(const Point<T>& point)
    {
        try
        {
            x -= point.x;
            y -= point.y;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator-(const Point<T>& point) const
    {
        try
        {
            return Point<T>{T(x - point.x), T(y - point.y)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator-=(const T value)
    {
        try
        {
            x -= value;
            y -= value;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator-(const T value) const
    {
        try
        {
            return Point<T>{T(x - value), T(y - value)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator*=(const T value)
    {
        try
        {
            x *= value;
            y *= value;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator*(const T value) const
    {
        try
        {
            return Point<T>{T(x * value), T(y * value)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

    template<typename T>
    Point<T>& Point<T>::operator/=(const T value)
    {
        try
        {
            x /= value;
            y /= value;
            // Return
            return *this;
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return *this;
        }
    }

    template<typename T>
    Point<T> Point<T>::operator/(const T value) const
    {
        try
        {
            return Point<T>{T(x / value), T(y / value)};
        }
        catch (const std::exception& e)
        {
            throw e.what();
            return Point<T>{};
        }
    }

	COMPILE_TEMPLATE_BASIC_TYPES_STRUCT(Point);
}
