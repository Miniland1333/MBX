#ifndef MBNRG_1B_A1B3_DEG6_V1_H
#define MBNRG_1B_A1B3_DEG6_V1_H 

#include <cmath>
#include <string>
#include <vector>

#include "tools/constants.h"
#include "tools/variable.h"
#include "tools/water_monomer_lp.h"
#include "poly_1b_A1B3_deg6_v1.h" 

////////////////////////////////////////////////////////////////////////////////

namespace mbnrg_A1B3_deg6 {

//----------------------------------------------------------------------------//

struct mbnrg_A1B3_deg6_v1 {
    mbnrg_A1B3_deg6_v1() {};
    mbnrg_A1B3_deg6_v1(const std::string mon1);

    ~mbnrg_A1B3_deg6_v1() {};

    typedef poly_A1B3_deg6_v1 polynomial;

    std::vector<double> eval(const double *xyz1, const size_t n);
    std::vector<double> eval(const double *xyz1, double *grad1 , const size_t n,std::vector<double> *virial = 0);

  private:
    double m_k_x_intra_A_B_1;
    double m_k_x_intra_B_B_1;

    double m_ri = 7.0;
    double m_ro = 8.0;

    double f_switch(const double, double&);

    std::vector<double> coefficients;
};

//----------------------------------------------------------------------------//

} // namespace mbnrg_A1B3_deg6

////////////////////////////////////////////////////////////////////////////////

#endif 
