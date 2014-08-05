/* Copyright (c) 2014 Scott Kuhl. All rights reserved.
 * License: This code is licensed under a 3-clause BSD license. See
 * the file named "LICENSE" for a full copy of the license.
 */

/** @file
 * @author Scott Kuhl
 *
 * This file provides miscellaneous matrix and vector math operations
 * as well as helper functions for loading vertex/fragment shader,
 * textures via imageio.h (if KUHL_UTIL_USE_IMAGEMAGICK preprocessor
 * variable is defined) and 3D models (if KUHL_UTIL_USE_ASSIMP
 * preprocessor variable is defined).
 *
 * See imageio.c and imageio.h for more information about texture
 * loading capabilities.
 *
 * ASSIMP is published under 3-clause BSD license (as is this
 * file). Some of the code involving ASSIMP in this file is based on
 * code in an example program in the ASSIMP source tree. ASSIMP code
 * is Copyright (c) 2006-2012 assimp team. All rights reserved.
 * For more information about ASSIMP, see: http://assimp.sourceforge.net/
 */

#ifndef __KUHL_UTIL_H__
#define __KUHL_UTIL_H__

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
/** M_PI is an approximation of pi. This variable is often available
 * in C without defining it yourself, but it isn't guaranteed to exist
 * according to the C standard. For more information, see:
 * http://c-faq.com/fp/mpi.html */
#define M_PI 3.14159265358979323846
#endif



/** Call kuhl_errorcheck() with no parameters frequently for easy
 * OpenGL error checking. OpenGL doesn't report errors by
 * default. Instead, we must periodically check for errors
 * ourselves. When you call kuhl_errorcheck() with no parameters. The
 * C preprocessor will replace those calls with
 * kuhl_errorcheckFileLine() and pass the source code filename and the
 * line in the source code file as parameters to it.
 *
 * One alternative way to carefully check for errors is to set up a
 * OpenGL context with debugging enabled and then use
 * glDebugMessageCallback() to ask OpenGL to call a function that you
 * write every time an error occurs. We aren't using this seemingly
 * easier callback approach because it doesn't make it easy to narrow
 * down the line(s) of code causing an error.
 */
#define kuhl_errorcheck() kuhl_errorcheckFileLine(__FILE__, __LINE__)
// kuhl_errorcheck() calls this C function:
int kuhl_errorcheckFileLine(char *file, int line);

/* General tips for learning the vector and matrix functions provided by kuhl-util:

   Matrices are assumed to be column-major (like most OpenGL manuals do).

   This API implements both float and double values for OpenGL. For example, a function that operates on a "float vec[3];" will be named vec3f_something(). A 4x4 matrix would be declared as "float mat[16];" and the functions that operate on the matrix are named mat4f_something(). Matrices are implemented as 1D arrays (as OpenGL expects!).

   Many of the functions have the inline keyword to encourage the compiler to inline the code. Note that inline functions have the code in the header file and something that looks like a prototype line in the C file (the reverse of normal!)

   If a calculation returns a single value, the corresponding function in this file will return that value. If a calculation returns a matrix or a vector, the destination is the first argument. For example, to take the cross product of two vectors with 3 float elements (written mathematically: C = A cross B) you could use vec3f_cross_new(C, A, B);

   Some functions end with "_new" to make it clear that the first argument is not part of the calculation and is simply the place where the result of the calculation is stored. For example, mat4f_invert_new(destMatrix, sourceMatrix) will invert sourceMatrix and store it in destMatrix. However, mat4f_invert(matrix) will invert matrix in place.

   Some functions include the "Vec" in their names such as: mat4f_translate_new() and mat4f_translateVec_new(). Both of these functions do the same thing but the first version takes a list of numbers as a parameter and the second one takes an array of numbers.

*/


/** Set the values in a 3-component float vector */
static inline void   vec3f_set(float  v[3], float  a, float  b, float  c)
{ v[0]=a; v[1]=b; v[2]=c; }
/** Set the values in a 3-component double vector */
static inline void   vec3d_set(double v[3], double a, double b, double c)
{ v[0]=a; v[1]=b; v[2]=c; }
/** Set the values in a 4-component float vector */
static inline void   vec4f_set(float  v[4], float  a, float  b, float  c, float  d)
{ v[0]=a; v[1]=b; v[2]=c; v[3]=d; }
/** Set the values in a 4-component double vector */
static inline void   vec4d_set(double v[4], double a, double b, double c, double d)
{ v[0]=a; v[1]=b; v[2]=c; v[3]=d; }

/** Copy the contents of one float vector in to another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 * @param n The length of the array.
 */
static inline void vecNf_copy(float result[ ], const float a[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i];
}
/** Copy the contents of one double vector in to another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 * @param n The length of the array.
 */
static inline void vecNd_copy(double result[ ], const double a[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i];
}
/** Copy the contents 3-component float vector into another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 */
static inline void vec3f_copy(float result[3], const float a[3])
{ vecNf_copy(result, a, 3); }
/** Copy the contents 3-component double vector into another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 */
static inline void vec3d_copy(double result[3], const double a[3])
{ vecNd_copy(result, a, 3); }
/** Copy the contents 4-component float vector into another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 */
static inline void vec4f_copy(float result[4], const float a[4])
{ vecNf_copy(result, a, 4); }
/** Copy the contents 4-component double vector into another vector.
 * @param result An array to copy values into.
 * @param a An array to copy values out of.
 */
