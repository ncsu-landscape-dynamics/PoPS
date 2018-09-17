#ifndef RASTER_H
#define RASTER_H

/*
 * SOD model - raster manipulation
 *
 * Copyright (C) 2015-2018 by the authors.
 *
 * Authors: Vaclav Petras <wenzeslaus gmail com>
 *          Completely rewritten by Vaclav Petras based on
 *          version by Zexi Chen <zchen22 ncsu edu>.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <iostream>
#include <ostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <initializer_list>
#include <stdlib.h>

#ifdef POPS_RASTER_WITH_GRASS_GIS

extern "C" {
#include <grass/gis.h>
#include <grass/glocale.h>
#include <grass/raster.h>
}

#endif // POPS_RASTER_WITH_GRASS_GIS

using std::string;
using std::cerr;
using std::endl;

/*! Iterate over two ranges and apply a binary function which modifies
 *  the first parameter.
 */
template<class InputIt1, class InputIt2, class BinaryOperation>
BinaryOperation for_each_zip(InputIt1 first1, InputIt1 last1, InputIt2 first2, BinaryOperation f) {
    for (; first1 != last1; ++first1, ++first2) {
        f(*first1, *first2);
    }
    return f;
}

/*! Representation of a raster image.
 *
 * The object support raster algebra operations:
 *
 * ```
 * Raster<int> a = {{1, 2}, {3, 4}};
 * auto b = 2 * (a + 1);
 * ```
 *
 * The raster algebra operations sometimes overlap with matrix
 * operations, e.g. for plus operator or multiplication by scalar.
 * However, in some cases, the behavior is different, e.g.,
 * multiplication of the rasters results in a new raster with cell
 * values which are result of multiplying cell values in the relevant
 * positions of the two raster.
 *
 * ```
 * Raster<int> a = {{1, 2}, {3, 4}};
 * auto b = 2 * (a + 1);
 * ```
 *
 * The template parameter Number is the numerical type of the raster,
 * typically int, float, or double.
 */
template<typename Number>
class Raster
{
private:
    unsigned width;
    unsigned height;
    // the west-east resolution of the pixel
    double w_e_res;
    // the north-south resolution of the pixel
    double n_s_res;
    Number *data;
public:
    Raster()
    {
        width = 0;
        height = 0;
        w_e_res = 0;
        n_s_res = 0;
        data = NULL;
    }

    Raster(const Raster& other)
    {
        width = other.width;
        height = other.height;
        w_e_res = other.w_e_res;
        n_s_res = other.n_s_res;
        data = new Number[width * height];
        std::copy(other.data, other.data + (width * height), data);
    }

    Raster(const Raster& other, Number value)
    {
        width = other.width;
        height = other.height;
        w_e_res = other.w_e_res;
        n_s_res = other.n_s_res;
        data = new Number[width * height]{value};
    }

    Raster(Raster&& other)
    {
        width = other.width;
        height = other.height;
        w_e_res = other.w_e_res;
        n_s_res = other.n_s_res;
        data = other.data;
        other.data = nullptr;
    }

    Raster(int width, int height, int w_e_res, int n_s_res)
    {
        this->width = width;
        this->height = height;
        this->w_e_res = w_e_res;
        this->n_s_res = n_s_res;
        this->data = new Number[width * height];
    }

    // TODO: res are doubles
    // TODO: size is unsigned?
    Raster(int width, int height, int w_e_res, int n_s_res, int value)
    {
        this->width = width;
        this->height = height;
        this->w_e_res = w_e_res;
        this->n_s_res = n_s_res;
        this->data = new Number[width * height]{value};
    }

    // TODO: here we ignore res, not really used anywhere
    // maybe remove from the class, or make it optional together with
    // a reference
    Raster(std::initializer_list<std::initializer_list<Number>> l)
        : Raster(l.begin()->size(), l.size(), 1, 1)
    {
         unsigned i = 0;
         unsigned j = 0;
         for (const auto& subl : l)
         {
            for (const auto& value : subl)
            {
               data[width * i + j] = value;
               ++j;
            }
            j = 0;
            ++i;
         }
    }

    ~Raster()
    {
        if (data) {
            delete[] data;
        }
    }

    int getWidth() const
    {
        return width;
    }

    int getHeight() const
    {
        return height;
    }

    int getWEResolution() const
    {
        return w_e_res;
    }

