// -*- tab-width: 2; indent-tabs-mode: nil; coding: utf-8-with-signature -*-
//-----------------------------------------------------------------------------
// Copyright 2000-2022 CEA (www.cea.fr) IFPEN (www.ifpenergiesnouvelles.com)
// See the top-level COPYRIGHT file for details.
// SPDX-License-Identifier: Apache-2.0
//-----------------------------------------------------------------------------
// -*- C++ -*-
/* Author : encheryg at Fri Feb  3 18:27:33 2017
 * Generated by createNew
 */

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/
#include <memory>

#include "DiPhasicTest/Model/ILawConfig.h"
#include "DiPhasicTest/Model/ILawProperty.h"
#include "CubicSpline.h"

// generated from VanGenuchten.law
#include "VanGenuchten_law.h"

#include "VanGenuchtenCapPressLaw_axl.h"

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

class VanGenuchtenCapPressLawService 
  : public ArcaneVanGenuchtenCapPressLawObject
{
public:
  
  VanGenuchtenCapPressLawService(const Arcane::ServiceBuildInfo & sbi) 
    : ArcaneVanGenuchtenCapPressLawObject(sbi) {}
  
  ~VanGenuchtenCapPressLawService() {}
  
public:

  void initialize(const Law::ScalarRealProperty& p, Law::FunctionManager& law_register) ;

  struct Algo {
  public:
    
    Algo() : m_cslow(nullptr), m_cshigh(nullptr) {}

  private:
    void _compute(const Arcane::Real value, Arcane::Real& result) const {
      const Arcane::Real value2((m_other) ? 1. - value : value);
      const Arcane::Real truncS = (value2 < m_swr) ? 0. : ((value2 > 1 - m_snr) ? 1. : (value2 - m_swr)/(1 - m_snr - m_swr)) ;

      if(truncS <= m_selow) {
        m_cslow -> eval(truncS, result) ;
      } else if (truncS >= m_sehigh) {
        m_cshigh -> eval(truncS, result) ;
      } else {
        result = 1. / m_alphavg * pow(pow(truncS, -1. / m_mvg) - 1., 1. - m_mvg) ;
      }
    }

  public:
    void compute(const Arcane::Real value, Arcane::Real& result, Arcane::Real& deriv) const {
      const Arcane::Real value2((m_other) ? 1. - value : value);

      if(value2 < m_swr || value2 > 1. - m_snr) {
        _compute(value, result) ;
        deriv = 0. ;
      } else {
        const Arcane::Real normS((value2 - m_swr) / (1 - m_snr - m_swr)) ;
        const Arcane::Real sign((m_other) ? -1. : 1.0) ;
        const Arcane::Real dNormS(sign / (1 - m_snr - m_swr)) ;

        if(normS <= m_selow) {
          m_cslow -> eval(normS, result, deriv) ;
          deriv *= dNormS ;
        } else if (normS >= m_sehigh) {
          m_cshigh -> eval(normS, result, deriv) ;
          deriv *= dNormS ;
        } else {
          _compute(value, result) ;
          deriv = 1. / m_alphavg * (1. - 1. / m_mvg) * (pow(pow(normS, -1. / m_mvg) - 1., - m_mvg)) *
            pow(normS, -1. / m_mvg -1.) * dNormS ;
        }
      }
    }

    void eval(const Arcane::Real value, Arcane::Real& result) const {
    	const Arcane::Real truncS = (value < m_swr) ? 0. : ((value > 1 - m_snr) ? 1. : (value - m_swr)/(1 - m_snr - m_swr)) ;
    	result = 1. / m_alphavg * pow(pow(truncS, -1. / m_mvg) - 1., 1. - m_mvg) ;
    }

    void eval(const Arcane::Real value, Arcane::Real& result, Arcane::Real& deriv) const {
      eval(value, result) ;

      if(value < m_swr || value > 1. - m_snr) {
        deriv = 0. ;
      } else {
        const Arcane::Real normS((value - m_swr) / (1 - m_snr - m_swr)) ;
        const Arcane::Real dNormS(1. / (1 - m_snr - m_swr)) ;
        deriv = 1. / m_alphavg * (1. - 1. / m_mvg) * (pow(pow(normS, -1. / m_mvg) - 1., - m_mvg)) *
          pow(normS, -1. / m_mvg -1.) * dNormS ;
      }
    }

    Arcane::Real m_swr ;
    Arcane::Real m_snr ;
    Arcane::Real m_selow ;
    Arcane::Real m_sehigh ;
    Arcane::Real m_mvg ;
    Arcane::Real m_alphavg ;
    bool m_other ;
    std::unique_ptr<CubicSpline> m_cslow, m_cshigh ;
  };

private:
  Algo m_algo ;
};

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

void 
VanGenuchtenCapPressLawService::
initialize(const Law::ScalarRealProperty& p, Law::FunctionManager& law_register)
{
  const Arcane::Real selow(options() -> selow()) ;

   if(selow <= 0. || selow >= 1.)
    fatal() << "selow should belong to (0,1)" ; 

  const Arcane::Real sehigh(options() -> sehigh()) ;

  if(sehigh <= 0. || sehigh >= 1.)
    fatal() << "sehigh should belong to (0,1)" ;

  m_algo.m_swr = options() -> swr() ;
  m_algo.m_snr = options() -> snr() ;
  m_algo.m_selow = selow ;
  m_algo.m_sehigh = sehigh ;
  m_algo.m_mvg = options() -> mvg() ;
  m_algo.m_alphavg = options() -> alphavg() ;
  m_algo.m_other = options() -> otherPhaseDependency() ;

  VanGenuchten::Signature s;
  s.saturation = options() -> dependency() -> getProperty() ;
  s.kr_or_pc = p;
  auto f = std::make_shared<VanGenuchten::Function>(s, m_algo, &Algo::compute);
  law_register << f;

  Arcane::Real valueAtSelow(0.) ;
  Arcane::Real valueAtSelowDivBy10(0.) ;
  Arcane::RealUniqueArray derivAtSelow(1, 0.) ;
  m_algo.eval(selow, valueAtSelow, derivAtSelow[0]) ;
  m_algo.eval(selow/10., valueAtSelowDivBy10) ;
  m_algo.m_cslow.reset(new CubicSpline(0., selow, valueAtSelowDivBy10, valueAtSelow, derivAtSelow[0], derivAtSelow[0])) ;

  Arcane::Real valueAtSehigh(0.) ;
  Arcane::RealUniqueArray derivAtSehigh(1, 0.) ;
  m_algo.eval(sehigh, valueAtSehigh, derivAtSehigh[0]) ;
  m_algo.m_cshigh.reset(new CubicSpline(sehigh, 1.0, valueAtSehigh, 0.0, derivAtSehigh[0], derivAtSehigh[0])) ;
}

/*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/

using namespace Arcane;
ARCANE_REGISTER_SERVICE_VANGENUCHTENCAPPRESSLAW(VanGenuchtenCapPressLaw,VanGenuchtenCapPressLawService);