static inline void vec4d_copy(double result[4], const double a[4])
{ vecNd_copy(result, a, 4); }


/** Cross product of two 3-component float vectors.
 * @param result An array to store the resulting calculations. It is OK if result points to the same location as A or B.
 * @param A The vector for the left side of the cross product.
 * @param B The vector for the right side of the cross product.
 */
static inline void vec3f_cross_new(float result[3], const float A[3], const float B[3])
{
	float temp[3];
	temp[0] = A[1]*B[2] - A[2]*B[1];
	temp[1] = A[2]*B[0] - A[0]*B[2];
	temp[2] = A[0]*B[1] - A[1]*B[0];
	vec3f_copy(result, temp);
}
/** Cross product of two 3-component double vectors.
 * @param result An array to store the resulting calculations. It is OK if result points to the same location as A or B.
 * @param A The vector for the left side of the cross product.
 * @param B The vector for the right side of the cross product.
 */
static inline void vec3d_cross_new(double result[3], const double A[3], const double B[3])
{
	double temp[3];
	temp[0] = A[1]*B[2] - A[2]*B[1];
	temp[1] = A[2]*B[0] - A[0]*B[2];
	temp[2] = A[0]*B[1] - A[1]*B[0];
	vec3d_copy(result, temp);
}

/** Calculates the dot product of two float vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @param n The number of components in the vectors.
 * @return The dot product of the vectors.
 */
static inline float vecNf_dot(const float A[ ], const float B[ ], const int n)
{
	float sum = 0.0f;
	for(int i=0; i<n; i++)
		sum += A[i]*B[i];
	return sum;
}
/** Calculates the dot product of two double vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @param n The number of components in the vectors.
 * @return The dot product of the vectors.
 */
static inline double vecNd_dot(const double A[ ], const double B[ ], const int n)
{
	double sum = 0.0;
	for(int i=0; i<n; i++)
		sum += A[i]*B[i];
	return sum;
}
/** Calculates the dot product of two 3-component float vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @return The dot product of the vectors.
 */
static inline float vec3f_dot(const float A[3], const float B[3])
{ return A[0]*B[0] + A[1]*B[1] + A[2]*B[2]; }
/** Calculates the dot product of two 3-component double vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @return The dot product of the vectors.
 */
static inline double vec3d_dot(const double A[3], const double B[3])
{ return A[0]*B[0] + A[1]*B[1] + A[2]*B[2]; }
/** Calculates the dot product of two 4-component float vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @return The dot product of the vectors.
 */
static inline float vec4f_dot(const float A[4], const float B[4])
{ return A[0]*B[0] + A[1]*B[1] + A[2]*B[2] + A[3]*B[3]; }
/** Calculates the dot product of two 4-component double vectors.
 *
 * @param A One of the vectors to take the dot product of.
 * @param B The other vector to take the dot product of.
 * @return The dot product of the vectors.
 */
static inline double vec4d_dot(const double A[4], const double B[4])
{ return A[0]*B[0] + A[1]*B[1] + A[2]*B[2] + A[3]*B[3]; }

/** Calculate the norm squared or length squared of a 3-component float vector.
 * @param A The vector to calculate the length of.
 * @return The length*length of the vector.
 */
static inline float vec3f_normSq(const float A[3])
{ return vec3f_dot(A,A); }
/** Calculate the norm squared or length squared of a 3-component double vector.
 * @param A The vector to calculate the length of.
 * @return The length*length of the vector.
 */
static inline double vec3d_normSq(const double A[3])
{ return vec3d_dot(A,A); }
/** Calculate the norm squared or length squared of a 4-component float vector.
 * @param A The vector to calculate the length of.
 * @return The length*length of the vector.
 */
static inline float vec4f_normSq(const float A[4])
{ return vec4f_dot(A,A); }
/** Calculate the norm squared or length squared of a 4-component double vector.
 * @param A The vector to calculate the length of.
 * @return The length*length of the vector.
 */
static inline double vec4d_normSq(const double A[4])
{ return vec4d_dot(A,A); }

/** Calculate the norm or length of a 3-component float vector.
 * @param A The vector to calculate the length of.
 * @return The length of the vector.
 */
static inline float vec3f_norm(const float A[3])
{ return sqrtf(vec3f_dot(A,A)); }

/** Calculate the norm or length of a 3-component double vector.
 * @param A The vector to calculate the length of.
 * @return The length of the vector.
 */
static inline double vec3d_norm(const double A[3])
{ return sqrt(vec3d_dot(A,A)); }
/** Calculate the norm or length of a 4-component float vector.
 * @param A The vector to calculate the length of.
 * @return The length of the vector.
 */
static inline float vec4f_norm(const float A[4])
{ return sqrtf(vec4f_dot(A,A)); }
/** Calculate the norm or length of a 4-component double vector.
 * @param A The vector to calculate the length of.
 * @return The length of the vector.
 */
static inline double vec4d_norm(const double A[4])
{ return sqrt(vec4d_dot(A,A)); }