    int getNSResolution() const
    {
        return n_s_res;
    }

    void fill(Number value)
    {
        std::fill(data, data + (width * height), value);
    }

    void zero()
    {
        std::fill(data, data + (width * height), 0);
    }

    template<class UnaryOperation>
    void for_each(UnaryOperation op)
    {
        std::for_each(data, data + (width * height), op);
    }

    const Number& operator()(unsigned row, unsigned col) const
    {
        return data[row * width + col];
    }

    Number& operator()(unsigned row, unsigned col)
    {
        return data[row * width + col];
    }

    Raster& operator=(const Raster& other)
    {
        if (this != &other)
        {
            if (data)
                delete[] data;
            width = other.width;
            height = other.height;
            w_e_res = other.w_e_res;
            n_s_res = other.n_s_res;
            data = new Number[width * height];
            std::copy(other.data, other.data + (width * height), data);
        }
        return *this;
    }

    Raster& operator=(Raster&& other)
    {
        if (this != &other)
        {
            if (data)
                delete[] data;
            width = other.width;
            height = other.height;
            w_e_res = other.w_e_res;
            n_s_res = other.n_s_res;
            data = other.data;
            other.data = nullptr;
        }
        return *this;
    }

    Raster operator+(const Raster& image) const
    {
        if (this->width != image.getWidth() || this->height != image.getHeight()) {
            cerr << "The height or width of one image do not match with that of the other one!" << endl;
            return Raster();
        }
        else {
            auto re_width = this->width;
            auto re_height = this->height;
            auto out = Raster(re_width, re_height, this->w_e_res, this->n_s_res);

            for (int i = 0; i < re_height; i++) {
                for (int j = 0; j < re_width; j++) {
                    out.data[i * width + j] = this->data[i * width + j] + image.data[i * width + j];
                }
            }
            return out;
        }
    }

    Raster operator-(const Raster& image) const
    {
        if (this->width != image.getWidth() || this->height != image.getHeight()) {
            cerr << "The height or width of one image do not match with that of the other one!" << endl;
            return Raster();
        }
        else {
            auto re_width = this->width;
            auto re_height = this->height;
            auto out = Raster(re_width, re_height, this->w_e_res, this->n_s_res);

            for (int i = 0; i < re_height; i++) {
                for (int j = 0; j < re_width; j++) {
                    out.data[i * width + j] = this->data[i * width + j] - image.data[i * width + j];
                }
            }
            return out;
        }
    }

    Raster operator*(const Raster& image) const
    {
        if (width != image.getWidth() || height != image.getHeight()) {
            throw std::runtime_error("The height or width of one image do"
                                     " not match with that of the other one.");
        }
        auto out = Raster(width, height, w_e_res, n_s_res);

        std::transform(data, data + (width * height), image.data, out.data,
                       [](const Number& a, const Number& b) { return a * b; });
        return out;
    }

    Raster operator/(const Raster& image) const
    {
        if (width != image.getWidth() || height != image.getHeight()) {
            throw std::runtime_error("The height or width of one image do"
                                     " not match with that of the other one.");
        }
        auto out = Raster(width, height, w_e_res, n_s_res);

        std::transform(data, data + (width * height), image.data, out.data,
                       [](const Number& a, const Number& b) { return a / b; });
        return out;
    }

    Raster operator*(double value) const
    {
        auto out = Raster(width, height, w_e_res, n_s_res);

        std::transform(data, data + (width * height), out.data,
                       [&value](const Number& a) { return a * value; });
        return out;
    }

    Raster operator/(double value) const
    {
        auto out = Raster(width, height, w_e_res, n_s_res);

        std::transform(data, data + (width * height), out.data,
                       [&value](const Number& a) { return a / value; });
        return out;
    }

    Raster& operator+=(Number value)
    {
        std::for_each(data, data + (width * height),
                      [&value](Number& a) { a += value; });
        return *this;
    }

    Raster& operator-=(Number value)
    {
        std::for_each(data, data + (width * height),
                      [&value](Number& a) { a -= value; });
        return *this;
    }

    Raster& operator*=(double value)
    {
        std::for_each(data, data + (width * height),
                      [&value](Number& a) { a *= value; });
        return *this;
    }

    Raster& operator/=(double value)
    {
        std::for_each(data, data + (width * height),
                      [&value](Number& a) { a /= value; });
        return *this;
    }

