/*
 * (C) Copyright 2017-2018 UCAR
 * 
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
 */

#include "ufo/atmosphere/radiosonde/ObsRadiosonde.h"

#include <ostream>
#include <string>
#include <vector>
#include <boost/algorithm/string.hpp>

#include "oops/util/Logger.h"

#include "ioda/ObsVector.h"

#include "ufo/GeoVaLs.h"
#include "ufo/ObsBias.h"

namespace ufo {

// -----------------------------------------------------------------------------
static ObsOperatorMaker<ObsRadiosonde> makerRadiosonde_("Radiosonde");
// -----------------------------------------------------------------------------

ObsRadiosonde::ObsRadiosonde(const ioda::ObsSpace & odb, const eckit::Configuration & config)
  : keyOperRadiosonde_(0), odb_(odb), varin_(), varout_()
{
  const eckit::Configuration * configc = &config;
  ufo_radiosonde_setup_f90(keyOperRadiosonde_, &configc);

  // Read in vout list from configuration
  varout_.reset(new oops::Variables(config));

  /* Read in from C++
  // Decide the vin based on vout
  // We always need vertical coordinates
  std::vector<std::string> vv{"atmosphere_ln_pressure_coordinate"};

  for (std::size_t ii=0; ii < varout_->variables().size(); ++ii) {
    // To be revisited here, it should not be hard-wired.
    if (varout_->variables()[ii] == "air_temperature")
      vv.push_back("virtual_temperature");
    else
      vv.push_back(varout_->variables()[ii]);
  }
  varin_.reset(new oops::Variables(vv));
  */

  // Read in from Fortran demonstration
  std::vector<std::string> vvin, vvout;
  this->get_vars_from_f90(config, vvin, vvout);
  varin_.reset(new oops::Variables(vvin));
  varout_.reset(new oops::Variables(vvout));

  oops::Log::trace() << "ObsRadiosonde created." << std::endl;
}

// -----------------------------------------------------------------------------

ObsRadiosonde::~ObsRadiosonde() {
  ufo_radiosonde_delete_f90(keyOperRadiosonde_);
  oops::Log::trace() << "ObsRadiosonde destructed" << std::endl;
}

// -----------------------------------------------------------------------------

void ObsRadiosonde::simulateObs(const GeoVaLs & gom, ioda::ObsVector & ovec,
                                const ObsBias & bias) const {
  ufo_radiosonde_simobs_f90(keyOperRadiosonde_, gom.toFortran(), odb_,
                            ovec.size(), ovec.toFortran(), bias.toFortran());
}

// -----------------------------------------------------------------------------
  void ObsRadiosonde::get_vars_from_f90(const eckit::Configuration & config,
                                        std::vector<std::string> & vvin,
                                        std::vector<std::string> & vvout) {
  int c_name_size = 200;
  char *buffin = new char[c_name_size];
  char *buffout = new char[c_name_size];
  const eckit::Configuration * configc = &config;
  ufo_radiosonde_getvars_f90(keyOperRadiosonde_, &configc, buffin, buffout, c_name_size);

  std::string vstr_in(buffin), vstr_out(buffout);
  boost::split(vvin, vstr_in, boost::is_any_of("\t"));
  boost::split(vvout, vstr_out, boost::is_any_of("\t"));
}

// -----------------------------------------------------------------------------

void ObsRadiosonde::print(std::ostream & os) const {
  os << "ObsRadiosonde::print not implemented";
}

// -----------------------------------------------------------------------------

}  // namespace ufo