/** Divide each element in a float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNf_scalarDiv_new(float result[], const float v[], const float scalar, const int n)
{
	for(int i=0; i<n; i++)
		result[i] = v[i]/scalar;
}
/** Divide each element in a double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNd_scalarDiv_new(double result[], const double v[], const double scalar, const int n)
{
	for(int i=0; i<n; i++)
		result[i] = v[i]/scalar;
}
/** Divide each element in a 3-component float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec3f_scalarDiv_new(float result[3], const float v[3], const float scalar)
{ vecNf_scalarDiv_new(result, v, scalar, 3); }
/** Divide each element in a 3-component double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec3d_scalarDiv_new(double result[3], const double v[3], const double scalar)
{ vecNd_scalarDiv_new(result, v, scalar, 3); }
/** Divide each element in a 4-component float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec4f_scalarDiv_new(float result[4], const float v[4], const float scalar)
{ vecNf_scalarDiv_new(result, v, scalar, 4); }
/** Divide each element in a 4-component double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec4d_scalarDiv_new(double result[4], const double v[4], const double scalar)
{ vecNd_scalarDiv_new(result, v, scalar, 4); }

/** Divide each element in a float vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 * @param n Number of components in the vector.
 */
static inline void vecNf_scalarDiv(float  v[ ], const float  scalar, const int n)
{ vecNf_scalarDiv_new(v, v, scalar, n); }
/** Divide each element in a double vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 * @param n Number of components in the vector.
 */
static inline void vecNd_scalarDiv(double v[ ], const double scalar, const int n)
{ vecNd_scalarDiv_new(v, v, scalar, n); }
/** Divide each element in a 3-component float vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 */
static inline void vec3f_scalarDiv(float  v[3], const float  scalar)
{ vec3f_scalarDiv_new(v, v, scalar); }
/** Divide each element in a 3-component double vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 */
static inline void vec3d_scalarDiv(double v[3], const double scalar)
{ vec3d_scalarDiv_new(v, v, scalar); }
/** Divide each element in a 4-component float vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 */
static inline void vec4f_scalarDiv(float  v[4], const float  scalar)
{ vec4f_scalarDiv_new(v, v, scalar); }
/** Divide each element in a 4-component double vector by a scalar in place.
 * @param v Input vector to be replaced with the new result.
 * @param scalar Value to divide by.
 */
static inline void vec4d_scalarDiv(double v[4], const double scalar)
{ vec4d_scalarDiv_new(v, v, scalar); }

/** Multiply each element in a float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNf_scalarMult_new(float result[], const float v[], const float scalar, const int n)
{
	for(int i=0; i<n; i++)
		result[i] = v[i]*scalar;
}
/** Multiply each element in a double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNd_scalarMult_new(double result[], const double v[], const double scalar, const int n)
{
	for(int i=0; i<n; i++)
		result[i] = v[i]*scalar;
}
/** Multiply each element in a 3-component float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec3f_scalarMult_new(float result[3], const float v[3], const float scalar)
{ vecNf_scalarMult_new(result, v, scalar, 3); }
/** Multiply each element in a 3-component double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec3d_scalarMult_new(double result[3], const double v[3], const double scalar)
{ vecNd_scalarMult_new(result, v, scalar, 3); }
/** Multiply each element in a 4-component float vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec4f_scalarMult_new(float result[4], const float v[4], const float scalar)
{ vecNf_scalarMult_new(result, v, scalar, 4); }
/** Multiply each element in a 4-component double vector by a scalar.
 * @param result Resulting vector
 * @param v Input vector
 * @param scalar Value to divide by.
 */
static inline void vec4d_scalarMult_new(double result[4], const double v[4], const double scalar)
{ vecNd_scalarMult_new(result, v, scalar, 4); }

/** Multiply each element in a float vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNf_scalarMult(float  v[ ], const float  scalar, const int n)
{ vecNf_scalarMult_new(v, v, scalar, n); }
/** Multiply each element in a double vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 * @param n Number of components in vector. */
static inline void vecNd_scalarMult(double v[ ], const double scalar, const int n)
{ vecNd_scalarMult_new(v, v, scalar, n); }
/** Multiply each element in a 3-component float vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 */
static inline void vec3f_scalarMult(float  v[3], const float  scalar)
{ vec3f_scalarMult_new(v, v, scalar); }
/** Multiply each element in a 3-component double vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 */
static inline void vec3d_scalarMult(double v[3], const double scalar)
{ vec3d_scalarMult_new(v, v, scalar); }
/** Multiply each element in a 4-component float vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 */
static inline void vec4f_scalarMult(float  v[4], const float  scalar)
{ vec4f_scalarMult_new(v, v, scalar); }
/** Multiply each element in a 4-component double vector by a scalar in place.
 * @param v Input vector to be replaced by the new values.
 * @param scalar Value to divide by.
 */
static inline void vec4d_scalarMult(double v[4], const double scalar)
{ vec4d_scalarMult_new(v, v, scalar); }

/** Normalize a 3-component float vector and store result at a new location.
 * @param dest The new normalized vector.
 * @param src The input vector.
 */
static inline void vec3f_normalize_new(float  dest[3], const float  src[3])
{ float  len = vec3f_norm(src); vec3f_scalarDiv_new(dest, src, len); }
/** Normalize a 3-component double vector and store result at a new location.
 * @param dest The new normalized vector.
 * @param src The input vector.
 */
static inline void vec3d_normalize_new(double dest[3], const double src[3])
{ double len = vec3d_norm(src); vec3d_scalarDiv_new(dest, src, len); }
/** Normalize a 4-component float vector and store result at a new location.
 * @param dest The new normalized vector.
 * @param src The input vector.
 */