    Raster& operator+=(const Raster& image)
    {
        for_each_zip(data, data + (width * height), image.data,
                     [](Number& a, Number& b) { a += b; });
        return *this;
    }

    Raster& operator-=(const Raster& image)
    {
        for_each_zip(data, data + (width * height), image.data,
                     [](Number& a, Number& b) { a -= b; });
        return *this;
    }

    Raster& operator*=(const Raster& image)
    {
        for_each_zip(data, data + (width * height), image.data,
                     [](Number& a, Number& b) { a *= b; });
        return *this;
    }

    Raster& operator/=(const Raster& image)
    {
        for_each_zip(data, data + (width * height), image.data,
                     [](Number& a, Number& b) { a /= b; });
        return *this;
    }

    bool operator==(const Raster& other) const
    {
        // TODO: assumes same sizes
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                if (this->data[i * width + j] != other.data[i * width + j])
                    return false;
            }
        }
        return true;
    }

    bool operator!=(const Raster& other) const
    {
        // TODO: assumes same sizes
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < width; j++) {
                if (this->data[i * width + j] != other.data[i * width + j])
                    return true;
            }
        }
        return false;
    }

    friend inline Raster operator*(double factor, const Raster& image)
    {
        return image * factor;
    }

    friend inline Raster pow(Raster image, double value) {
        image.for_each([value](Number& a){a = std::pow(a, value);});
        return image;
    }
    friend inline Raster sqrt(Raster image) {
        image.for_each([](Number& a){a = std::sqrt(a);});
        return image;
    }

    friend inline std::ostream& operator<<(std::ostream& stream, const Raster& image) {
        stream << "[[";
        for (unsigned i = 0; i < image.height; i++) {
            if (i != 0)
                stream << "],\n [";
            for (unsigned j = 0; j < image.width; j++) {
                if (j != 0)
                    stream << ", ";
                stream << image.data[i * image.width + j];
            }
        }
        stream << "]]\n";
        return stream;
    }

    #ifdef POPS_RASTER_WITH_GRASS_GIS

    /** Read a GRASS GIS raster map to the Raster.
     */
    static inline Raster fromGrassRaster(const char *name)
    {
        int fd = Rast_open_old(name, "");

        Raster img;

        img.width = Rast_window_cols();
        img.height = Rast_window_rows();

        Cell_head region;
        Rast_get_window(&region);
        img.w_e_res = region.ew_res;
        img.n_s_res = region.ns_res;

        img.data = new Number[img.height * img.width];

        for (int row = 0; row < img.height; row++) {
            Rast_get_d_row(fd, img.data + (row * img.width), row);
        }

        Rast_close(fd);
        return img;
    }

    /** Write the Raster to a GRASS GIS raster map.
     */
    void inline toGrassRaster(const char *name)
    {
        int fd = Rast_open_new(name, DCELL_TYPE);
        for (int i = 0; i < height; i++)
            Rast_put_d_row(fd, data + (i * width));
        Rast_close(fd);
    }

    #endif // POPS_RASTER_WITH_GRASS_GIS
};

#ifdef POPS_RASTER_WITH_GRASS_GIS

/** Read a GRASS GIS raster map to the Raster.
 *
 * This is a specialization for reading using int.
 */
template <>
inline Raster<int> Raster<int>::fromGrassRaster(const char *name)
{
    int fd = Rast_open_old(name, "");

    Raster img;

    img.width = Rast_window_cols();
    img.height = Rast_window_rows();

    Cell_head region;
    Rast_get_window(&region);
    img.w_e_res = region.ew_res;
    img.n_s_res = region.ns_res;

    img.data = new int[img.height * img.width];

    for (int row = 0; row < img.height; row++) {
        Rast_get_c_row(fd, img.data + (row * img.width), row);
    }

    Rast_close(fd);
    return img;
}

/** Write the Raster to a GRASS GIS raster map.
 *
 * This is a specialization for reading using int.
 */
template <>
inline void Raster<int>::toGrassRaster(const char *name)
{
    int fd = Rast_open_new(name, CELL_TYPE);
    for (int i = 0; i < height; i++)
        Rast_put_c_row(fd, data + (i * width));
    Rast_close(fd);
}

#endif // POPS_RASTER_WITH_GRASS_GIS

// convenient definitions, names for backwards compatibility
typedef Raster<int> Img;
typedef Raster<double> DImg;

#endif