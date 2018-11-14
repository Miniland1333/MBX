#ifndef ELECTROSTATICS_H
#define ELECTROSTATICS_H

#include <vector>
#include <cstdlib>
#include <string>
#include <cmath>
#include <memory>

#ifdef _OPENMP
#include <omp.h>
#endif

#include "bblock/sys_tools.h"
#include "tools/definitions.h"
#include "tools/constants.h"
#include "tools/math_tools.h"
#include "potential/electrostatics/gammq.h"
#include "potential/electrostatics/fields.h"
#include "potential/electrostatics/electrostatic_tensors.h"
#include "potential/electrostatics/electrostatic_tensors_short.h"

#include "kdtree_utils.h"

////////////////////////////////////////////////////////////////////////////////

/**
 * @file electrostatics.h
 * @brief Contains the electrostatics class declaration, along
 * with all its member functions and variables.
 */

/**
 * @namespace elec
 * @brief Namespace that includes all the electrostatic related functions
 * and classes
 */
namespace elec {

class Electrostatics {
   public:
    /**
     * @brief Default constructor of the class. Does nothing.
     */
    Electrostatics();

    /**
     * @brief Initializes the electrostatics class with the system information
     *
     * Given the information of the system, such as charges, coordinates,
     * polarizabilities, ... of the system, it initializes the class,
     * reordering in the proper way the variables that need to be reordered.
     * @param[in] chg Charges of the system
     * @param[in] chg_grad Charge gradients previously obtained when
     * the charges were set in the system
     * @param[in] polfac Vector of polarizability factors of all atoms the system
     * @param[in] pol Vector of polarizabilities of all atoms the system
     * @param[in] sys_xyz Vector of coordinates of all atoms of the system
     * @param[in] mon_id Vector with the id of all the monomers in the system
     * @param[in] sites Vector with the number of sites of all monomers int he system
     * @param[in] first_ind Vector with the index in the atom list of the
     * first atom of each monomer
     * @param[in] mon_type_count Vector of pairs. First is the id, second is
     * the number of monomers of that id.
     * @param[in] do_grads If true, grads will be computed
     * @param[in] tolerance Maximum error per dipole to assume convergence in
     * iterative methods
     * @param[in] maxit Maximum number of iterations of the dipole calculation
     * @param[in] dip_method Method to use to compute the dipoles
     * @param[in] box Vector of 9 components with the box. The elements are:
     * {v1x,v1y,v1z,v2x,v2y,v2z,v3x,v3y,v3z} where v1, v2, and v3 are the
     * trhee main vectors of the box
     */
    void Initialize(const std::vector<double> &chg, const std::vector<double> &chg_grad,
                    const std::vector<double> &polfac, const std::vector<double> &pol,
                    const std::vector<double> &sys_xyz, const std::vector<std::string> &mon_id,
                    const std::vector<size_t> &sites, const std::vector<size_t> &first_ind,
                    const std::vector<std::pair<std::string, size_t> > &mon_type_count, const bool do_grads = true,
                    const double tolerance = 1E-16, const size_t maxit = 100, const std::string dip_method = "iter",
                    const std::vector<double> &box = {});

    /**
     * @brief Gets the electrostatic energy
     *
     * Once the system has been initialized, computes the electrostatic energy
     * of the system.
     * @param[out] grad Gradients will be saved here
     * @return Total electrostatic energy
     */
    double GetElectrostatics(std::vector<double> &grad);

    /**
     * @brief Clears the ASPC history
     *
     * If ASPC is not being used, will reset the dipole history anyways.
     * It basically clears out the dipole history vector. Then it is forced to
     * recalculate the dipoles iteratively to get a new history.
     */
    void ResetAspcHistory();

    /**
     * @brief "Reinitializes" the electrostatics class.
     *
     * If the system changes (coordinates, charges, and so on) it is
     * needed to reenter the information to the system, and it is done
     * with this function.
     * @param[in] xyz Vector of coordinates of all atoms of the system
     * @param[in] chg Charges of the system
     * @param[in] chg_grad Charge gradients previously obtained when
     * the charges were set in the system
     * @param[in] pol Vector of polarizabilities of all atoms the system
     * @param[in] polfac Vector of polarizability factors of all atoms the system
     * @param[in] dip_method Method to use to compute the dipoles
     * @param[in] do_grads If true, grads will be computed
     * @param[in] box Vector of 9 components with the box. The elements are:
     * {v1x,v1y,v1z,v2x,v2y,v2z,v3x,v3y,v3z} where v1, v2, and v3 are the
     * trhee main vectors of the box
     * @param[in] use_pbc Boolean specifying if PBC are going to be used or not
     */
    void SetNewParameters(const std::vector<double> &xyz, const std::vector<double> &chg,
                          const std::vector<double> &chg_grad, const std::vector<double> &pol,
                          const std::vector<double> &polfac, const std::string dip_method, const bool do_grads,
                          const std::vector<double> &box = {});