static inline void vec4f_normalize_new(float  dest[4], const float  src[4])
{ float  len = vec4f_norm(src); vec4f_scalarDiv_new(dest, src, len); }
/** Normalize a 4-component double vector and store result at a new location.
 * @param dest The new normalized vector.
 * @param src The input vector.
 */
static inline void vec4d_normalize_new(double dest[4], const double src[4])
{ double len = vec4d_norm(src); vec4d_scalarDiv_new(dest, src, len); }

/** Normalize a 3-component float vector in place.
 * @param v The vector that will be normalized in place. */
static inline void vec3f_normalize(float v[3])
{ float  len = vec3f_norm(v); vec3f_scalarDiv(v, len); }
/** Normalize a 3-component double vector in place.
 * @param v The vector that will be normalized in place. */
static inline void vec3d_normalize(double v[3])
{ double len = vec3d_norm(v); vec3d_scalarDiv(v, len); }
/** Normalize a 4-component float vector in place.
 * @param v The vector that will be normalized in place. */
static inline void vec4f_normalize(float v[4])
{ float  len = vec4f_norm(v); vec4f_scalarDiv(v, len); }
/** Normalize a 4-component double vector in place.
 * @param v The vector that will be normalized in place. */
static inline void vec4d_normalize(double v[4])
{ double len = vec4d_norm(v); vec4d_scalarDiv(v, len); }

/** Homogenize a 4-element float vector and store the result in a new
 * location. Homogenization divides all elements in the array by the
 * 4th component.
 * @param dest The new homogenized vector.
 * @param src The vector to be homogenized.
 */
static inline void vec4f_homogenize_new(float dest[4], const float  src[4])
{ vec4f_scalarDiv_new(dest, src, src[3]); }
/** Homogenize a 4-element double vector and store the result in a new
 * location. Homogenization divides all elements in the array by the
 * 4th component.
 * @param dest The new homogenized vector.
 * @param src The vector to be homogenized.
 */
static inline void vec4d_homogenize_new(double dest[4], const double src[4])
{ vec4d_scalarDiv_new(dest, src, src[3]); }

/** Homogenize a 4-element float vector in place. Homogenization
 * divides all elements in the array by the 4th component.
 * @param v The new homogenized vector to be replaced with a new
 * homogenized vector.
 */
static inline void vec4f_homogenize(float  v[4])
{ vec4f_scalarDiv(v, v[3]); }
/** Homogenize a 4-element double vector in place. Homogenization
 * divides all elements in the array by the 4th component.
 * @param v The new homogenized vector to be replaced with a new
 * homogenized vector.
 */
static inline void vec4d_homogenize(double v[4])
{ vec4d_scalarDiv(v, v[3]); }

/** Add two float vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 * @param n The number of components in the vectors
 */
static inline void vecNf_add_new(float result[ ], const float a[ ], const float b[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i]+b[i];
}
/** Add two double vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 * @param n The number of components in the vectors
 */
static inline void vecNd_add_new(double result[ ], const double a[ ], const double b[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i]+b[i];
}

/** Add two 3-component float vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 */
static inline void vec3f_add_new(float result[3], const float a[3], const float b[3])
{ vecNf_add_new(result, a, b, 3); }
/** Add two 3-component double vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 */
static inline void vec3d_add_new(double result[3], const double a[3], const double b[3])
{ vecNd_add_new(result, a, b, 3); }
/** Add two 4-component float vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 */
static inline void vec4f_add_new(float result[4], const float a[4], const float b[4])
{ vecNf_add_new(result, a, b, 4); }
/** Add two 4-component double vectors together and store the result in a new location.
 * @param result The sum of the two input vectors.
 * @param a One of the input vectors.
 * @param b The other input vector.
 */
static inline void vec4d_add_new(double result[4], const double a[4], const double b[4])
{ vecNd_add_new(result, a, b, 4); }

/** Add two float vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 * @param n The number of components in the vectors
 */
static inline void vecNf_add(float  a[ ], const float  b[ ], const int n)
{ vecNf_add_new(a, a, b, n); }
/** Add two double vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 * @param n The number of components in the vectors
 */
static inline void vecNd_add(double a[ ], const double b[ ], const int n)
{ vecNd_add_new(a, a, b, n); }
/** Add two 3-component float vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 */
static inline void vec3f_add(float  a[3], const float  b[3])
{ vecNf_add_new(a, a, b, 3); }
/** Add two 3-component double vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 */
static inline void vec3d_add(double a[3], const double b[3])
{ vecNd_add_new(a, a, b, 3); }
/** Add two 4-component float vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 */
static inline void vec4f_add(float  a[4], const float  b[4])
{ vecNf_add_new(a, a, b, 4); }
/** Add two 4-component double vectors together and store the result in the first input vector.
 * @param a One of the input vectors (to be replaced with the new sum).
 * @param b The other input vector.
 */
static inline void vec4d_add(double a[4], const double b[4])
{ vecNd_add_new(a, a, b, 4); }


static inline void vecNf_sub_new(float result[ ], const float a[ ], const float b[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i]-b[i];
}
static inline void vecNd_sub_new(double result[ ], const double a[ ], const double b[ ], const int n)
{
	for(int i=0; i<n; i++)
		result[i] = a[i]-b[i];
}
static inline void vec3f_sub_new(float result[3], const float a[3], const float b[3])
{ vecNf_sub_new(result, a, b, 3); }
static inline void vec3d_sub_new(double result[3], const double a[3], const double b[3])
{ vecNd_sub_new(result, a, b, 3); }
static inline void vec4f_sub_new(float result[4], const float a[4], const float b[4])
{ vecNf_sub_new(result, a, b, 4); }
static inline void vec4d_sub_new(double result[4], const double a[4], const double b[4])
{ vecNd_sub_new(result, a, b, 4); }


