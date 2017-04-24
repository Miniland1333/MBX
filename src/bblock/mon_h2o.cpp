#include "mon_h2o.h"

////////////////////////////////////////////////////////////////////////////////

namespace bblock { // Building Block :: System

////////////////////////////////////////////////////////////////////////////////

H2O::H2O() {
}

H2O::H2O(double * coords, std::vector<std::string> names) {
  n_real_sites = 3;
  n_virt_sites = 1;

  n_sites = n_real_sites + n_virt_sites;

  xyz = std::shared_ptr<double> (new double[n_sites * 3],
           []( double *p ) { delete[] p; });
  
  at_names.clear();
  
  for (size_t i = 0; i < n_real_sites; i++) {
    at_names.push_back(names[i]);
  }

  for (size_t i = n_real_sites; i < n_sites; i++) {
    at_names.push_back("virt");
  }

  mon_id = "h2o";

  std::copy(coords, coords + n_real_sites * 3, xyz.get() );
  std::fill(coords + n_real_sites * 3 , coords + n_sites * 3 , 0.0);
}

H2O::~H2O() {
}

////////////////////////////////////////////////////////////////////////////////

} // Building Block :: System

////////////////////////////////////////////////////////////////////////////////