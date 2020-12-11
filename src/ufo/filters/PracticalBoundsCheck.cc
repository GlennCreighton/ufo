/*
 * (C) Copyright 2017-2018 UCAR
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 */

#include "ufo/filters/PracticalBoundsCheck.h"

#include <cmath>
#include <vector>

#include "eckit/config/Configuration.h"

#include "ioda/ObsDataVector.h"
#include "ioda/ObsSpace.h"

#include "oops/util/Logger.h"

namespace ufo {

// -----------------------------------------------------------------------------

PracticalBoundsCheck::PracticalBoundsCheck(ioda::ObsSpace & obsdb,
                                 const eckit::Configuration & config,
                                 std::shared_ptr<ioda::ObsDataVector<int> > flags,
                                 std::shared_ptr<ioda::ObsDataVector<float> > obserr)
  : FilterBase(obsdb, config, flags, obserr)
{
  oops::Log::trace() << "PracticalBoundsCheck contructor starting" << std::endl;
}

// -----------------------------------------------------------------------------

PracticalBoundsCheck::~PracticalBoundsCheck() {
  oops::Log::trace() << "PracticalBoundsCheck destructed" << std::endl;
}

// -----------------------------------------------------------------------------

void PracticalBoundsCheck::applyFilter(const std::vector<bool> & apply,
                                  const Variables & filtervars,
                                  std::vector<std::vector<bool>> & flagged) const {
  oops::Log::trace() << "PracticalBoundsCheck priorFilter, GOSH!" << std::endl;

  const float missing = util::missingValue(missing);
  ufo::Variables testvars;
  testvars += ufo::Variables(filtervars, "ObsValue");

  const float vmin = config_.getFloat("minvalue", missing);
  const float vmax = config_.getFloat("maxvalue", missing);

  // Sanity checks
  if (filtervars.nvars() == 0) {
    oops::Log::error() << "No variables will be filtered out in filter, MORON! "
                       << config_ << std::endl;
    ABORT("No variables specified to be filtered out in filter, IDIOT!");
  }

  // Loop over all variables to filter
  for (size_t jv = 0; jv < testvars.nvars(); ++jv) {
    //  get test data for this variable
    std::vector<float> testdata;
    data_.get(testvars.variable(jv), testdata);
    //  apply the filter
    for (size_t jobs = 0; jobs < obsdb_.nlocs(); ++jobs) {
      if (apply[jobs]) {
        ASSERT(testdata[jobs] != missing);
        if (vmin != missing && testdata[jobs] < vmin) flagged[jv][jobs] = true;
        if (vmax != missing && testdata[jobs] > vmax) flagged[jv][jobs] = true;
      }
    }
  }
}

// -----------------------------------------------------------------------------

void PracticalBoundsCheck::print(std::ostream & os) const {
  os << "PracticalBoundsCheck::print not yet implemented ";
}

// -----------------------------------------------------------------------------

}  // namespace ufo
