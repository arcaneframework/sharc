//
// Created by scravagl on 1/18/23.
//

#include <vector>

#include "gtest/gtest.h"
#include "../../src/TwoPhaseFlowSimulation/PhysicalLaws/RelativePermeabilityPowerLaw/RelativePermeabilityPowerLaw.h"
#include <mipp.h>
#include <iostream>

#define N_TEST 16

TEST(RelativePermeabilityPowerLaw, OptimizedScalarLaw){
    // Create arrays of input and output for the law
    std::vector<Real> Sw(N_TEST);
    for (int i = 0; i<N_TEST; i++) {
        Sw[i] = (Real)i/(Real)N_TEST;
    }
    std::vector<Real> krw_ref(N_TEST);
    std::vector<Real> dkrw_dSw_ref(N_TEST);
    std::vector<Real> krw(N_TEST);
    std::vector<Real> dkrw_dSw(N_TEST);

    // Create and parametrize law
    RelativePermeabilityPowerLaw law = RelativePermeabilityPowerLaw();
    law.setParameters(0.1,0.2,2);

    // Evaluate reference
    for (int i = 0; i<N_TEST; i++) {
        law.eval_orig(Sw[i], krw_ref[i], dkrw_dSw_ref[i]);
    }
    // Evaluate scalar
    for (int i = 0; i<N_TEST; i++) {
        law.eval(Sw[i], krw[i], dkrw_dSw[i]);
    }
    for (int i = 0; i<N_TEST; i++) {
        ASSERT_DOUBLE_EQ(krw[i], krw_ref[i]);
        ASSERT_DOUBLE_EQ(dkrw_dSw[i], dkrw_dSw_ref[i]);
    }
}

TEST(RelativePermeabilityPowerLaw, SIMDLaw){
    // Create arrays of input and output for the law
    std::vector<Real> Sw(N_TEST);
    for (int i = 0; i<N_TEST-1; i++) {
        Sw[i] = (Real)i/(Real)N_TEST;
    }
    std::vector<Real> krw_ref(N_TEST);
    std::vector<Real> dkrw_dSw_ref(N_TEST);
    std::vector<Real> krw(N_TEST);
    std::vector<Real> dkrw_dSw(N_TEST);

    // Create and parametrize law
    RelativePermeabilityPowerLaw law = RelativePermeabilityPowerLaw();
    law.setParameters(0,0,2);

    // Evaluate reference
    for (int i = 0; i<N_TEST; i++) {
        law.eval_orig(Sw[i], krw_ref[i], dkrw_dSw_ref[i]);
    }
    // Evaluate vector
    int nb_loop = N_TEST/mipp::N<Real>();
    mipp::Reg<Real> Sw_reg, krw_reg, dkrw_dSw_reg;
    for (int i = 0; i<nb_loop; i++) {
        Sw_reg = &Sw[i * mipp::N<Real>()];
        law.evalVect(Sw_reg, krw_reg, dkrw_dSw_reg);
        krw_reg.store(&krw[i*mipp::N<Real>()]);
        dkrw_dSw_reg.store(&dkrw_dSw[i*mipp::N<Real>()]);
    }
    for (int i = 0; i<N_TEST; i++) {
        ASSERT_DOUBLE_EQ(krw[i], krw_ref[i]);
        ASSERT_DOUBLE_EQ(dkrw_dSw[i], dkrw_dSw_ref[i]);
    }
}
