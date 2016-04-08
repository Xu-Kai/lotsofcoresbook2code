// This example from an alpha release of the Scalable HeterOgeneous Computing
// (SHOC) Benchmark Suite Alpha v1.1.4a-mic for Intel MIC architecture
// Contact: Kyle Spafford <kys@ornl.gov>
//          Rezaur Rahman <rezaur.rahman@intel.com>
//
// Copyright (c) 2011, UT-Battelle, LLC
// Copyright (c) 2013, Intel Corporation
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//   
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of Oak Ridge National Laboratory, nor UT-Battelle, LLC, 
//    nor the names of its contributors may be used to endorse or promote 
//    products derived from this software without specific prior written 
//    permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
// OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
// THE POSSIBILITY OF SUCH DAMAGE.

// Rob Van der Wijngaart (Intel Corp) created C version
// V1.3 Nathan Wichmann made further improvements
// V1.2 Ramanan fixed few bugs Apr08
// V1.1 John Levesque vectorized the code Feb08
// V1.0 was from Sandia

//     March 09, 2007
//     
//     18-step reduced mechanism for ehtylene-air
//
//     By Tianfeng Lu
//     Princeton University
//     Email: tlu@princeton.edu

#include "S3D.h"

template <class real, int MAXVL>
    __declspec(target(mic)) void 
