/**
 * @author Julius Isken, Max Engel
 *
 * @section LICENSE
 * Copyright 2020, Julius Isken, Max Engel
 *
 * Redistribution and use in source and binary forms, with or without
 *modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *this list of conditions and the following disclaimer in the documentation
 *and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 *FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 *DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 *SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 *OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 * one-dimensional shock shock problem
 **/
#include "TsunamiEvent.h"

#include <cmath>

// TODO: split netdcf class into init, read and write, so we avoid redundant
// computation
tsunami_lab::setups::TsunamiEvent::TsunamiEvent(t_idx i_nx) {
  l_nx = i_nx;
  l_netcdf = new tsunami_lab::io::NetCdf(l_nx, "bathymetry_data.nc",
                                         "displacement_data.nc");
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent::getHeight(
    t_real i_x, t_real i_y) const {
  t_real height;
  height = l_netcdf->read_bathymetry(i_x, i_y);
  height *= -1;
  return height;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent::getMomentumX(
    t_real, t_real) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent::getMomentumY(
    t_real, t_real) const {
  return 0;
}

tsunami_lab::t_real tsunami_lab::setups::TsunamiEvent::getBathymetry(
    t_real i_x, t_real i_y) const {
  t_real bath;
  bath = l_netcdf->read_bathymetry(i_x, i_y);
  bath += l_netcdf->read_displacement(i_x, i_y);
  return bath;
}