static inline void vecNf_print(const float v[ ], const int n)
{
	// use temporary variable since arguments to printf are not const.
	float tmp[n]; 
	vecNf_copy(tmp, v, n);
	printf("vec%df(",n);
	for(int i=0; i<n; i++)
		printf("%10.3f ",tmp[i]);
	printf(")\n");
}
static inline void vecNd_print(const double v[ ], const int n)
{
	// use temporary variable since arguments to printf are not const.
	double tmp[n];
	vecNd_copy(tmp, v, n);
	printf("vec%dd(",n);
	for(int i=0; i<n; i++)
		printf("%10.3f ",tmp[i]);
	printf(")\n");
}

static inline void vec3f_print(const float v[3])
{ vecNf_print(v, 3); }
static inline void vec3d_print(const double v[3])
{ vecNd_print(v, 3); }
static inline void vec4f_print(const float v[4])
{ vecNf_print(v, 4); }
static inline void vec4d_print(const double v[4])
{ vecNd_print(v, 4); }

static inline int matN_getIndex(const int row, const int col, const int n)
{ return row+col*n; }
static inline int mat3_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 3); }
static inline int mat4_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 4); }
static inline int mat3f_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 3); }
static inline int mat4f_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 4); }
static inline int mat3d_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 3); }
static inline int mat4d_getIndex(const int row, const int col)
{ return matN_getIndex(row, col, 4); }

/* Get a column from a matrix. The first column in the matrix is column 0. */
static inline void matNf_getColumn(float  result[ ], const float  m[  ], const int col, const int n)
{ for(int i=0; i<n; i++) result[i] = m[matN_getIndex(i, col, n)]; }
static inline void matNd_getColumn(double result[ ], const double m[  ], const int col, const int n)
{ for(int i=0; i<n; i++) result[i] = m[matN_getIndex(i, col, n)]; }
static inline void mat4f_getColumn(float  result[4], const float  m[16], const int col)
{ matNf_getColumn(result, m, col, 4); }
static inline void mat4d_getColumn(double result[4], const double m[16], const int col)
{ matNd_getColumn(result, m, col, 4); }
static inline void mat3f_getColumn(float  result[3], const float  m[ 9], const int col)
{ matNf_getColumn(result, m, col, 3); }
static inline void mat3d_getColumn(double result[3], const double m[ 9], const int col)
{ matNd_getColumn(result, m, col, 3); }

/* Get a row from a matrix. The first row in the matrix is row 0. */
static inline void matNf_getRow(float  result[ ], const float  m[  ], const int row, const int n)
{ for(int i=0; i<n; i++) result[i] = m[matN_getIndex(row, i, n)]; }
static inline void matNd_getRow(double result[ ], const double m[  ], const int row, const int n)
{ for(int i=0; i<n; i++) result[i] = m[matN_getIndex(row, i, n)]; }
static inline void mat4f_getRow(float  result[4], const float  m[16], const int row)
{ matNf_getRow(result, m, row, 4); }
static inline void mat4d_getRow(double result[4], const double m[16], const int row)
{ matNd_getRow(result, m, row, 4); }
static inline void mat3f_getRow(float  result[3], const float  m[ 9], const int row)
{ matNf_getRow(result, m, row, 3); }
static inline void mat3d_getRow(double result[3], const double m[ 9], const int row)
{ matNd_getRow(result, m, row, 3); }

/* Set the specific column or row in matrix to the values stored in
 * vector v. The first row/column is 0. The size of the matrix must
 * match the size of the vector for these to work correctly (i.e., if
 * you want to set a row or column of a 4x4 matrix, you must use
 * mat4[fd]_set[Row|Column]() and pass in a vector with 4 elements. */
static inline void matNf_setColumn(float  matrix[  ], const float  v[ ], const int col, const int n)
{ for(int row=0; row<n; row++) matrix[matN_getIndex(row, col, n)] = v[row]; }
static inline void matNd_setColumn(double matrix[  ], const double v[ ], const int col, const int n)
{ for(int row=0; row<n; row++) matrix[matN_getIndex(row, col, n)] = v[row]; }
static inline void mat3f_setColumn(float  matrix[ 9], const float  v[3], const int col)
{ matNf_setColumn(matrix, v, col, 3); }
static inline void mat3d_setColumn(double matrix[ 9], const double v[3], const int col)
{ matNd_setColumn(matrix, v, col, 3); }
static inline void mat4f_setColumn(float  matrix[16], const float  v[4], const int col)
{ matNf_setColumn(matrix, v, col, 4); }
static inline void mat4d_setColumn(double matrix[16], const double v[4], const int col)
{ matNd_setColumn(matrix, v, col, 4); }
static inline void matNf_setRow(float  matrix[  ], const float  v[ ], const int row, const int n)
{ for(int col=0; col<n; col++) matrix[matN_getIndex(row, col, n)] = v[row]; }
static inline void matNd_setRow(double matrix[  ], const double v[ ], const int row, const int n)
{ for(int col=0; col<n; col++) matrix[matN_getIndex(row, col, n)] = v[row]; }
static inline void mat3f_setRow(float  matrix[ 9], const float  v[3], const int row)
{ matNf_setRow(matrix, v, row, 3); }
static inline void mat3d_setRow(double matrix[ 9], const double v[3], const int row)
{ matNd_setRow(matrix, v, row, 3); }
static inline void mat4f_setRow(float  matrix[16], const float  v[4], const int row)
{ matNf_setRow(matrix, v, row, 4); }
static inline void mat4d_setRow(double matrix[16], const double v[4], const int row)
{ matNd_setRow(matrix, v, row, 4); }