rdsmh_i_VEC(real * RESTRICT T, real * RESTRICT SMH) 
{

    ALIGN64      real TN[MAXVL*5], TLOG[MAXVL], TI[MAXVL];
    int VL = MAXVL;
    int I;

    vrda_log_(&VL, T, TLOG);
    #pragma vector aligned
    for (I=1; I<=VL; I++) TI(I) = 1.0e0/T(I);

    #pragma vector aligned
    for (I=1; I<=VL; I++) TN(I,1) = TLOG(I) - 1.0;
    #pragma vector aligned
    for (I=1; I<=VL; I++) TN(I,2) = T(I);
    #pragma vector aligned
    for (I=1; I<=VL; I++) TN(I,3) = TN(I,2)*T(I);
    #pragma vector aligned
    for (I=1; I<=VL; I++) TN(I,4) = TN(I,3)*T(I);
    #pragma vector aligned
    for (I=1; I<=VL; I++) TN(I,5) = TN(I,4)*T(I);

    #pragma vector aligned
    for (I=1; I<=VL; I++) 
    {
        if (T(I) > 1.e3) 
        {
            SMH(I,1) = -3.20502331e+00 + 9.50158922e+02*TI(I) 
                + 3.33727920e+00*TN(I,1) - 2.47012365e-05*TN(I,2) 
                + 8.32427963e-08*TN(I,3) - 1.49638662e-11*TN(I,4) 
                + 1.00127688e-15*TN(I,5);
            SMH(I,2) = -4.46682914e-01 - 2.54736599e+04*TI(I) 
                + 2.50000001e+00*TN(I,1) - 1.15421486e-11*TN(I,2) 
                + 2.69269913e-15*TN(I,3) - 3.94596029e-19*TN(I,4) 
                + 2.49098679e-23*TN(I,5);
            SMH(I,3) = 4.78433864e+00 - 2.92175791e+04*TI(I) 
                + 2.56942078e+00*TN(I,1) - 4.29870569e-05*TN(I,2) 
                + 6.99140982e-09*TN(I,3) - 8.34814992e-13*TN(I,4) 
                + 6.14168455e-17*TN(I,5);
            SMH(I,4) = 5.45323129e+00 + 1.08845772e+03*TI(I) 
                + 3.28253784e+00*TN(I,1) + 7.41543770e-04*TN(I,2) 
                - 1.26327778e-07*TN(I,3) + 1.74558796e-11*TN(I,4) 
                - 1.08358897e-15*TN(I,5);
            SMH(I,5) = 4.47669610e+00 - 3.85865700e+03*TI(I) 
                + 3.09288767e+00*TN(I,1) + 2.74214858e-04*TN(I,2) 
                + 2.10842047e-08*TN(I,3) - 7.32884630e-12*TN(I,4) 
                + 5.87061880e-16*TN(I,5);
            SMH(I,6) = 4.96677010e+00 + 3.00042971e+04*TI(I) 
                + 3.03399249e+00*TN(I,1) + 1.08845902e-03*TN(I,2) 
                - 2.73454197e-08*TN(I,3) - 8.08683225e-12*TN(I,4) 
                + 8.41004960e-16*TN(I,5);
            SMH(I,7) = 3.78510215e+00 - 1.11856713e+02*TI(I) 
                + 4.01721090e+00*TN(I,1) + 1.11991007e-03*TN(I,2) 
                - 1.05609692e-07*TN(I,3) + 9.52053083e-12*TN(I,4) 
                - 5.39542675e-16*TN(I,5);
            SMH(I,8) = 2.91615662e+00 + 1.78617877e+04*TI(I) 
                + 4.16500285e+00*TN(I,1) + 2.45415847e-03*TN(I,2) 
                - 3.16898708e-07*TN(I,3) + 3.09321655e-11*TN(I,4) 
                - 1.43954153e-15*TN(I,5);
            SMH(I,9) = 5.48497999e+00 - 7.10124364e+04*TI(I) 
                + 2.87846473e+00*TN(I,1) + 4.85456841e-04*TN(I,2) 
                + 2.40742758e-08*TN(I,3) - 1.08906541e-11*TN(I,4) 
                + 8.80396915e-16*TN(I,5);
            SMH(I,10) = 6.17119324e+00 - 4.62636040e+04*TI(I) 
                + 2.87410113e+00*TN(I,1) + 1.82819646e-03*TN(I,2) 
                - 2.34824328e-07*TN(I,3) + 2.16816291e-11*TN(I,4) 
                - 9.38637835e-16*TN(I,5);
            SMH(I,11) = 8.62650169e+00 - 5.09259997e+04*TI(I) 
                + 2.29203842e+00*TN(I,1) + 2.32794319e-03*TN(I,2) 
                - 3.35319912e-07*TN(I,3) + 3.48255000e-11*TN(I,4) 
                - 1.69858183e-15*TN(I,5);
            SMH(I,12) = 8.48007179e+00 - 1.67755843e+04*TI(I) 
                + 2.28571772e+00*TN(I,1) + 3.61995018e-03*TN(I,2) 
                - 4.97857247e-07*TN(I,3) + 4.96403870e-11*TN(I,4) 
                - 2.33577197e-15*TN(I,5);
            SMH(I,13) = 1.84373180e+01 + 9.46834459e+03*TI(I) 
                + 7.48514950e-02*TN(I,1) + 6.69547335e-03*TN(I,2) 
                - 9.55476348e-07*TN(I,3) + 1.01910446e-10*TN(I,4) 
                - 5.09076150e-15*TN(I,5); 
            SMH(I,14) = 7.81868772e+00 + 1.41518724e+04*TI(I) 
                + 2.71518561e+00*TN(I,1) + 1.03126372e-03*TN(I,2) 
                - 1.66470962e-07*TN(I,3) + 1.91710840e-11*TN(I,4) 
                - 1.01823858e-15*TN(I,5);
        }
    }

    #pragma vector aligned
    for (I=1; I<=VL; I++) 
    {

        if (T(I) > 1.e3) 
        {
            SMH(I,15) = 2.27163806e+00 + 4.87591660e+04*TI(I) 
                + 3.85746029e+00*TN(I,1) + 2.20718513e-03*TN(I,2) 
                - 3.69135673e-07*TN(I,3) + 4.36241823e-11*TN(I,4) 
                - 2.36042082e-15*TN(I,5);
            SMH(I,16) = 9.79834492e+00 - 4.01191815e+03*TI(I) 
                + 2.77217438e+00*TN(I,1) + 2.47847763e-03*TN(I,2) 
                - 4.14076022e-07*TN(I,3) + 4.90968148e-11*TN(I,4) 
                - 2.66754356e-15*TN(I,5);
            SMH(I,17) = 1.36563230e+01 + 1.39958323e+04*TI(I) 
                + 1.76069008e+00*TN(I,1) + 4.60000041e-03*TN(I,2) 
                - 7.37098022e-07*TN(I,3) + 8.38676767e-11*TN(I,4) 
                - 4.41927820e-15*TN(I,5);
            SMH(I,18) = 2.92957500e+00 - 1.27832520e+02*TI(I) 
                + 3.77079900e+00*TN(I,1) + 3.93574850e-03*TN(I,2) 
                - 4.42730667e-07*TN(I,3) + 3.28702583e-11*TN(I,4) 
                - 1.05630800e-15*TN(I,5);
            SMH(I,19) = -1.23028121e+00 - 2.59359992e+04*TI(I) 
                + 4.14756964e+00*TN(I,1) + 2.98083332e-03*TN(I,2) 
                - 3.95491420e-07*TN(I,3) + 3.89510143e-11*TN(I,4) 
                - 1.80617607e-15*TN(I,5);
            SMH(I,20) = 6.40237010e-01 - 4.83166880e+04*TI(I) 
                + 4.27803400e+00*TN(I,1) + 2.37814020e-03*TN(I,2) 
                - 2.71683483e-07*TN(I,3) + 2.12190050e-11*TN(I,4) 
                - 7.44318950e-16*TN(I,5);
            SMH(I,21) = 7.78732378e+00 - 3.46128739e+04*TI(I) 
                + 3.01672400e+00*TN(I,1) + 5.16511460e-03*TN(I,2) 
                - 7.80137248e-07*TN(I,3) + 8.48027400e-11*TN(I,4) 
                - 4.31303520e-15*TN(I,5);
            SMH(I,22) = 1.03053693e+01 - 4.93988614e+03*TI(I) 
                + 2.03611116e+00*TN(I,1) + 7.32270755e-03*TN(I,2) 
                - 1.11846319e-06*TN(I,3) + 1.22685769e-10*TN(I,4) 
                - 6.28530305e-15*TN(I,5);
            SMH(I,23) = 1.34624343e+01 - 1.28575200e+04*TI(I) 
                + 1.95465642e+00*TN(I,1) + 8.69863610e-03*TN(I,2) 
                - 1.33034445e-06*TN(I,3) + 1.46014741e-10*TN(I,4) 
                - 7.48207880e-15*TN(I,5);
            SMH(I,24) = 1.51156107e+01 + 1.14263932e+04*TI(I) 
                + 1.07188150e+00*TN(I,1) + 1.08426339e-02*TN(I,2) 
                - 1.67093445e-06*TN(I,3) + 1.84510001e-10*TN(I,4) 
                - 9.50014450e-15*TN(I,5);
            SMH(I,25) = -3.93025950e+00 - 1.93272150e+04*TI(I) 
                + 5.62820580e+00*TN(I,1) + 2.04267005e-03*TN(I,2) 
                - 2.65575783e-07*TN(I,3) + 2.38550433e-11*TN(I,4) 
                - 9.70391600e-16*TN(I,5);
            SMH(I,26) = 6.32247205e-01 + 7.55105311e+03*TI(I) 
                + 4.51129732e+00*TN(I,1) + 4.50179872e-03*TN(I,2) 
                - 6.94899392e-07*TN(I,3) + 7.69454902e-11*TN(I,4) 
                - 3.97419100e-15*TN(I,5);
            SMH(I,27) = -5.03208790e+00 - 4.90321780e+02*TI(I) 
                + 5.97566990e+00*TN(I,1) + 4.06529570e-03*TN(I,2) 
                - 4.57270750e-07*TN(I,3) + 3.39192008e-11*TN(I,4) 
                - 1.08800855e-15*TN(I,5);
            SMH(I,28) = -3.48079170e+00 + 2.25931220e+04*TI(I) 
                + 5.40411080e+00*TN(I,1) + 5.86152950e-03*TN(I,2) 
                - 7.04385617e-07*TN(I,3) + 5.69770425e-11*TN(I,4) 
                - 2.04924315e-15*TN(I,5);
            SMH(I,29) = -1.12430500e+01 - 1.74824490e+04*TI(I) 
                + 6.50078770e+00*TN(I,1) + 7.16236550e-03*TN(I,2) 
                - 9.46360533e-07*TN(I,3) + 9.23400083e-11*TN(I,4) 
                - 4.51819435e-15*TN(I,5);
            SMH(I,30) = -1.33133500e+01 + 9.23570300e+02*TI(I) 
                + 6.73225700e+00*TN(I,1) + 7.45417000e-03*TN(I,2) 
                - 8.24983167e-07*TN(I,3) + 6.01001833e-11*TN(I,4) 
                - 1.88310200e-15*TN(I,5);
            SMH(I,31) = -1.55152970e+01 - 7.97622360e+03*TI(I) 
                + 7.70974790e+00*TN(I,1) + 8.01574250e-03*TN(I,2) 
                - 8.78670633e-07*TN(I,3) + 6.32402933e-11*TN(I,4) 
                - 1.94313595e-15*TN(I,5);

        }
    }

        #pragma vector aligned
    for (I=1; I<=VL; I++) 
    {
        if (T(I) <= 1.e3) 
        {
            SMH(I,1) = 6.83010238e-01 + 9.17935173e+02*TI(I) 
                + 2.34433112e+00*TN(I,1) + 3.99026037e-03*TN(I,2) 
                - 3.24635850e-06*TN(I,3) + 1.67976745e-09*TN(I,4) 
                - 3.68805881e-13*TN(I,5);
            SMH(I,2) = -4.46682853e-01 - 2.54736599e+04*TI(I) 
                + 2.50000000e+00*TN(I,1) + 3.52666409e-13*TN(I,2) 
                - 3.32653273e-16*TN(I,3) + 1.91734693e-19*TN(I,4) 
                - 4.63866166e-23*TN(I,5);
            SMH(I,3) = 2.05193346e+00 - 2.91222592e+04*TI(I) 
                + 3.16826710e+00*TN(I,1) - 1.63965942e-03*TN(I,2) 
                + 1.10717733e-06*TN(I,3) - 5.10672187e-10*TN(I,4) 
                + 1.05632986e-13*TN(I,5);
            SMH(I,4) = 3.65767573e+00 + 1.06394356e+03*TI(I) 
                + 3.78245636e+00*TN(I,1) - 1.49836708e-03*TN(I,2) 
                + 1.64121700e-06*TN(I,3) - 8.06774591e-10*TN(I,4) 
                + 1.62186419e-13*TN(I,5);
            SMH(I,5) = -1.03925458e-01 - 3.61508056e+03*TI(I) 
                + 3.99201543e+00*TN(I,1) - 1.20065876e-03*TN(I,2) 
                + 7.69656402e-07*TN(I,3) - 3.23427778e-10*TN(I,4) 
                + 6.82057350e-14*TN(I,5);
            SMH(I,6) = -8.49032208e-01 + 3.02937267e+04*TI(I) 
                + 4.19864056e+00*TN(I,1) - 1.01821705e-03*TN(I,2) 
                + 1.08673369e-06*TN(I,3) - 4.57330885e-10*TN(I,4) 
                + 8.85989085e-14*TN(I,5);
            SMH(I,7) = 3.71666245e+00 - 2.94808040e+02*TI(I) 
                + 4.30179801e+00*TN(I,1) - 2.37456025e-03*TN(I,2) 
                + 3.52638152e-06*TN(I,3) - 2.02303245e-09*TN(I,4) 
                + 4.64612562e-13*TN(I,5);
            SMH(I,8) = 3.43505074e+00 + 1.77025821e+04*TI(I) 
                + 4.27611269e+00*TN(I,1) - 2.71411208e-04*TN(I,2) 
                + 2.78892835e-06*TN(I,3) - 1.79809011e-09*TN(I,4) 
                + 4.31227182e-13*TN(I,5);
            SMH(I,9) = 2.08401108e+00 - 7.07972934e+04*TI(I) 
                + 3.48981665e+00*TN(I,1) + 1.61917771e-04*TN(I,2) 
                - 2.81498442e-07*TN(I,3) + 2.63514439e-10*TN(I,4) 
                - 7.03045335e-14*TN(I,5);
            SMH(I,10) = 1.56253185e+00 - 4.60040401e+04*TI(I) 
                + 3.76267867e+00*TN(I,1) + 4.84436072e-04*TN(I,2) 
                + 4.65816402e-07*TN(I,3) - 3.20909294e-10*TN(I,4) 
                + 8.43708595e-14*TN(I,5);
            SMH(I,11) = -7.69118967e-01 - 5.04968163e+04*TI(I) 
                + 4.19860411e+00*TN(I,1) - 1.18330710e-03*TN(I,2) 
                + 1.37216037e-06*TN(I,3) - 5.57346651e-10*TN(I,4) 
                + 9.71573685e-14*TN(I,5);
            SMH(I,12) = 1.60456433e+00 - 1.64449988e+04*TI(I) 
                + 3.67359040e+00*TN(I,1) + 1.00547588e-03*TN(I,2) 
                + 9.55036427e-07*TN(I,3) - 5.72597854e-10*TN(I,4) 
                + 1.27192867e-13*TN(I,5);
            SMH(I,13) = -4.64130376e+00 + 1.02466476e+04*TI(I) 
                + 5.14987613e+00*TN(I,1) - 6.83548940e-03*TN(I,2) 
                + 8.19667665e-06*TN(I,3) - 4.03952522e-09*TN(I,4) 
                + 8.33469780e-13*TN(I,5); 
            SMH(I,14) = 3.50840928e+00 + 1.43440860e+04*TI(I) 
                + 3.57953347e+00*TN(I,1) - 3.05176840e-04*TN(I,2) 
                + 1.69469055e-07*TN(I,3) + 7.55838237e-11*TN(I,4) 
                - 4.52212249e-14*TN(I,5);
            SMH(I,15) = 9.90105222e+00 + 4.83719697e+04*TI(I) 
                + 2.35677352e+00*TN(I,1) + 4.49229839e-03*TN(I,2) 
                - 1.18726045e-06*TN(I,3) + 2.04932518e-10*TN(I,4) 
                - 7.18497740e-15*TN(I,5);
        }
    }

    #pragma vector aligned
    for (I=1; I<=VL; I++) 
    {
        if (T(I) <= 1.e3) 
        {
            SMH(I,16) = 3.39437243e+00 - 3.83956496e+03*TI(I) 
                + 4.22118584e+00*TN(I,1) - 1.62196266e-03*TN(I,2) 
                + 2.29665743e-06*TN(I,3) - 1.10953411e-09*TN(I,4) 
                + 2.16884433e-13*TN(I,5);
            SMH(I,17) = 6.02812900e-01 + 1.43089567e+04*TI(I) 
                + 4.79372315e+00*TN(I,1) - 4.95416685e-03*TN(I,2) 
                + 6.22033347e-06*TN(I,3) - 3.16071051e-09*TN(I,4) 
                + 6.58863260e-13*TN(I,5);
            SMH(I,18) = 1.31521770e+01 - 9.78601100e+02*TI(I) 
                + 2.10620400e+00*TN(I,1) + 3.60829750e-03*TN(I,2) 
                + 8.89745333e-07*TN(I,3) - 6.14803000e-10*TN(I,4) 
                + 1.03780500e-13*TN(I,5);
            SMH(I,19) = 1.39397051e+01 - 2.64289807e+04*TI(I) 
                + 8.08681094e-01*TN(I,1) + 1.16807815e-02*TN(I,2) 
                - 5.91953025e-06*TN(I,3) + 2.33460364e-09*TN(I,4) 
                - 4.25036487e-13*TN(I,5);
            SMH(I,20) = 5.92039100e+00 - 4.86217940e+04*TI(I) 
                + 3.28154830e+00*TN(I,1) + 3.48823955e-03*TN(I,2) 
                - 3.97587400e-07*TN(I,3) - 1.00870267e-10*TN(I,4) 
                + 4.90947725e-14*TN(I,5);
            SMH(I,21) = 8.51054025e+00 - 3.48598468e+04*TI(I) 
                + 3.21246645e+00*TN(I,1) + 7.57395810e-04*TN(I,2) 
                + 4.32015687e-06*TN(I,3) - 2.98048206e-09*TN(I,4) 
                + 7.35754365e-13*TN(I,5);
            SMH(I,22) = 4.09733096e+00 - 5.08977593e+03*TI(I) 
                + 3.95920148e+00*TN(I,1) - 3.78526124e-03*TN(I,2) 
                + 9.51650487e-06*TN(I,3) - 5.76323961e-09*TN(I,4) 
                + 1.34942187e-12*TN(I,5);
            SMH(I,23) = 4.70720924e+00 - 1.28416265e+04*TI(I) 
                + 4.30646568e+00*TN(I,1) - 2.09329446e-03*TN(I,2) 
                + 8.28571345e-06*TN(I,3) - 4.99272172e-09*TN(I,4) 
                + 1.15254502e-12*TN(I,5);
            SMH(I,24) = 2.66682316e+00 + 1.15222055e+04*TI(I) 
                + 4.29142492e+00*TN(I,1) - 2.75077135e-03*TN(I,2) 
                + 9.99063813e-06*TN(I,3) - 5.90388571e-09*TN(I,4) 
                + 1.34342886e-12*TN(I,5);
            SMH(I,25) = 1.24904170e+01 - 2.00594490e+04*TI(I) 
                + 2.25172140e+00*TN(I,1) + 8.82751050e-03*TN(I,2) 
                - 3.95485017e-06*TN(I,3) + 1.43964658e-09*TN(I,4) 
                - 2.53324055e-13*TN(I,5);
            SMH(I,26) = 1.22156480e+01 + 7.04291804e+03*TI(I) 
                + 2.13583630e+00*TN(I,1) + 9.05943605e-03*TN(I,2) 
                - 2.89912457e-06*TN(I,3) + 7.78664640e-10*TN(I,4) 
                - 1.00728807e-13*TN(I,5);
            SMH(I,27) = 9.57145350e+00 - 1.52147660e+03*TI(I) 
                + 3.40906240e+00*TN(I,1) + 5.36928700e-03*TN(I,2) 
                + 3.15248750e-07*TN(I,3) + 5.96548592e-10*TN(I,4) 
                + 1.43369255e-13*TN(I,5);
            SMH(I,28) = 4.10301590e+00 + 2.15728780e+04*TI(I) 
                + 4.72945950e+00*TN(I,1) - 1.59664290e-03*TN(I,2) 
                + 7.92248683e-06*TN(I,3) - 4.78821758e-09*TN(I,4) 
                + 1.09655560e-12*TN(I,5);
            SMH(I,29) = 1.71732140e+01 - 1.92456290e+04*TI(I) 
                + 1.36318350e+00*TN(I,1) + 9.90691050e-03*TN(I,2) 
                + 2.08284333e-06*TN(I,3) - 2.77962958e-09*TN(I,4) 
                + 7.92328550e-13*TN(I,5);
            SMH(I,30) = 1.61453400e+01 - 1.07482600e+03*TI(I) 
                + 1.49330700e+00*TN(I,1) + 1.04625900e-02*TN(I,2) 
                + 7.47799000e-07*TN(I,3) - 1.39076000e-09*TN(I,4) 
                + 3.57907300e-13*TN(I,5);
            SMH(I,31) = 2.11360340e+01 - 1.03123460e+04*TI(I) 
                + 1.04911730e+00*TN(I,1) + 1.30044865e-02*TN(I,2) 
                + 3.92375267e-07*TN(I,3) - 1.63292767e-09*TN(I,4) 
                + 4.68601035e-13*TN(I,5) ;
        }
    }
}

