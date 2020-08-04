//
//  GlobSeqHelpers.hpp
//  All
//
//  Created by Cristian Andres Vogel on 01/07/2020.
//
#pragma once

#include <string>

class GlobSeqHelpers
{
public:

  /** Clips the value \p x between \p lo and \p hi
   * @param x Input value
   * @param lo Minimum value to be allowed
   * @param hi Maximum value to be allowed
   * If \p x is outside given range, it will be set to one of the boundaries */
  template <typename T>
  T clip(T x, T lo, T hi) { return std::min(std::max(x, lo), hi); }

  /** Linear interpolate between values \p a and \p b
  * @param a Low value
  * @param b High value
  * @param f Value betweeen 0-1 for interpolation */
  template <typename T>
  static inline T lerp(T a, T b, T f) { return ((b - a) * f + a); }
  
  
  /** remove spaces
   */
  std::string chomp(std::string &str)
  {
     str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
     return str;
  }
  /** remove spaces and a number **/
  std::string chomp(std::string &str,  const int i)
  {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), (char) i ), str.end());
    return str;
  }
  
  
  /** true of contains a string
   */
  bool stringContains(const std::string &stringToScan, const std::string &stringToScanFor)
  {
    return ( stringToScan.find( stringToScanFor ) != std::string::npos );
  }
  
};