/* Copies a NxN matrix */
static inline void matNf_copy(float dest[], const float src[], const int n)
{ memcpy(dest, src, n*n*sizeof(float)); }
static inline void matNd_copy(double dest[], const double src[], const int n)
{ memcpy(dest, src, n*n*sizeof(double)); }
static inline void mat3f_copy(float dest[9], const float src[9])
{ matNf_copy(dest, src, 3); }
static inline void mat3d_copy(double dest[9], const double src[9])
{ matNd_copy(dest, src, 3); }
static inline void mat4f_copy(float dest[16], const float src[16])
{ matNf_copy(dest, src, 4); }
static inline void mat4d_copy(double dest[16], const double src[16])
{ matNd_copy(dest, src, 4); }

/* Multiplies two matrices together. result = matrixA * matrixB  */
static inline void matNf_mult_matNf_new(float  result[  ], const float  matA[  ], const float  matB[  ], const int n)
{
	/* Use a temporary matrix so callers can do:
	   matrixA = matrixA * matrixB */
	float tempMatrix[n*n];
	float vecA[n], vecB[n];
	for(int i=0; i<n; i++)
	{
		/* Get the ith row from matrix a */
		matNf_getRow(vecA, matA, i, n);
		for(int j=0; j<n; j++)
		{
			/* Get the jth column from matrix b */
			matNf_getColumn(vecB, matB, j, n);
			/* Dot product */
			tempMatrix[matN_getIndex(i,j,n)] = vecNf_dot(vecA, vecB, n);
		}
	}
	matNf_copy(result, tempMatrix, n);
}
static inline void matNd_mult_matNd_new(double result[  ], const double matA[ ], const double matB[], const int n)
{
	/* Use a temporary matrix so callers can do:
	   matrixA = matrixA * matrixB */
	double tempMatrix[n*n];
	double vecA[n], vecB[n];
	for(int i=0; i<n; i++)
	{
		/* Get the ith row from matrix a */
		matNd_getRow(vecA, matA, i, n);
		for(int j=0; j<n; j++)
		{
			/* Get the jth column from matrix b */
			matNd_getColumn(vecB, matB, j, n);
			/* Dot product */
			tempMatrix[matN_getIndex(i,j,n)] = vecNd_dot(vecA, vecB, n);
		}
	}
	matNd_copy(result, tempMatrix, n);
}
static inline void mat3f_mult_mat3f_new(float  result[3], const float  matA[ 9], const float  matB[9])
{ matNf_mult_matNf_new(result, matA, matB, 3); }
static inline void mat3d_mult_mat3d_new(double result[3], const double matA[ 9], const double matB[9])
{ matNd_mult_matNd_new(result, matA, matB, 3); }
static inline void mat4f_mult_mat4f_new(float  result[4], const float  matA[16], const float  matB[16])
{ matNf_mult_matNf_new(result, matA, matB, 4); }
static inline void mat4d_mult_mat4d_new(double result[4], const double matA[16], const double matB[16])
{ matNd_mult_matNd_new(result, matA, matB, 4); }



/* multiply a column vector by a matrix: result=m*v (n=dimension,
   typically 3 or 4) Works even if result and v point to the same memory location */
static inline void matNf_mult_vecNf_new(float result[], const float m[], const float v[], const int n)
{
	float tmp[n];
	for(int i=0; i<n; i++)
	{
		tmp[i] = 0;
		for(int j=0; j<n; j++)
			tmp[i] += m[matN_getIndex(i,j,n)] * v[j];
	}
	for(int i=0; i<n; i++)
		result[i] = tmp[i];
}
static inline void matNd_mult_vecNd_new(double result[], const double m[], const double v[], const int n)
{
	double tmp[n];
	for(int i=0; i<n; i++)
	{
		tmp[i] = 0;
		for(int j=0; j<n; j++)
			tmp[i] += m[matN_getIndex(i,j,n)] * v[j];
	}
	for(int i=0; i<n; i++)
		result[i] = tmp[i];
}
static inline void mat3f_mult_vec3f_new(float result[3], const float m[9], const float v[3])
{ matNf_mult_vecNf_new(result, m, v, 3); }
static inline void mat3d_mult_vec3d_new(double result[3], const double m[9], const double v[3])
{ matNd_mult_vecNd_new(result, m, v, 3); }
static inline void mat4f_mult_vec4f_new(float result[4], const float m[16], const float v[4])
{ matNf_mult_vecNf_new(result, m, v, 4); }
static inline void mat4d_mult_vec4d_new(double result[4], const double m[16], const double v[4])
{ matNd_mult_vecNd_new(result, m, v, 4); }

