/**
 * @author Alexander Breuer (alex.breuer AT uni-jena.de)
 *
 * @section LICENSE
 * Copyright 2020, Friedrich Schiller University Jena
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * @section DESCRIPTION
 * IO-routines for writing a snapshot as Comma Separated Values (CSV).
 **/
#include "NetCdf.h"

#define METER "m"
#define ERR(e) {printf("Error: %s\n", nc_strerror(e));}



tsunami_lab::io::NetCdf::NetCdf(t_idx i_nx,t_idx i_ny, t_real i_dxy){

  l_nx = i_nx;
  l_ny = i_ny;


  int x_dim, y_dim, time_dim;
  int retval;

  if ((retval = nc_create("solver.nc", NC_CLOBBER, &ncid)))
        ERR(retval);

  // define the dimensions.
  if ((retval = nc_def_dim(ncid, "x", l_nx, &x_dim)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "y", l_ny, &y_dim)))
    ERR(retval);
  if ((retval = nc_def_dim(ncid, "seconds since", NC_UNLIMITED, &time_dim)))
    ERR(retval);


  // define coordinates
  if ((retval = nc_def_var(ncid, "x", NC_DOUBLE, 1, &x_dim, &x_varid)))
    ERR(retval);
  if ((retval = nc_def_var(ncid, "y", NC_DOUBLE, 1, &y_dim, &y_varid)))
    ERR(retval);
  if ((retval = nc_def_var(ncid, "seconds since", NC_DOUBLE, 1, &time_dim, &time_varid)))
    ERR(retval);


  // assign units attributes to coordinate and time variables
  if ((retval = nc_put_att_text(ncid, x_varid, "units", strlen(METER), METER)))
    ERR(retval);
  if ((retval = nc_put_att_text(ncid, y_varid, "units", strlen(METER), METER)))
    ERR(retval);
  if ((retval = nc_put_att_text(ncid, time_varid, "units", strlen("s"), "s")))
    ERR(retval);

  // dims array is used to pass dimension of variables
  int dims[3];
  dims[0] = time_dim;
  dims[1] = x_dim;
  dims[2] = y_dim;

  //define other variables
  if ((retval = nc_def_var(ncid, "height", NC_DOUBLE, 3, dims, &h_varid)))
    ERR(retval);
  if ((retval = nc_def_var(ncid, "momentum_x", NC_DOUBLE, 3, dims, &hu_varid)))
    ERR(retval);
  if ((retval = nc_def_var(ncid, "momentum_y", NC_DOUBLE, 3, dims, &hv_varid)))
    ERR(retval);

  // assign units attributes to coordinate and time variables
  if ((retval = nc_put_att_text(ncid, h_varid, "units", strlen(METER), METER)))
    ERR(retval);
  if ((retval = nc_put_att_text(ncid, hu_varid, "units", strlen(METER), METER)))
    ERR(retval);
  if ((retval = nc_put_att_text(ncid, hv_varid, "units", strlen(METER), METER)))
    ERR(retval);

  //end define mode
  if ((retval = nc_enddef(ncid)))
    ERR(retval);

  // derive coordinates of cell center
  t_real *l_posX = new t_real[i_nx];
  t_real *l_posY = new t_real[i_ny];
  for( t_idx l_iy = 0; l_iy < i_ny; l_iy++ ) {
    for( t_idx l_ix = 0; l_ix < i_nx; l_ix++ ) {
      l_posX[l_ix] = (l_ix + 0.5) * i_dxy;
      l_posY[l_iy] = (l_iy + 0.5) * i_dxy;
    }
  }

  // write the coordinate variable data
  if ((retval = nc_put_var_float(ncid, x_varid, &l_posX[0])))
    ERR(retval);
  if ((retval = nc_put_var_float(ncid, y_varid, &l_posY[0])))
    ERR(retval);
}

tsunami_lab::io::NetCdf::~NetCdf(){
  int retval;
  // close the file
   if ((retval = nc_close(ncid)))
      ERR(retval);
}

void tsunami_lab::io::NetCdf::write(
                                  t_idx  i_stride,
                                  t_real const * i_h,
                                  t_real  const * i_hu,
                                  t_real  const * i_hv,
                                  t_real const * ,
                                  t_idx i_timeStep,
                                  t_real i_simTime) {



  t_real *l_h = new t_real[l_nx*l_ny];
  t_real *l_hu = new t_real[l_nx*l_ny];
  t_real *l_hv = new t_real[l_nx*l_ny];
  int retval;

  // iterate over all cells
  for( t_idx l_iy = 0; l_iy < l_ny; l_iy++ ) {
    for( t_idx l_ix = 0; l_ix < l_nx; l_ix++ ) {
      t_idx l_id_from = l_iy * i_stride + l_ix;
      t_idx l_id_to = l_iy * l_nx + l_ix;
      l_h[l_id_to]= i_h[l_id_from];
      l_hu[l_id_to]= i_hu[l_id_from];
      l_hv[l_id_to]= i_hv[l_id_from];
    }
  }

  size_t start[3], count[3];


  count[0] = 1;
  count[1] = l_nx;
  count[2] = l_ny;
  start[1] = 0;
  start[2] = 0;

  start[0] = i_timeStep;
  //write time since start
  if ((retval = nc_put_vara_float(ncid, time_varid, start, count, &i_simTime)))
    ERR(retval);


  // write the computed data.

  if ((retval = nc_put_vara_float(ncid, h_varid, start, count, &l_h[0])))
    ERR(retval);
  if ((retval = nc_put_vara_float(ncid, hu_varid, start, count, &l_hu[0])))
    ERR(retval);

  if ((retval = nc_put_vara_float(ncid, hv_varid, start, count, &l_hv[0])))
    ERR(retval);
  std::cout <<i_simTime<<std::endl;

  }