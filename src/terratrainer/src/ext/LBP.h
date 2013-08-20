
/*********************************************************************
 * Copyright (C) 2002 Maenpaa
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.Redistributions of source code must retain all copyright
 *   notices, this list of conditions and the following disclaimer.
 *
 * 2.Redistributions in binary form must reproduce the above
 *   copyright notice, this list of conditions and the following
 *   disclaimer in the documentation and/or other materials provided
 *   with the distribution.
 *
 * 3.The name(s) of the author(s) may not be used to endorse or
 *   promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR(S) ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR(S) BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

#ifndef _LBP_H
#define _LBP_H

#include <fstream>
#include <vector>
#include <math.h>
// #include <string>

typedef struct
{
	int x,y;
} integerpoint;

typedef struct
{
	double x,y;
} doublepoint;

class LBP
{
  public:
    //! Gets the distance in descriptor space between Ipoints
    float operator-(const LBP &rhs)
    {
      float sum=0.f;
      for(int i=0; i < 256; ++i)
        sum += (this->histogram[i] - rhs.histogram[i])*(this->histogram[i] - rhs.histogram[i]);
      return sqrt(sum);
    };

    //! returns indexed descriptor 
    int operator[](int index)
    {
      if (index >= 0 && index < 256)
        return histogram[index];
      else
        return 0;
    };
    
    // histogram bins
    int histogram[256];
    std::string class_name;
};

void calculate_points(void);
inline double interpolate_at_ptr(int* upperLeft, int i, int columns);
int* lbp_histogram(int* img, int rows, int columns, int* result, int interpolated);
void LoadLBPHists(char *filename, std::vector<LBP> &hists, bool append, bool classes=false);
void SaveLBPHist(std::ofstream &outfile, const LBP &hists, char *class_name=NULL);

#endif