/* in-place vector = matrix*vector multiplication */
static inline void matNf_mult_vecNf(float vector[], const float matrix[], const int n)
{ matNf_mult_vecNf_new(vector, matrix, vector, n); }
static inline void matNd_mult_vecNd(double vector[], const double matrix[], const int n)
{ matNd_mult_vecNd_new(vector, matrix, vector, n); }
static inline void mat3f_mult_vec3f(float vector[3], const float matrix[9])
{ mat3f_mult_vec3f_new(vector, matrix, vector); }
static inline void mat3d_mult_vec3d(double vector[3], const double matrix[9])
{ mat3d_mult_vec3d_new(vector, matrix, vector); }
static inline void mat4f_mult_vec4f(float vector[4], const float matrix[16])
{ mat4f_mult_vec4f_new(vector, matrix, vector); }
static inline void mat4d_mult_vec4d(double vector[4], const double matrix[16])
{ mat4d_mult_vec4d_new(vector, matrix, vector); }


/* Transpose an n*n matrix in place */
static inline void matNf_transpose(float m[], const int n)
{
	float tmp;
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<i; j++)
		{
			int index1 = matN_getIndex(i,j,n);
			int index2 = matN_getIndex(j,i,n);
			tmp = m[index1];
			m[index1] = m[index2];
			m[index2] = tmp;
		}
	}
}
static inline void matNd_transpose(double m[], const int n)
{
	double tmp;
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<i; j++)
		{
			int index1 = matN_getIndex(i,j,n);
			int index2 = matN_getIndex(j,i,n);
			tmp = m[index1];
			m[index1] = m[index2];
			m[index2] = tmp;
		}
	}
}
static inline void mat3f_transpose(float m[9])
{ matNf_transpose(m, 3); }
static inline void mat3d_transpose(double m[9])
{ matNd_transpose(m, 3); }
static inline void mat4f_transpose(float m[16])
{ matNf_transpose(m, 4); }
static inline void mat4d_transpose(double m[16])
{ matNd_transpose(m, 4); }

/* Transpose a matrix and store the result at a new location. */
static inline void matNf_transpose_new(float  dest[  ], const float  src[  ], const int n)
{ matNf_copy(dest, src, n); matNf_transpose(dest, n); }
static inline void matNd_transpose_new(double dest[  ], const double src[  ], const int n)
{ matNd_copy(dest, src, n); matNd_transpose(dest, n); }
static inline void mat3f_transpose_new(float  dest[ 9], const float  src[ 9])
{ mat3f_copy(dest, src); mat3f_transpose(dest); }
static inline void mat3d_transpose_new(double dest[ 9], const double src[ 9])
{ mat3d_copy(dest, src); mat3d_transpose(dest); }
static inline void mat4f_transpose_new(float  dest[16], const float  src[16])
{ mat4f_copy(dest, src); mat4f_transpose(dest); }
static inline void mat4d_transpose_new(double dest[16], const double src[16])
{ mat4d_copy(dest, src); mat4d_transpose(dest); }



/* fills in a NxN matrix with the identity matrix */
static inline void matNf_identity(float m[], int n)
{
	for(int i=0; i<n; i++)
		for(int j=0; j<n; j++)
			if(i==j)
				m[matN_getIndex(i,j,n)] = 1.0f;
			else
				m[matN_getIndex(i,j,n)] = 0.0f;
}
static inline void matNd_identity(double m[], int n)
{
	for(int i=0; i<n; i++)
		for(int j=0; j<n; j++)
			if(i==j)
				m[matN_getIndex(i,j,n)] = 1.0;
			else
				m[matN_getIndex(i,j,n)] = 0.0;
}
static inline void mat3f_identity(float m[9])
{ matNf_identity(m, 3); }
static inline void mat3d_identity(double m[9])
{ matNd_identity(m, 3); }
static inline void mat4f_identity(float m[16])
{ matNf_identity(m, 4); }
static inline void mat4d_identity(double m[16])
{ matNd_identity(m, 4); }

static inline void matNf_print(const float  m[], const int n)
{
	printf("matrix:\n");
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			printf("%10.3f ", m[matN_getIndex(i,j,n)]);
		printf("\n");
	}
}
static inline void matNd_print(const double m[], const int n)
{
	printf("matrix:\n");
	for(int i=0; i<n; i++)
	{
		for(int j=0; j<n; j++)
			printf("%10.3f ", m[matN_getIndex(i,j,n)]);
		printf("\n");
	}
}
static inline void mat3f_print(const float m[9])
{ matNf_print(m, 3); }
static inline void mat3d_print(const double m[9])
{ matNd_print(m, 3); }
static inline void mat4f_print(const float m[16])
{ matNf_print(m, 4); }
static inline void mat4d_print(const double m[16])
{ matNd_print(m, 4); }

static inline void mat3f_to_mat3d(double dest[ 9], const float  src[ 9])
{ for(int i=0; i<9; i++) dest[i] = (double) src[i]; }
static inline void mat4f_to_mat4d(double dest[16], const float  src[16])
{ for(int i=0; i<16; i++) dest[i] = (double) src[i]; }
static inline void mat3d_to_mat3f(float  dest[ 9], const double src[ 9])
{ for(int i=0; i<9; i++) dest[i] = (float) src[i]; }
static inline void mat4d_to_mat4f(float  dest[16], const double src[16])
{ for(int i=0; i<16; i++) dest[i] = (float) src[i]; }



/* mat[43][df]_invert_new() will invert a matrix and store the
 * inverted matrix at a new location. However, these functions work
 * correctly even if you try to invert a matrix in place. For example,
 * if you want to invert a 4x4 float matrix in place, you can use either of the
 * following:
 *    mat4f_invert_new(matrix, matrix);
 *    mat4f_invert(matrix);
 *
 * Returns 1 if matrix was inverted. If an error occurs, a message is
 * printed to stdout, the function returns 0 and the matrix 'out' is
 * left unchanged.
 */