    /**
     * @brief Sets the cutoff for electrostatic interactions
     *
     * @param[in] cutoff New cutoff value
     */
    void SetCutoff(double cutoff);

   private:
    void CalculatePermanentElecField();
    void CalculateDipolesIterative();
    void DipolesIterativeIteration();
    void CalculateDipolesCG();
    void DipolesCGIteration(std::vector<double> &in_v, std::vector<double> &out_v);
    void CalculateDipolesAspc();
    void SetAspcParameters(size_t k);
    void CalculateDipoles();
    void CalculateElecEnergy();
    void CalculateGradients(std::vector<double> &grad);

    void ReorderData();

    // Charges of each site. Order has to follow mon_type_count.
    std::vector<double> chg_;
    // Charges of each site. Order has to follow mon_type_count.
    std::vector<double> sys_chg_;
    // Gradients due to site dependent charges
    std::vector<double> sys_chg_grad_;
    // Polfacs of each site. For now assuming not site dependent.
    std::vector<double> polfac_;
    // Square root of pol of each site. Size 3N (3 per atom, are the same)
    std::vector<double> pol_sqrt_;
    // Polarizabilities of each site. For now assuming not site dependent.
    std::vector<double> pol_;
    // System xyz, not ordered XYZ. xyzxyz...(mon1)xyzxyz...(mon2) ...
    std::vector<double> sys_xyz_;
    // System xyz, ordered XYZ. xx..yy..zz(mon1) xx..yy..zz(mon2) ...
    std::vector<double> xyz_;
    // Name of the monomers (h2o, f...)
    std::vector<std::string> mon_id_;
    // Number of sites of each mon
    std::vector<size_t> sites_;
    // First index of each atom in system
    std::vector<size_t> first_ind_;
    // Vector that contains all different monomer types and the number of
    // monomers of each type.
    std::vector<std::pair<std::string, size_t> > mon_type_count_;
    // Tolerance in the iterative calculation of the dipoles
    // Tolerance refers to the maximum squared difference overall the dipoles
    double tolerance_;
    // Maximum number of iterations allowed in the iterative dipole calculation
    size_t maxit_;
    // Bool that if true will perform the gradients calculation.
    bool do_grads_;
    // Gradients in the original order (same as xyz). This is the vector in
    // which the electrostatics gradients will be added.
    std::vector<double> sys_grad_;
    // Gradients
    std::vector<double> grad_;
    // Electric potential on each site with sys order
    std::vector<double> sys_phi_;
    // Electric potential on each site
    std::vector<double> phi_;
    // Permanent electric field with sys order
    std::vector<double> sys_Efq_;
    // Permanent electric field
    std::vector<double> Efq_;
    // Induced electric field with sys_order
    std::vector<double> sys_Efd_;
    // Induced electric field
    std::vector<double> Efd_;
    // Dipoles with sys order
    std::vector<double> sys_mu_;
    // Dipoles
    std::vector<double> mu_;
    // Dipole history for ASPC
    std::vector<double> mu_hist_;
    // Dipole predictor
    std::vector<double> mu_pred_;
    // B constants to use for ASPC
    std::vector<double> b_consts_aspc_;
    // omega used in ASPC
    double omega_aspc_;
    // Number of history steps stored
    size_t hist_num_aspc_;
    // Order of ASPC
    size_t k_aspc_;
    // Total number of electrostatic sites
    size_t nsites_;
    // Thole dampings
    double aCC_, aCD_, aDD_;
    // Constants to be used later:
    // Gamma 3/4
    double g34_;
    // aCC ^ (1/4)
    double aCC1_4_;
    // Max number of monomers
    size_t maxnmon_;
    // Permanent electrostatics
    double Eperm_;
    // Induced electrostatics
    double Eind_;
    // Method for dipoles (ITERative, Conjugate Gradient, ASPC, INVersion)
    std::string dip_method_;
    // box of the system
    std::vector<double> box_;
    // use pbc in the electrostatics calculation
    bool use_pbc_;
    // electrostatics cutoff
    double cutoff_;
};

////////////////////////////////////////////////////////////////////////////////

}  // namespace elec

////////////////////////////////////////////////////////////////////////////////

#endif