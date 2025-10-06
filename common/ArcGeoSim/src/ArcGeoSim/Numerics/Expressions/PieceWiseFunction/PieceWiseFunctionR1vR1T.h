#ifndef ARCGEOSIM_NUMERICS_EXPRESSIONS_PIECEWISEFUNCTIONR1VR1_H
#error "Template implementation has to be used with template declaration"
#endif 

#ifdef WIN32
#include <ciso646>
#endif

/*---------------------------------------------------------------------------*/

#include <arcane/ArcaneVersion.h>
#include "ArcGeoSim/Utils/ArrayUtils.h"

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::init()
    {
      //! Check Table is filled : table must be filled before init()
      //!       done either by the Service (data read from mydata.arc) :
      //!               MyInterpolationPieceWiseFunctionService->init()
      //!       either manually :
      //!               PieceWiseFunctionR1vR1T<MyInterpolator> my_pwf(traceMng());
      //!               my_pwf.setBorderCondition();
      //!               my_pwf.setPoint(x,y);...
      //!               my_pwf.init(); // Object ready for use
      if (m_initialized)
        return;
      if (m_filled == false)
        m_trace_mng->fatal()
            << "Cannot initialize PieceWiseFunctionR1vR1T<MyInterpolator> before filling table";
      m_initialized = true;
    }

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR1vR1T<T>::setVariable(const String & name,
		const Array<Real> & variable)
		{
	if (name != m_name)
	{
		m_trace_mng->fatal()
              		<< "Cannot set variable data for undefined variable '" << name
              		<< "'";
	}
	m_variable.setArray(variable);
		}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR1vR1T<T>::setVariable(Integer index,
		const Array<Real> & variable)
		{
	if (index != 0)
	{
		m_trace_mng->fatal()
              		<< "Cannot set variable data for undefined variable position "
              		<< index;
	}
	m_variable.setArray(variable);
		}

/*---------------------------------------------------------------------------*/