int mat4f_invert_new(float  dest[16], const float  src[16]);
int mat4d_invert_new(double dest[16], const double src[16]);
int mat3f_invert_new(float  dest[ 9], const float  src[9]);
int mat3d_invert_new(double dest[ 9], const double src[9]);
/* Invert a matrix in place. */
int mat4f_invert(float  matrix[16]);
int mat4d_invert(double matrix[16]);
int mat3f_invert(float  matrix[ 9]);
int mat3d_invert(double matrix[ 9]);


/* Creates a new 3x3 rotation matrix which produces a rotation around
   the given "axis" by the given number of "degrees".  Stores the
   resulting matrix in the "result" matrix (translation part of 4x4
   matrix set to identity). Any data in the 'result' matrix that you
   pass to these functions will be ignored and lost. */
void mat3f_rotateVec_new(float  result[ 9], float  degrees, const float  axis[3]);
void mat3d_rotateVec_new(double result[ 9], double degrees, const double axis[3]);
void mat4f_rotateVec_new(float  result[16], float  degrees, const float  axis[3]);
void mat4d_rotateVec_new(double result[16], double degrees, const double axis[3]);
void mat3f_rotate_new(float  result[ 9], float  degrees, float  axisX, float  axisY, float  axisZ);
void mat3d_rotate_new(double result[ 9], double degrees, double axisX, double axisY, double axisZ);
void mat4f_rotate_new(float  result[16], float  degrees, float  axisX, float  axisY, float  axisZ);
void mat4d_rotate_new(double result[16], double degrees, double axisX, double axisY, double axisZ);



/* Create a new translation matrix (rotation part set to
   identity). Any data in the 'result' matrix that you pass to these
   functions will be ignored and lost. */
void mat4f_translate_new(float  result[16], float  x, float  y, float  z);
void mat4d_translate_new(double result[16], double x, double y, double z);
void mat4f_translateVec_new(float  result[16], const float  xyz[3]);
void mat4d_translateVec_new(double result[16], const double xyz[3]);

/* Set the matrix to the identity and then set the first three numbers along the diagonal starting from the upper-left corner of the matrix */
void mat4f_scale_new(float  result[16], float  x, float  y, float  z);
void mat4d_scale_new(double result[16], double x, double y, double z);
void mat4f_scaleVec_new(float  result[16], const float  xyz[3]);
void mat4d_scaleVec_new(double result[16], const double xyz[3]);

/* Sets dest to the identity and then copies src into the upper-left
 * corner of dest. */
void mat3f_to_mat4f(float  dest[16], const float  src[ 9]);
void mat3d_to_mat4d(double dest[16], const double src[ 9]);
/* Copies upper-left 3x3 portion of src into dest. */
void mat4f_to_mat3f(float  dest[ 9], const float  src[16]);
void mat4d_to_mat3d(double dest[ 9], const double src[16]);

/* Replacements for glFrustum, glOrtho, gluPerspective */
void mat4f_frustum_new(float  result[16], float  left, float  right, float  bottom, float  top, float  near, float  far);
void mat4d_frustum_new(double result[16], double left, double right, double bottom, double top, double near, double far);
void mat4f_ortho_new(float  result[16], float  left, float  right, float  bottom, float  top, float  near, float  far);
void mat4d_ortho_new(double result[16], double left, double right, double bottom, double top, double near, double far);
void mat4f_perspective_new(float  result[16], float  fovy, float  aspect, float  near, float  far);
void mat4d_perspective_new(double result[16], double fovy, double aspect, double near, double far);

/* Replacements for gluLookAt(). Also, provide versions where inputs
 * are arrays. */
void mat4f_lookat_new(float result[16], float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upz);
void mat4d_lookat_new(double result[16], double eyeX, double eyeY, double eyeZ, double centerX, double centerY, double centerZ, double upX, double upY, double upZ);
void mat4f_lookatVec_new(float  result[16], const float  eye[3], const float  center[3], const float  up[3]);
void mat4d_lookatVec_new(double result[16], const double eye[3], const double center[3], const double up[3]);


char* kuhl_text_read(const char *filename);
GLuint kuhl_create_shader(const char *filename, GLuint shader_type);
GLuint kuhl_create_program(const char *vertexFilename, const char *fragFilename, char *attribs[]);
void kuhl_delete_program(GLuint program);
void kuhl_print_program_log(GLuint program);
void kuhl_print_program_info(GLuint program);
	
GLint kuhl_get_uniform(GLuint program, const char *uniformName);


void kuhl_limitfps(int fps);

float kuhl_getfps(int milliseconds);

GLuint kuhl_read_texture_rgba_array(const char *array, int width, int height);

#ifdef KUHL_UTIL_USE_IMAGEMAGICK
float kuhl_make_label(const char *label, GLuint *texName, float color[3], float bgcolor[4], float pointsize);
float kuhl_read_texture_file(const char *filename, GLuint *texName);
#endif

#ifdef KUHL_UTIL_USE_ASSIMP
int kuhl_draw_model_file_ogl2(const char *modelFilename, const char *textureDirname);
int kuhl_model_bounding_box(const char *modelFilename, float min[3], float max[3], float center[3]);
#endif


#endif