template <class real, int MAXVL>
__declspec(target(mic)) void
rdsmh_i_ (int *VLp, real * RESTRICT T, real * RESTRICT SMH) 
{

    ALIGN64      real TN[MAXVL*5], TLOG[MAXVL], TI[MAXVL];
    int VL = *VLp;
    int I;
    vrda_log_(VLp, T, TLOG);
    for (I=1; I<=VL; I++) TI(I) = 1.0e0/T(I);

    for (I=1; I<=VL; I++) TN(I,1) = TLOG(I) - 1.0;
    for (I=1; I<=VL; I++) TN(I,2) = T(I);
    for (I=1; I<=VL; I++) TN(I,3) = TN(I,2)*T(I);
    for (I=1; I<=VL; I++) TN(I,4) = TN(I,3)*T(I);
    for (I=1; I<=VL; I++) TN(I,5) = TN(I,4)*T(I);

    for (I=1; I<=VL; I++) 
    {
        if (T(I) > 1.e3) 
        {
            SMH(I,1) = -3.20502331e+00 + 9.50158922e+02*TI(I) 
                + 3.33727920e+00*TN(I,1) - 2.47012365e-05*TN(I,2) 
                + 8.32427963e-08*TN(I,3) - 1.49638662e-11*TN(I,4) 
                + 1.00127688e-15*TN(I,5);
            SMH(I,2) = -4.46682914e-01 - 2.54736599e+04*TI(I) 
                + 2.50000001e+00*TN(I,1) - 1.15421486e-11*TN(I,2) 
                + 2.69269913e-15*TN(I,3) - 3.94596029e-19*TN(I,4) 
                + 2.49098679e-23*TN(I,5);
            SMH(I,3) = 4.78433864e+00 - 2.92175791e+04*TI(I) 
                + 2.56942078e+00*TN(I,1) - 4.29870569e-05*TN(I,2) 
                + 6.99140982e-09*TN(I,3) - 8.34814992e-13*TN(I,4) 
                + 6.14168455e-17*TN(I,5);
            SMH(I,4) = 5.45323129e+00 + 1.08845772e+03*TI(I) 
                + 3.28253784e+00*TN(I,1) + 7.41543770e-04*TN(I,2) 
                - 1.26327778e-07*TN(I,3) + 1.74558796e-11*TN(I,4) 
                - 1.08358897e-15*TN(I,5);
            SMH(I,5) = 4.47669610e+00 - 3.85865700e+03*TI(I) 
                + 3.09288767e+00*TN(I,1) + 2.74214858e-04*TN(I,2) 
                + 2.10842047e-08*TN(I,3) - 7.32884630e-12*TN(I,4) 
                + 5.87061880e-16*TN(I,5);
            SMH(I,6) = 4.96677010e+00 + 3.00042971e+04*TI(I) 
                + 3.03399249e+00*TN(I,1) + 1.08845902e-03*TN(I,2) 
                - 2.73454197e-08*TN(I,3) - 8.08683225e-12*TN(I,4) 
                + 8.41004960e-16*TN(I,5);
            SMH(I,7) = 3.78510215e+00 - 1.11856713e+02*TI(I) 
                + 4.01721090e+00*TN(I,1) + 1.11991007e-03*TN(I,2) 
                - 1.05609692e-07*TN(I,3) + 9.52053083e-12*TN(I,4) 
                - 5.39542675e-16*TN(I,5);
            SMH(I,8) = 2.91615662e+00 + 1.78617877e+04*TI(I) 
                + 4.16500285e+00*TN(I,1) + 2.45415847e-03*TN(I,2) 
                - 3.16898708e-07*TN(I,3) + 3.09321655e-11*TN(I,4) 
                - 1.43954153e-15*TN(I,5);
            SMH(I,9) = 5.48497999e+00 - 7.10124364e+04*TI(I) 
                + 2.87846473e+00*TN(I,1) + 4.85456841e-04*TN(I,2) 
                + 2.40742758e-08*TN(I,3) - 1.08906541e-11*TN(I,4) 
                + 8.80396915e-16*TN(I,5);
            SMH(I,10) = 6.17119324e+00 - 4.62636040e+04*TI(I) 
                + 2.87410113e+00*TN(I,1) + 1.82819646e-03*TN(I,2) 
                - 2.34824328e-07*TN(I,3) + 2.16816291e-11*TN(I,4) 
                - 9.38637835e-16*TN(I,5);
            SMH(I,11) = 8.62650169e+00 - 5.09259997e+04*TI(I) 
                + 2.29203842e+00*TN(I,1) + 2.32794319e-03*TN(I,2) 
                - 3.35319912e-07*TN(I,3) + 3.48255000e-11*TN(I,4) 
                - 1.69858183e-15*TN(I,5);
            SMH(I,12) = 8.48007179e+00 - 1.67755843e+04*TI(I) 
                + 2.28571772e+00*TN(I,1) + 3.61995018e-03*TN(I,2) 
                - 4.97857247e-07*TN(I,3) + 4.96403870e-11*TN(I,4) 
                - 2.33577197e-15*TN(I,5);
            SMH(I,13) = 1.84373180e+01 + 9.46834459e+03*TI(I) 
                + 7.48514950e-02*TN(I,1) + 6.69547335e-03*TN(I,2) 
                - 9.55476348e-07*TN(I,3) + 1.01910446e-10*TN(I,4) 
                - 5.09076150e-15*TN(I,5); 
            SMH(I,14) = 7.81868772e+00 + 1.41518724e+04*TI(I) 
                + 2.71518561e+00*TN(I,1) + 1.03126372e-03*TN(I,2) 
                - 1.66470962e-07*TN(I,3) + 1.91710840e-11*TN(I,4) 
                - 1.01823858e-15*TN(I,5);
        }
    }

    for (I=1; I<=VL; I++) 
    {
        if (T(I) > 1.e3) 
        {
            SMH(I,15) = 2.27163806e+00 + 4.87591660e+04*TI(I) 
                + 3.85746029e+00*TN(I,1) + 2.20718513e-03*TN(I,2) 
                - 3.69135673e-07*TN(I,3) + 4.36241823e-11*TN(I,4) 
                - 2.36042082e-15*TN(I,5);
            SMH(I,16) = 9.79834492e+00 - 4.01191815e+03*TI(I) 
                + 2.77217438e+00*TN(I,1) + 2.47847763e-03*TN(I,2) 
                - 4.14076022e-07*TN(I,3) + 4.90968148e-11*TN(I,4) 
                - 2.66754356e-15*TN(I,5);
            SMH(I,17) = 1.36563230e+01 + 1.39958323e+04*TI(I) 
                + 1.76069008e+00*TN(I,1) + 4.60000041e-03*TN(I,2) 
                - 7.37098022e-07*TN(I,3) + 8.38676767e-11*TN(I,4) 
                - 4.41927820e-15*TN(I,5);
            SMH(I,18) = 2.92957500e+00 - 1.27832520e+02*TI(I) 
                + 3.77079900e+00*TN(I,1) + 3.93574850e-03*TN(I,2) 
                - 4.42730667e-07*TN(I,3) + 3.28702583e-11*TN(I,4) 
                - 1.05630800e-15*TN(I,5);
            SMH(I,19) = -1.23028121e+00 - 2.59359992e+04*TI(I) 
                + 4.14756964e+00*TN(I,1) + 2.98083332e-03*TN(I,2) 
                - 3.95491420e-07*TN(I,3) + 3.89510143e-11*TN(I,4) 
                - 1.80617607e-15*TN(I,5);
            SMH(I,20) = 6.40237010e-01 - 4.83166880e+04*TI(I) 
                + 4.27803400e+00*TN(I,1) + 2.37814020e-03*TN(I,2) 
                - 2.71683483e-07*TN(I,3) + 2.12190050e-11*TN(I,4) 
                - 7.44318950e-16*TN(I,5);
            SMH(I,21) = 7.78732378e+00 - 3.46128739e+04*TI(I) 
                + 3.01672400e+00*TN(I,1) + 5.16511460e-03*TN(I,2) 
                - 7.80137248e-07*TN(I,3) + 8.48027400e-11*TN(I,4) 
                - 4.31303520e-15*TN(I,5);
            SMH(I,22) = 1.03053693e+01 - 4.93988614e+03*TI(I) 
                + 2.03611116e+00*TN(I,1) + 7.32270755e-03*TN(I,2) 
                - 1.11846319e-06*TN(I,3) + 1.22685769e-10*TN(I,4) 
                - 6.28530305e-15*TN(I,5);
            SMH(I,23) = 1.34624343e+01 - 1.28575200e+04*TI(I) 
                + 1.95465642e+00*TN(I,1) + 8.69863610e-03*TN(I,2) 
                - 1.33034445e-06*TN(I,3) + 1.46014741e-10*TN(I,4) 
                - 7.48207880e-15*TN(I,5);
            SMH(I,24) = 1.51156107e+01 + 1.14263932e+04*TI(I) 
                + 1.07188150e+00*TN(I,1) + 1.08426339e-02*TN(I,2) 
                - 1.67093445e-06*TN(I,3) + 1.84510001e-10*TN(I,4) 
                - 9.50014450e-15*TN(I,5);
            SMH(I,25) = -3.93025950e+00 - 1.93272150e+04*TI(I) 
                + 5.62820580e+00*TN(I,1) + 2.04267005e-03*TN(I,2) 
                - 2.65575783e-07*TN(I,3) + 2.38550433e-11*TN(I,4) 
                - 9.70391600e-16*TN(I,5);
            SMH(I,26) = 6.32247205e-01 + 7.55105311e+03*TI(I) 
                + 4.51129732e+00*TN(I,1) + 4.50179872e-03*TN(I,2) 
                - 6.94899392e-07*TN(I,3) + 7.69454902e-11*TN(I,4) 
                - 3.97419100e-15*TN(I,5);
            SMH(I,27) = -5.03208790e+00 - 4.90321780e+02*TI(I) 
                + 5.97566990e+00*TN(I,1) + 4.06529570e-03*TN(I,2) 
                - 4.57270750e-07*TN(I,3) + 3.39192008e-11*TN(I,4) 
                - 1.08800855e-15*TN(I,5);
            SMH(I,28) = -3.48079170e+00 + 2.25931220e+04*TI(I) 
                + 5.40411080e+00*TN(I,1) + 5.86152950e-03*TN(I,2) 
                - 7.04385617e-07*TN(I,3) + 5.69770425e-11*TN(I,4) 
                - 2.04924315e-15*TN(I,5);
            SMH(I,29) = -1.12430500e+01 - 1.74824490e+04*TI(I) 
                + 6.50078770e+00*TN(I,1) + 7.16236550e-03*TN(I,2) 
                - 9.46360533e-07*TN(I,3) + 9.23400083e-11*TN(I,4) 
                - 4.51819435e-15*TN(I,5);
            SMH(I,30) = -1.33133500e+01 + 9.23570300e+02*TI(I) 
                + 6.73225700e+00*TN(I,1) + 7.45417000e-03*TN(I,2) 
                - 8.24983167e-07*TN(I,3) + 6.01001833e-11*TN(I,4) 
                - 1.88310200e-15*TN(I,5);
            SMH(I,31) = -1.55152970e+01 - 7.97622360e+03*TI(I) 
                + 7.70974790e+00*TN(I,1) + 8.01574250e-03*TN(I,2) 
                - 8.78670633e-07*TN(I,3) + 6.32402933e-11*TN(I,4) 
                - 1.94313595e-15*TN(I,5);

        }
    }

    for (I=1; I<=VL; I++) 
    {
        if (T(I) <= 1.e3) 
        {
            SMH(I,1) = 6.83010238e-01 + 9.17935173e+02*TI(I) 
                + 2.34433112e+00*TN(I,1) + 3.99026037e-03*TN(I,2) 
                - 3.24635850e-06*TN(I,3) + 1.67976745e-09*TN(I,4) 
                - 3.68805881e-13*TN(I,5);
            SMH(I,2) = -4.46682853e-01 - 2.54736599e+04*TI(I) 
                + 2.50000000e+00*TN(I,1) + 3.52666409e-13*TN(I,2) 
                - 3.32653273e-16*TN(I,3) + 1.91734693e-19*TN(I,4) 
                - 4.63866166e-23*TN(I,5);
            SMH(I,3) = 2.05193346e+00 - 2.91222592e+04*TI(I) 
                + 3.16826710e+00*TN(I,1) - 1.63965942e-03*TN(I,2) 
                + 1.10717733e-06*TN(I,3) - 5.10672187e-10*TN(I,4) 
                + 1.05632986e-13*TN(I,5);
            SMH(I,4) = 3.65767573e+00 + 1.06394356e+03*TI(I) 
                + 3.78245636e+00*TN(I,1) - 1.49836708e-03*TN(I,2) 
                + 1.64121700e-06*TN(I,3) - 8.06774591e-10*TN(I,4) 
                + 1.62186419e-13*TN(I,5);
            SMH(I,5) = -1.03925458e-01 - 3.61508056e+03*TI(I) 
                + 3.99201543e+00*TN(I,1) - 1.20065876e-03*TN(I,2) 
                + 7.69656402e-07*TN(I,3) - 3.23427778e-10*TN(I,4) 
                + 6.82057350e-14*TN(I,5);
            SMH(I,6) = -8.49032208e-01 + 3.02937267e+04*TI(I) 
                + 4.19864056e+00*TN(I,1) - 1.01821705e-03*TN(I,2) 
                + 1.08673369e-06*TN(I,3) - 4.57330885e-10*TN(I,4) 
                + 8.85989085e-14*TN(I,5);
            SMH(I,7) = 3.71666245e+00 - 2.94808040e+02*TI(I) 
                + 4.30179801e+00*TN(I,1) - 2.37456025e-03*TN(I,2) 
                + 3.52638152e-06*TN(I,3) - 2.02303245e-09*TN(I,4) 
                + 4.64612562e-13*TN(I,5);
            SMH(I,8) = 3.43505074e+00 + 1.77025821e+04*TI(I) 
                + 4.27611269e+00*TN(I,1) - 2.71411208e-04*TN(I,2) 
                + 2.78892835e-06*TN(I,3) - 1.79809011e-09*TN(I,4) 
                + 4.31227182e-13*TN(I,5);
            SMH(I,9) = 2.08401108e+00 - 7.07972934e+04*TI(I) 
                + 3.48981665e+00*TN(I,1) + 1.61917771e-04*TN(I,2) 
                - 2.81498442e-07*TN(I,3) + 2.63514439e-10*TN(I,4) 
                - 7.03045335e-14*TN(I,5);
            SMH(I,10) = 1.56253185e+00 - 4.60040401e+04*TI(I) 
                + 3.76267867e+00*TN(I,1) + 4.84436072e-04*TN(I,2) 
                + 4.65816402e-07*TN(I,3) - 3.20909294e-10*TN(I,4) 
                + 8.43708595e-14*TN(I,5);
            SMH(I,11) = -7.69118967e-01 - 5.04968163e+04*TI(I) 
                + 4.19860411e+00*TN(I,1) - 1.18330710e-03*TN(I,2) 
                + 1.37216037e-06*TN(I,3) - 5.57346651e-10*TN(I,4) 
                + 9.71573685e-14*TN(I,5);
            SMH(I,12) = 1.60456433e+00 - 1.64449988e+04*TI(I) 
                + 3.67359040e+00*TN(I,1) + 1.00547588e-03*TN(I,2) 
                + 9.55036427e-07*TN(I,3) - 5.72597854e-10*TN(I,4) 
                + 1.27192867e-13*TN(I,5);
            SMH(I,13) = -4.64130376e+00 + 1.02466476e+04*TI(I) 
                + 5.14987613e+00*TN(I,1) - 6.83548940e-03*TN(I,2) 
                + 8.19667665e-06*TN(I,3) - 4.03952522e-09*TN(I,4) 
                + 8.33469780e-13*TN(I,5); 
            SMH(I,14) = 3.50840928e+00 + 1.43440860e+04*TI(I) 
                + 3.57953347e+00*TN(I,1) - 3.05176840e-04*TN(I,2) 
                + 1.69469055e-07*TN(I,3) + 7.55838237e-11*TN(I,4) 
                - 4.52212249e-14*TN(I,5);
            SMH(I,15) = 9.90105222e+00 + 4.83719697e+04*TI(I) 
                + 2.35677352e+00*TN(I,1) + 4.49229839e-03*TN(I,2) 
                - 1.18726045e-06*TN(I,3) + 2.04932518e-10*TN(I,4) 
                - 7.18497740e-15*TN(I,5);
        }
    }

    for (I=1; I<=VL; I++) 
    {
        if (T(I) <= 1.e3) 
        {
            SMH(I,16) = 3.39437243e+00 - 3.83956496e+03*TI(I) 
                + 4.22118584e+00*TN(I,1) - 1.62196266e-03*TN(I,2) 
                + 2.29665743e-06*TN(I,3) - 1.10953411e-09*TN(I,4) 
                + 2.16884433e-13*TN(I,5);
            SMH(I,17) = 6.02812900e-01 + 1.43089567e+04*TI(I) 
                + 4.79372315e+00*TN(I,1) - 4.95416685e-03*TN(I,2) 
                + 6.22033347e-06*TN(I,3) - 3.16071051e-09*TN(I,4) 
                + 6.58863260e-13*TN(I,5);
            SMH(I,18) = 1.31521770e+01 - 9.78601100e+02*TI(I) 
                + 2.10620400e+00*TN(I,1) + 3.60829750e-03*TN(I,2) 
                + 8.89745333e-07*TN(I,3) - 6.14803000e-10*TN(I,4) 
                + 1.03780500e-13*TN(I,5);
            SMH(I,19) = 1.39397051e+01 - 2.64289807e+04*TI(I) 
                + 8.08681094e-01*TN(I,1) + 1.16807815e-02*TN(I,2) 
                - 5.91953025e-06*TN(I,3) + 2.33460364e-09*TN(I,4) 
                - 4.25036487e-13*TN(I,5);
            SMH(I,20) = 5.92039100e+00 - 4.86217940e+04*TI(I) 
                + 3.28154830e+00*TN(I,1) + 3.48823955e-03*TN(I,2) 
                - 3.97587400e-07*TN(I,3) - 1.00870267e-10*TN(I,4) 
                + 4.90947725e-14*TN(I,5);
            SMH(I,21) = 8.51054025e+00 - 3.48598468e+04*TI(I) 
                + 3.21246645e+00*TN(I,1) + 7.57395810e-04*TN(I,2) 
                + 4.32015687e-06*TN(I,3) - 2.98048206e-09*TN(I,4) 
                + 7.35754365e-13*TN(I,5);
            SMH(I,22) = 4.09733096e+00 - 5.08977593e+03*TI(I) 
                + 3.95920148e+00*TN(I,1) - 3.78526124e-03*TN(I,2) 
                + 9.51650487e-06*TN(I,3) - 5.76323961e-09*TN(I,4) 
                + 1.34942187e-12*TN(I,5);
            SMH(I,23) = 4.70720924e+00 - 1.28416265e+04*TI(I) 
                + 4.30646568e+00*TN(I,1) - 2.09329446e-03*TN(I,2) 
                + 8.28571345e-06*TN(I,3) - 4.99272172e-09*TN(I,4) 
                + 1.15254502e-12*TN(I,5);
            SMH(I,24) = 2.66682316e+00 + 1.15222055e+04*TI(I) 
                + 4.29142492e+00*TN(I,1) - 2.75077135e-03*TN(I,2) 
                + 9.99063813e-06*TN(I,3) - 5.90388571e-09*TN(I,4) 
                + 1.34342886e-12*TN(I,5);
            SMH(I,25) = 1.24904170e+01 - 2.00594490e+04*TI(I) 
                + 2.25172140e+00*TN(I,1) + 8.82751050e-03*TN(I,2) 
                - 3.95485017e-06*TN(I,3) + 1.43964658e-09*TN(I,4) 
                - 2.53324055e-13*TN(I,5);
            SMH(I,26) = 1.22156480e+01 + 7.04291804e+03*TI(I) 
                + 2.13583630e+00*TN(I,1) + 9.05943605e-03*TN(I,2) 
                - 2.89912457e-06*TN(I,3) + 7.78664640e-10*TN(I,4) 
                - 1.00728807e-13*TN(I,5);
            SMH(I,27) = 9.57145350e+00 - 1.52147660e+03*TI(I) 
                + 3.40906240e+00*TN(I,1) + 5.36928700e-03*TN(I,2) 
                + 3.15248750e-07*TN(I,3) + 5.96548592e-10*TN(I,4) 
                + 1.43369255e-13*TN(I,5);
            SMH(I,28) = 4.10301590e+00 + 2.15728780e+04*TI(I) 
                + 4.72945950e+00*TN(I,1) - 1.59664290e-03*TN(I,2) 
                + 7.92248683e-06*TN(I,3) - 4.78821758e-09*TN(I,4) 
                + 1.09655560e-12*TN(I,5);
            SMH(I,29) = 1.71732140e+01 - 1.92456290e+04*TI(I) 
                + 1.36318350e+00*TN(I,1) + 9.90691050e-03*TN(I,2) 
                + 2.08284333e-06*TN(I,3) - 2.77962958e-09*TN(I,4) 
                + 7.92328550e-13*TN(I,5);
            SMH(I,30) = 1.61453400e+01 - 1.07482600e+03*TI(I) 
                + 1.49330700e+00*TN(I,1) + 1.04625900e-02*TN(I,2) 
                + 7.47799000e-07*TN(I,3) - 1.39076000e-09*TN(I,4) 
                + 3.57907300e-13*TN(I,5);
            SMH(I,31) = 2.11360340e+01 - 1.03123460e+04*TI(I) 
                + 1.04911730e+00*TN(I,1) + 1.30044865e-02*TN(I,2) 
                + 3.92375267e-07*TN(I,3) - 1.63292767e-09*TN(I,4) 
                + 4.68601035e-13*TN(I,5) ;
        }
    }
}