template<typename T>
  String
  PieceWiseFunctionR1vR1T<T>::variableName(Integer index) const
    {
      if (index != 0)
        {
          m_trace_mng->fatal() << "Variable index " << index
              << " not defined for PieceWiseFunctionR1vR1T<T>";
        }
      return m_name;
    }

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR1vR1T<T>::setEvaluationResult(Array<Real> & result)
{
	m_eval_result.setArray(result);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR1vR1T<T>::setDerivationResult(Integer di, Array<Real> & result)
{
	if (di != 0)
	{
		m_trace_mng->fatal()
              		<< "Cannot derive following undefined variable index " << di;
	}
	m_diff_result.setArray(result);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR1vR1T<T>::setDerivationResult(const String & di,
		Array<Real> & result)
		{
	if (di != m_name)
	{
		m_trace_mng->fatal()
              		<< "Cannot derive following undefined variable '" << m_name
              		<< "'";
	}
	m_diff_result.setArray(result);
		}

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::cleanup()
    {
      m_variable = RealConstArrayView();
      m_eval_result = RealArrayView();
      m_diff_result = RealArrayView();
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::setVariableName(const String & name)
    {
      m_name = name;
    }

/*---------------------------------------------------------------------------*/

template<typename T>
void PieceWiseFunctionR1vR1T<T>::setPoint(const Real & x, const Real & y) {

	auto itx = find(m_x.begin(), m_x.end(), x);
	if ((itx != m_x.end())) {
		if ((m_y[itx - m_x.begin()] != y)){
			if (x==0 && m_y[itx - m_x.begin()]==0){
				m_y[itx - m_x.begin()] =y ;

			}else{

			throw FatalErrorException(String::format(
					"The x component is specified for different y ({0},{1},{2}) ", x,y,m_y[itx - m_x.begin()]));
			}
		}
	}else {
	//! Tests
	if (!m_boundary_defined)
		m_trace_mng->pinfo()
				<< "Table Boundary condition must be set before filling";

	if (m_border_type == BorderType::FiniteProlongation and (x <= m_lower_limit
			or x >= m_upper_limit)) {
		m_trace_mng->fatal()
				<< "Point inserted in the Table must belong to ]lower_limit,upper_limit[";
	}

	//! Store x and y min and max values
	if (m_filled == false) {
		m_x_min = x;
		m_x_max = x;
		m_y_min = y;
		m_y_max = y;
		m_filled = true;
	} else {
		m_x_min = math::min(m_x_min, x);
		m_x_max = math::max(m_x_max, x);
		m_y_min = math::min(m_y_min, y);
		m_y_max = math::max(m_y_max, y);
	}

	//! Add point in data m_x m_y and create a new interval in m_interval
	addTablePoint(x, y);

	//! Sort
	sort();

	//! Update table point ghost cells : boundary for interval coefficient evaluation
	updateGhostPoints();

	//! obtain intervals created or modified with point insertion
	getConnectedIntervals();

	//! Compute interval coefficient(s) for both intervals
	computeIntervalCoef();
	}
}

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::printPoints() const
    {
      //! Check initialization
      if (m_initialized == false)
        m_trace_mng->fatal() << "Cannot print uninitialized table";
      //! Debug print
      m_trace_mng->pinfo() << " ---Table points (x,f(x))---";
      m_trace_mng->pinfo() << " Table size = " << m_inserted_point_number;
      for (Integer ii = m_first_point_index; ii <= m_last_point_index; ++ii)
        {
          m_trace_mng->pinfo() << "(m_x[" << ii << "]= " << m_x[ii] << ", m_y["
              << ii << "]= " << m_y[ii] << ")";
        }
      m_trace_mng->pinfo() << " ---End table points---";
    }

/*---------------------------------------------------------------------------*/

template<typename T>
inline Real PieceWiseFunctionR1vR1T<T>::_eval(const Real & variable) const {
	//! Check initialization in debug mode only
	ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

	//! Find lower point x index : 3 search types available (linear most efficient)
	//      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x[0]);
	Integer lower_index = ArcGeoSim::ArrayScan::linearIntervalScan(variable,
			m_total_point_number, &m_x_scan[0]);

	//! Compute interpolated value : done by Interpolator T
	return T::interpolate(&m_x[lower_index], &m_y[lower_index],
			&m_interval[lower_index], variable);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void PieceWiseFunctionR1vR1T<T>::_eval(const Real & variable,
		Real & result) const {
	result = _eval(variable);
}

template<typename T>
inline Real PieceWiseFunctionR1vR1T<T>::eval(const Real & variable) {
	return _eval(variable);
}

template<typename T>
void PieceWiseFunctionR1vR1T<T>::eval(const Real & variable, Real & result) {
	_eval(variable, result);
}
/*---------------------------------------------------------------------------*/

template<typename T>
void PieceWiseFunctionR1vR1T<T>::_eval(const Array<Real> & variable,
		Array<Real> & result) const {
	/*setVariable(0, variable);
	 setEvaluationResult(result);
	 eval();
	 cleanup();*/
	const Integer size = variable.size();

	if (size != result.size()) {
		m_trace_mng->fatal() << "Inconsistent size when evaluation";
	}

	for (Integer i = 0; i < size; ++i) {
		result[i] = _eval(variable[i]);
	}
}
template<typename T>
void PieceWiseFunctionR1vR1T<T>::eval(const Array<Real> & variable,
		Array<Real> & result) {
	_eval(variable, result);
}
/*---------------------------------------------------------------------------*/

template<typename T>
void PieceWiseFunctionR1vR1T<T>::_eval(const ConstArrayView<Real> variable,
		ArrayView<Real> result) const {
	/*   m_variable = variable;
	 m_eval_result = result;
	 eval();
	 cleanup();*/
	const Integer size = variable.size();

	if (size != result.size()) {
		m_trace_mng->fatal() << "Inconsistent size when evaluation";
	}

	for (Integer i = 0; i < size; ++i) {
		result[i] = _eval(variable[i]);
	}

}
template<typename T>
void PieceWiseFunctionR1vR1T<T>::eval(const ConstArrayView<Real> variable,
		ArrayView<Real> result) {
	_eval(variable, result);
}
/*---------------------------------------------------------------------------*/

template<typename T>
void PieceWiseFunctionR1vR1T<T>::eval() {
	const Integer size = m_variable.size();

	if (size != m_eval_result.size()) {
		m_trace_mng->fatal() << "Inconsistent size when evaluation";
	}

	for (Integer i = 0; i < size; ++i) {
		m_eval_result[i] = eval(m_variable[i]);
	}
}

/*---------------------------------------------------------------------------*/

template<typename T>
  inline Real
  PieceWiseFunctionR1vR1T<T>::_diffEval(const Real & variable) const
    {
      //! Check initialization in debug mode only
      ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

      //! Find lower point x index : 3 search types available (linear most efficient)
      //      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x_scan[0]);
      Integer lower_index = ArcGeoSim::ArrayScan::linearIntervalScan(variable, m_total_point_number,
          &m_x_scan[0]);
      Integer is_table_point = (m_x_scan[lower_index] == variable);

      //! Compute derivative: done by Interpolator T
      //! Left derivative
      Integer left_index = lower_index + is_table_point
          * m_coef_left_derivative;
      Real left_der = T::differentiate(&m_x[left_index], &m_y[left_index],
          &m_interval[left_index], variable);
      //! Right derivative
      Integer right_index = lower_index + is_table_point
          * m_coef_right_derivative;
      Real right_der = T::differentiate(&m_x[right_index], &m_y[right_index],
          &m_interval[right_index], variable);

      //! Return right or left derivative or mean (for table point only), otherwise right derivative
      return 0.5 * (right_der + left_der);

    }
template<typename T>
  inline Real
  PieceWiseFunctionR1vR1T<T>::diffEval(const Real & variable)
    {
	return _diffEval(variable);
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  inline void
  PieceWiseFunctionR1vR1T<T>::eval(const Real & variable, Real & result, Real & diffresult)
    {
      //! Coupled evaluation of f and its derivative

      //! Check initialization in debug mode only
      ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

      //! Find lower point x index : 3 search types available (linear most efficient)
      //      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x_scan[0]);
      Integer lower_index = ArcGeoSim::ArrayScan::linearIntervalScan(variable, m_total_point_number,
          &m_x_scan[0]);
      Integer is_table_point = (m_x_scan[lower_index] == variable);

      //! Compute interpolated value : done by Interpolator T
      result = T::interpolate(&m_x[lower_index], &m_y[lower_index],
          &m_interval[lower_index], variable);

      //! Compute derivative: done by Interpolator T
      //! Left derivative
      Integer left_index = lower_index + is_table_point
          * m_coef_left_derivative;
      Real left_der = T::differentiate(&m_x[left_index], &m_y[left_index],
          &m_interval[left_index], variable);
      //! Right derivative
      Integer right_index = lower_index + is_table_point
          * m_coef_right_derivative;
      Real right_der = T::differentiate(&m_x[right_index], &m_y[right_index],
          &m_interval[right_index], variable);

      //! Return right or left derivative or mean (for table point only), otherwise right derivative
      diffresult = 0.5 * (right_der + left_der);
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::sort()
    {
      //! Create iterators for inserted point and created intervals
      ComponentData::iterator ite_x;
      ComponentData::iterator ite_y;
      typename InterpolatorIntervalData::iterator ite_right_bound_interval; // [a , x]
      typename InterpolatorIntervalData::iterator ite_left_bound_interval; // [x , b]

      //! initialize point index
      m_current_x_index = m_total_point_number - 1; //! last element
      //! Sort in ascending order : costly but only in init()
      ite_x = (m_x_scan.end() - 1);
      ite_y = (m_y.end() - 1);
      typename InterpolatorIntervalData::iterator ite_interval =
          (m_interval.end() - 1);

      while (ite_x != m_x_scan.begin() && *ite_x < *(ite_x - 1))
        {
          //! Shift x value
          Real x_tmp = *(ite_x - 1);
          *(ite_x - 1) = *ite_x;
          *ite_x = x_tmp;
          //! Shift y value
          Real y_tmp = *(ite_y - 1);
          *(ite_y - 1) = *ite_y;
          *ite_y = y_tmp;
          if (ite_interval != m_interval.begin())
            {
              Real interval_tmp = *(ite_interval - 1);
              *(ite_interval - 1) = *ite_interval;
              *ite_interval = interval_tmp;
              --ite_interval;
            }
          --ite_x;
          --ite_y;
          --m_current_x_index;
        } // iterators ite_x and ite_y point on inserted values (x,y)

      //! Increment inserted points counters
      ++m_inserted_point_number;
      m_inserted_interval_number = m_inserted_point_number - 1;
      ++m_last_point_index;
      ++m_last_interval_index;

      //! Update m_x : first and last points are ghost cells : treated in updateGhostPoints
      for (Integer i = 1; i < m_total_point_number - 1; ++i)
        {
          m_x[i] = m_x_scan[i];
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::computeIntervalCoef()
    {

      if (m_inserted_point_number > 1)
        {

          //! Initialize current interval index : used in the call to T::computeIntervalCoef so that
          // the Interpolator T is aware of the available neighboors
          Integer current_interval_number = m_first_modified_interval_index;

          //! Compute modified interval coefficients within interpolator
          for (Integer i = m_first_modified_interval_index; i
              <= m_last_modified_interval_index; ++i)
            {
              // Signature : (*x_left, *y_left, interval_numer in [0, nb_interval-1], *current_interval_coef)
              T::computeIntervalCoef(&m_x[i], &m_y[i], current_interval_number,
                  &m_interval[i]);
              ++current_interval_number;
            }

          //! Update ghost interval coefficients: boundary for PieceWiseFunction evaluation
          //  (interval coefficients must already be computed)
          updateGhostIntervals();

        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::shiftRescale(const Real new_xmin, const Real new_xmax,
      const Real new_ymin, const Real new_ymax)
  //! Notice that arguments are y_min and y_max and NOT f(x_min)
  //  and f(x_max)
  //! The shape of the function is not modified, therefore no need to recompute interval coefficients
    {
      //! Check Arguments
      if (new_xmin > new_xmax || new_ymin > new_ymax)
        m_trace_mng->fatal()
            << "Input argument order incorrect : min values are greater than max values";

      //! Abscissas
      Real Dx = m_x_max - m_x_min;
      Real new_Dx = new_xmax - new_xmin;
      if (Dx == 0)
        m_trace_mng->fatal() << "Trying to rescale a Table with only one point";
      Real x_ratio = new_Dx / Dx;

      //! Ordinates
      Real Dy = m_y_max - m_y_min;
      Real new_Dy = new_ymax - new_ymin;
      Real y_ratio;
      if (Dy == 0)
        {
          y_ratio = 1;
        }
      else
        {
          y_ratio = new_Dy / Dy;
        }

      //! Shift and rescale
      for (Integer i = m_first_point_index; i <= m_last_point_index; i++)
        {
          m_x[i] = new_xmin + (m_x[i] - m_x_min) * x_ratio;
          m_x_scan[i] = m_x[i];
          m_y[i] = new_ymin + (m_y[i] - m_y_min) * y_ratio;
        }

      //! Update table point ghost cells
      updateGhostPoints();

      //! Shift and rescale upper and lower limit if FiniteProlongation mode
      if (m_border_type == BorderType::FiniteProlongation)
        {
          m_upper_limit = new_xmin + (m_upper_limit - m_x_min) * x_ratio;
          m_lower_limit = new_xmin + (m_lower_limit - m_x_min) * x_ratio;

          //! Change upper_limit and lower_limit values in the table
          updateLimitPoints();
        }

      //! PieceWiseFunction shape modified => need to recompute all interval coefficients
      m_first_modified_interval_index = m_first_interval_index;
      m_last_modified_interval_index = m_last_interval_index;
      computeIntervalCoef();

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::clone(
      const PieceWiseFunctionR1vR1T<T>& original_piecewisefunction)
    {
      //! Clone scalar
      m_x_max = original_piecewisefunction.m_x_max;
      m_x_min = original_piecewisefunction.m_x_min;
      m_y_max = original_piecewisefunction.m_y_max;
      m_y_min = original_piecewisefunction.m_y_min;
      m_first_point_index = original_piecewisefunction.m_first_point_index;
      m_last_point_index = original_piecewisefunction.m_last_point_index;
      m_inserted_point_number
          = original_piecewisefunction.m_inserted_point_number;
      m_total_point_number = original_piecewisefunction.m_total_point_number;
      m_first_interval_index
          = original_piecewisefunction.m_first_interval_index;
      m_last_interval_index = original_piecewisefunction.m_last_interval_index;
      m_first_modified_interval_index
          = original_piecewisefunction.m_first_modified_interval_index;
      m_last_modified_interval_index
          = original_piecewisefunction.m_last_modified_interval_index;
      m_inserted_interval_number
          = original_piecewisefunction.m_inserted_interval_number;
      m_total_interval_number
          = original_piecewisefunction.m_total_interval_number;
      m_border_type = original_piecewisefunction.m_border_type;
      m_prolongation_type = original_piecewisefunction.m_prolongation_type;
      m_lower_limit = original_piecewisefunction.m_lower_limit;
      m_upper_limit = original_piecewisefunction.m_upper_limit;
      m_out_of_bound_value = original_piecewisefunction.m_out_of_bound_value;
      m_coef_right_derivative
          = original_piecewisefunction.m_coef_right_derivative;
      m_coef_left_derivative
          = original_piecewisefunction.m_coef_left_derivative;
      m_initialized = original_piecewisefunction.m_initialized;
      m_filled = original_piecewisefunction.m_filled;
      m_first_point = original_piecewisefunction.m_first_point;

      //! Clone Array
      //! For std::vector (if Arcane::Array use clone() method instead)
      //! Table Data
      m_x.assign(original_piecewisefunction.m_x.begin(),
          original_piecewisefunction.m_x.end());
      m_y.assign(original_piecewisefunction.m_y.begin(),
          original_piecewisefunction.m_y.end());
      m_x_scan.assign(original_piecewisefunction.m_x_scan.begin(),
          original_piecewisefunction.m_x_scan.end());
      //! Interval Data
      m_interval.assign(original_piecewisefunction.m_interval.begin(),
          original_piecewisefunction.m_interval.end());

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::getPieceWiseFunctionType(PieceWiseFunctionType& type) const
    {
      T::getInterpolatorType(type.m_interpolator_type);
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::addTablePoint(const Real & x, const Real & y)
    {

      //! Insert points
      //      m_x.add(x); // For Arcane::Array<Real> use
      //      m_x_border_test.add(x); // For Arcane::Array<Real> use
      //      m_y.add(y); // For Arcane::Array<Real> use
      m_x.push_back(x); // For std::vector use instead of Array
      m_x_scan.push_back(x); // For std::vector use instead of Array
      m_y.push_back(y); // For std::vector use instead of Array
      //! Increment global counter
      ++m_total_point_number;

      if (!m_first_point)
        {
          //! Create interval
          m_interval.push_back(T::getDefaultInterval());
          //! Increment global counter
          ++m_total_interval_number;
        }

      //! Update first point indicator
      if (m_first_point)
        m_first_point = false;

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::initBorderCondition(
      const BorderType::eBorderType& border_type,
      const ProlongationType& prolongation_type, const Real& lower_limit,
      const Real& upper_limit, const BorderType::eCutType& cut_type)
    {
      if (m_boundary_defined or m_filled)
        m_trace_mng->fatal()
            << "Cannot change border behavior or set it after filling table";
      m_boundary_defined = true;

      //! Set border type
      initBorderType(border_type, prolongation_type, lower_limit, upper_limit,
          cut_type);

      //! Init Ghost points and intervals
      initGhostPoints();
      initGhostIntervals();

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::initBorderCondition()
    {
      /** Default behavior for boundary :
       * Infinite constant Prolongation
       */
      if (m_boundary_defined or m_filled)
        m_trace_mng->fatal()
            << "Cannot change border behavior or set it after filling table";
      m_boundary_defined = true;

      //! Init Ghost points and intervals
      initGhostPoints();
      initGhostIntervals();

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::initBorderType(
      const BorderType::eBorderType& border_type,
      const ProlongationType& prolongation_type, const Real& lower_limit,
      const Real& upper_limit, const BorderType::eCutType& cut_type)
    {
      m_border_type = border_type;
      m_prolongation_type = prolongation_type;
      m_lower_limit = lower_limit;
      m_upper_limit = upper_limit;
      //! cut value
      switch (cut_type)
        {
      case (BorderType::SilentCut):
        m_out_of_bound_value = std::numeric_limits<Real>::quiet_NaN();
        break;
      case (BorderType::ExceptionThrowCut):
        m_out_of_bound_value = std::numeric_limits<Real>::signaling_NaN();
        break;
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::setLimitDerivationStrategy(
      const LimitDerivationStrategy::eLimitDerivationStrategy& derivation_strategy)
    {
      //! Derivative computation given by:
      // derivative = 0.5* (der1 + der2)
      // with der1 = differentiate(m_interval[right_interval_index +  m_coef_right_derivative])
      // and  der2 = differentiate(m_interval[right_interval_index +  m_coef_left_derivative])
      switch (derivation_strategy)
        {
      case (LimitDerivationStrategy::RightDerivative):
        m_coef_right_derivative = 0;
        m_coef_left_derivative = 0;
        break;
      case (LimitDerivationStrategy::LeftDerivative):
        m_coef_right_derivative = -1;
        m_coef_left_derivative = -1;
        break;
      case (LimitDerivationStrategy::MeanDerivative):
        m_coef_right_derivative = 0;
        m_coef_left_derivative = -1;
        break;
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::initGhostPoints()
    {
      /** Init table ghost cells ***/

      //! Switch on border type
      switch (m_border_type)
        {
      case (BorderType::FiniteProlongation):
        {

          /** Table Data for Finite prolongation:
           m_x_scan = (-inf, lower, a, b, c, upper, +inf) Abscissa for scan (avoid test for boundary)
           m_x      = (0, lower, a, b, c, upper, 0) Abscissa for eval
           m_y = (0, f(lower),  f(a), f(b), f(c), f(upper),0) Ordinates
           **/
          //! Add (-inf, lower, upper, +inf) in the table) (m_x_scan and m_x)
          addTablePoint(-FloatInfo<Real>::maxValue(), 0);
          addTablePoint(m_lower_limit, 0);
          addTablePoint(m_upper_limit, 0);
          addTablePoint(FloatInfo<Real>::maxValue(), 0);
          Integer end_x = m_total_point_number - 1;
          //! Replace infinity by 0 in m_x
          m_x[0] = 0;
          m_x[end_x] = 0;
          m_y[0] = 0;
          m_y[end_x] = 0;
          //! Set indexes
          m_first_point_index = 2; // Index of first inserted point
          m_last_point_index = end_x - 2; // Index of last inserted point
        }
        break;
      case (BorderType::InfiniteProlongation):
        {
          /** Table Data for Finite prolongation:
           m_x_scan = (-inf, a, b, c, +inf) Abscissa for scan (avoid test for boundary)
           m_x      = (0, a, b, c, 0) Abscissa for eval
           m_y = (0, f(a), f(b), f(c), 0) Ordinates
           **/
          addTablePoint(-FloatInfo<Real>::maxValue(), 0);
          addTablePoint(FloatInfo<Real>::maxValue(), 0);
          Integer end_x = m_total_point_number - 1;
          //! Set indexes
          m_first_point_index = 1; // Index of first inserted point
          m_last_point_index = end_x - 1; // Index of last inserted point
        }
        break;
      case (BorderType::NotProlongated):
        {
          /** Table Data for Not prolongated boundary:
           m_x_scan = (-inf, a, b, c, +inf) Abscissa for scan (avoid test for boundary)
           m_x      = (0, a, b, c, 0) Abscissa for eval
           m_y = (NaN, f(a), f(b), f(c), NaN) Ordinates
           **/
          addTablePoint(-FloatInfo<Real>::maxValue(), 0);
          addTablePoint(FloatInfo<Real>::maxValue(), 0);
          Integer end_x = m_total_point_number - 1;
          m_x[0] = 0;
          m_x[end_x] = 0;
          m_y[0] = 0;
          m_y[end_x] = 0;
          //! Set indexes
          m_first_point_index = 1; // Index of first inserted point
          m_last_point_index = end_x - 1; // Index of last inserted point
        }
        break;
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::initGhostIntervals()
    {
      /** Init table ghost intervals ***/

      //! Switch on border type
      switch (m_border_type)
        {
      case (BorderType::FiniteProlongation):
        {
          /** m_interval = (NaN, lower-a_boundcoef, a-b_coef , b-c_coef, c-upper_boundcoef, NaN) interval coefficient **/
          //! Set m_interval(0) so that eval(x) if x in m_interval(0) returns NaN
          Integer end_interval = m_total_interval_number - 1;
          m_interval[0] = T::getForbiddenInterval(m_out_of_bound_value);
          //! Set m_interval(end) so that eval(x) if x in m_interval(end) returns NaN
          m_interval[end_interval] = T::getForbiddenInterval(
              m_out_of_bound_value);
          m_first_interval_index = 2;
          m_last_interval_index = end_interval - 2;
        }
        break;
      case (BorderType::InfiniteProlongation):
        {
          /**m_interval = (updated_boundary_value, a-b_coef , b-c_coef, updated_boundary_value) interval coefficient **/
          Integer end_interval = m_total_interval_number - 1;
          m_first_interval_index = 1;
          m_last_interval_index = end_interval - 1;
        }
        break;
      case (BorderType::NotProlongated):
        {
          /** m_interval = (NaN, a-b_coef , b-c_coef, NaN) interval coefficient **/
          //! Set m_interval(0) so that eval(x) if x in m_interval(0) returns NaN
          Integer end_interval = m_total_interval_number - 1;
          m_interval[0] = T::getForbiddenInterval(m_out_of_bound_value);
          //! Set m_interval(end) so that eval(x) if x in m_interval(end) returns NaN
          m_interval[end_interval] = T::getForbiddenInterval(
              m_out_of_bound_value);
          m_first_interval_index = 1;
          m_last_interval_index = end_interval - 1;
        }
        break;
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::updateGhostPoints()
    {
      //! first copy m_x_scan in m_x
      //! m_x_scan = (-inf,a,b,c,+inf)
      //! m_x      = (boundary,a,b,c,boundary)
      Integer end_x = m_total_point_number - 1;

      switch (m_border_type)
        {
      case BorderType::NotProlongated:
        //! ghost cells not used
        break;
      case BorderType::FiniteProlongation:
        T::updateGhostPoints(m_prolongation_type, &m_x[1], &m_x[end_x - 1],
            &m_y[1], &m_y[end_x - 1]);
        break;
      case BorderType::InfiniteProlongation:
        T::updateGhostPoints(m_prolongation_type, &m_x[0], &m_x[end_x],
            &m_y[0], &m_y[end_x]);
        break;
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::updateGhostIntervals()
    {
      Integer end_x = m_total_point_number - 1;
      Integer end_interval = m_total_interval_number - 1;

      switch (m_border_type)
        {

      case BorderType::NotProlongated:
        //! ghost interval coefficient: for points m_x = (0,a,b,c,0) and m_y = (0, f(a), f(b), f(c), 0)
        //! => m_interval = (NaN, a-b_coef, b-c_coef, NaN)
        m_interval[0] = T::getForbiddenInterval(m_out_of_bound_value);
        m_interval[end_interval]
            = T::getForbiddenInterval(m_out_of_bound_value);
        break;

      case BorderType::FiniteProlongation:
        //! ghost interval coefficient: for points m_x = (0,lower,a,b,c,upper,0) m_y = (0, f(lower),  f(a), f(b), f(c), f(upper),0)
        //! => m_interval = (NaN, lower-a_boundcoef, a-b_coef , b-c_coef, c-upper_boundcoef, NaN)
        m_interval[0] = T::getForbiddenInterval(m_out_of_bound_value);
        m_interval[end_interval]
            = T::getForbiddenInterval(m_out_of_bound_value);
        //! Update f(lower) f(upper) and intervals with upper_point and lower_point :
        //      i.e. compute lower-a_boundcoef
        //      &    compute c-upper_boundcoef
        T::updateGhostIntervals(m_prolongation_type, &m_x[1], &m_x[end_x - 1],
            &m_y[1], &m_y[end_x - 1], &m_interval[1], &m_interval[end_interval
                - 1]);
        break;

      case BorderType::InfiniteProlongation:
        //! Update f(infinite) f(-infinite) and interval with +/- infinite to obtain chosen prolongation
        T::updateGhostIntervals(m_prolongation_type, &m_x[0], &m_x[end_x],
            &m_y[0], &m_y[end_x], &m_interval[0], &m_interval[end_interval]);
        break;

        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::updateLimitPoints()
    {
      if (m_border_type != BorderType::FiniteProlongation)
        m_trace_mng->fatal()
            << "Limit points not used if not FiniteProlongation. updateLimitPoints() fails";
      //! Upper and lower limit only to be changed in the scan array m_x_scan
      //! Lower limit
      m_x_scan[1] = m_lower_limit;
      //! Upper limit
      m_x_scan[m_last_point_index + 1] = m_upper_limit;
      //! Update ghost interval coefficient
      updateGhostIntervals();

    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::getConnectedIntervals()
    {
      if (m_inserted_point_number > 1)
        {
          // Get the number (numero) between 0 and m_inserted_interval_number -1 of the intervals connected to the inserted point

          //! Get the effective number of interval present at the left and right of the current point
          // => stored in m_nb_left_interval and m_nb_right_interval
          getEffectiveIntervalNumber(m_current_x_index);

          //! Get the number of interval modified by the point insertion (given by the interpolator)
          //  at the left => nb_left_interval and at the right nb_right_interval of an inserted point
          Integer nb_left_interval, nb_right_interval;
          T::getConnectedIntervalNumber(nb_left_interval, nb_right_interval);

          //! Connection algorithm
          //! First modified interval
          if (m_nb_left_interval < nb_left_interval)
          // first point inserted or number of intervals at the left of the point < nb_left_interval
            {
              m_first_modified_interval_index = m_first_interval_index;
            }
          else // default case
            {
              m_first_modified_interval_index = m_current_x_index
                  - nb_left_interval;
            }
          //! Last modified interval
          if (m_nb_right_interval < nb_right_interval)
          // last point inserted or number of intervals at the right of the point < nb_right_interval
            {
              m_last_modified_interval_index = m_last_interval_index;
            }
          else // default case
            {
              m_last_modified_interval_index = m_current_x_index
                  + nb_right_interval - 1;
            }
        }
    }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR1vR1T<T>::getEffectiveIntervalNumber(Integer x_index)
    {
      //! method to get the index of interval at the left and right of the given point
      m_nb_left_interval = x_index - m_first_point_index;
      m_nb_right_interval = m_last_point_index - x_index;

    }

/*---------------------------------------------------------------------------*/
