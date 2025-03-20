#ifndef ARCGEOSIM_NUMERICS_EXPRESSIONS_PIECEWISEFUNCTIONR2VR1_H
#error "Template implementation has to be used with template declaration"
#endif 

#ifdef WIN32
#include <ciso646>
#endif

#include <arcane/ArcaneVersion.h>

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::init()
  {
    //! Check Table is filled : table must be filled before init()
    //!       done either by the Service (data read from mydata.arc) :
    //!               MyInterpolationPieceWiseFunctionService->init()
    //!       either manually :
    //!               PieceWiseFunctionR2vR1T<MyInterpolator> my_pwf(traceMng());
    //!               my_pwf.setBorderCondition();
    //!               my_pwf.setPoint(x,y);...
    //!               my_pwf.init(); // Object ready for use
    if (m_initialized)
      return;
    if (m_filled == false)
      throw FatalErrorException(A_FUNCINFO,
          "Cannot initialize PieceWiseFunctionR2vR1T<MyInterpolator> before filling table");
    m_initialized = true;
  }

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::setVariable(const String & name, const Array<
		Real> & variable)
		{
	for (Integer i = 0; i < m_nb_variables; i++)
	{
		if (name == m_axe_name[i])
		{
			m_var[i].setArray(variable);
			return;
		}
	}
	throw FatalErrorException(A_FUNCINFO,
			"Cannot set variable data for undefined variable '" + name + "'");
		}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::setVariable(Integer index,
		const Array<Real> & variable)
		{
	if (index >= 0 && index < m_nb_variables)
	{
		m_var[index].setArray(variable);
		return;
	}
	throw FatalErrorException(A_FUNCINFO,
			String::format(
					"Cannot set variable data for undefined variable position '{0}'",
					index));
		}

/*---------------------------------------------------------------------------*/
template<typename T>
  String
  PieceWiseFunctionR2vR1T<T>::variableName(Integer index) const
  {
    if (index >= 0 && index < m_nb_variables)
      {
        return m_axe_name[index];
      }
    throw FatalErrorException(A_FUNCINFO,
        String::format(
            "Cannot get variable name for undefined variable position '{0}'",
            index));
    return "";
  }

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::setEvaluationResult(Array<Real> & result)
{
	m_eval_result.setArray(result);
}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::setDerivationResult(Integer di,
		Array<Real> & result)
		{
	if (di >= 0 && di < m_nb_variables)
	{
		m_diff_result[di].setArray(result);
		return;
	}
	throw FatalErrorException(A_FUNCINFO,
			String::format(
					"Cannot derive against undefined variable index '{0}'",
					di));
		}

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::setDerivationResult(const String & di, Array<
		Real> & result)
		{
	for (Integer i = 0; i < m_nb_variables; i++)
	{
		if (di == m_axe_name[i])
		{
			m_diff_result[i].setArray(result);
			return;
		}
	}
	throw FatalErrorException(A_FUNCINFO,
			"Cannot derive against undefined variable '" + di + "'");

		}

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::cleanup()
  {
    for (Integer i = 0; i < m_nb_variables; i++)
      {
        m_var[i] = RealConstArrayView();
        m_diff_result[i] = RealArrayView();
      }
    m_eval_result = RealArrayView();
  }
/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::setvariableName(Integer index,
      const String & name)
  {
    if (index >= 0 && index < m_nb_variables)
      {
        m_axe_name[index] = name;
        return;
      }
    throw FatalErrorException(A_FUNCINFO,
        String::format(
            "Cannot set variable name for undefined variable position '{0}'",
            index));
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::printPoints() const
  {
    //! Check initialization
    if (m_initialized == false)
      throw FatalErrorException(A_FUNCINFO,
          "Cannot print uninitialized table");
    //! Debug print
    m_trace_mng->pinfo() << " ---Table points (x,y,f(x,y))---";
    m_trace_mng->pinfo() << " Table row size = " << m_inserted_point_number[0];
    m_trace_mng->pinfo() << " Table column size = " << m_inserted_point_number[1];

    for (Integer i1 = 0; i1 < (Integer)m_x[1].size(); ++i1)
      for (Integer i0 = 0; i0 < (Integer)m_x[0].size(); ++i0)
        {
          m_trace_mng->pinfo() << "(m_x0[" << i0 << "]= " << m_x_scan[0][i0]
              << " m_x1[" << i1 << "]= " << m_x_scan[1][i1] << " m_y[" << i0 << "]["
              << i1 << "]= " << m_y[i0][i1] << ")";
        }
    m_trace_mng->pinfo() << " ---End table points---";
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  inline Real
  PieceWiseFunctionR2vR1T<T>::eval(const Real & var0, const Real & var1)
  {
    //! Check initialization in debug mode only
    ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

    //! Find lower point x index : 3 search types available (linear most efficient)
    //      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x[0]);
    Integer lower_index0 = ArcGeoSim::ArrayScan::linearIntervalScan(var0,
        m_total_point_number[0], &m_x_scan[0][0]);
    Integer lower_index1 = ArcGeoSim::ArrayScan::linearIntervalScan(var1,
        m_total_point_number[1], &m_x_scan[1][0]);

    //! Compute interpolated value : done by Interpolator T
    return T::interpolate(&m_x[0][lower_index0], &m_x[0][lower_index0 + 1],
        &m_x[1][lower_index1], &m_x[1][lower_index1 + 1],
        &m_y[lower_index0][lower_index1], &m_y[lower_index0 + 1][lower_index1],
        &m_y[lower_index0][lower_index1 + 1],
        &m_y[lower_index0 + 1][lower_index1 + 1], &var0, &var1);
  }


/*---------------------------------------------------------------------------*/

template<typename T>
  inline Real
  PieceWiseFunctionR2vR1T<T>::diff0Eval(const Real & var0, const Real & var1)
  {
    //! Check initialization in debug mode only
    ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

    //! Find lower point x index : 3 search types available (linear most efficient)
    //      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x[0]);
    Integer lower_index0 = ArcGeoSim::ArrayScan::linearIntervalScan(var0,
        m_total_point_number[0], &m_x_scan[0][0]);
    Integer lower_index1 = ArcGeoSim::ArrayScan::linearIntervalScan(var1,
        m_total_point_number[1], &m_x_scan[1][0]);

    //! Compute interpolated value : done by Interpolator T
    return T::differentiate0(&m_x[0][lower_index0], &m_x[0][lower_index0 + 1],
        &m_x[1][lower_index1], &m_x[1][lower_index1 + 1],
        &m_y[lower_index0][lower_index1], &m_y[lower_index0 + 1][lower_index1],
        &m_y[lower_index0][lower_index1 + 1],
        &m_y[lower_index0 + 1][lower_index1 + 1], &var1);
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  inline Real
  PieceWiseFunctionR2vR1T<T>::diff1Eval(const Real & var0, const Real & var1)
  {
    //! Check initialization in debug mode only
    ARCANE_ASSERT((m_initialized), ("Cannot evaluate uninitialized table"));

    //! Find lower point x index : 3 search types available (linear most efficient)
    //      Integer lower_index = ArcGeoSim::ArrayScan::dichotomicIntervalScan(variable, m_total_point_number, &m_x[0]);
    Integer lower_index0 = ArcGeoSim::ArrayScan::linearIntervalScan(var0,
        m_total_point_number[0], &m_x_scan[0][0]);
    Integer lower_index1 = ArcGeoSim::ArrayScan::linearIntervalScan(var1,
        m_total_point_number[1], &m_x_scan[1][0]);

    //! Compute interpolated value : done by Interpolator T
    return T::differentiate1(&m_x[0][lower_index0], &m_x[0][lower_index0 + 1],
        &m_x[1][lower_index1], &m_x[1][lower_index1 + 1],
        &m_y[lower_index0][lower_index1], &m_y[lower_index0 + 1][lower_index1],
        &m_y[lower_index0][lower_index1 + 1],
        &m_y[lower_index0 + 1][lower_index1 + 1], &var0);
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::eval(const Real & var0, const Real & var1,
      Real & res0)
  {
    m_var[0] = RealConstArrayView(1, &var0);
    m_var[1] = RealConstArrayView(1, &var1);
    m_eval_result = RealArrayView(1, &res0);
    eval();
    cleanup();
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::eval(const Real & var0, const Real & var1,
      Real & res0, Real & diffres0,
      Real & diffres1)
  {
    m_var[0] = RealConstArrayView(1, &var0);
    m_var[1] = RealConstArrayView(1, &var1);
    m_eval_result = RealArrayView(1, &res0);
    m_diff_result[0] = RealArrayView(1, &diffres0);
    m_diff_result[1] = RealArrayView(1, &diffres1);
    eval();
    diffeval();
    cleanup();
  }

/*---------------------------------------------------------------------------*/

template<typename T>
void
PieceWiseFunctionR2vR1T<T>::eval(const Array<Real> & var0, const Array<
		Real> & var1, Array<Real> & res0)
		{
	setVariable(0, var0);
	setVariable(1, var1);
	setEvaluationResult(res0);
	eval();
	cleanup();
		}

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::eval(const ConstArrayView<Real> var0,
      const ConstArrayView<Real> var1, ArrayView<Real> res0)
  {
    m_var[0] = var0;
    m_var[1] = var1;
    m_eval_result = res0;
    eval();
    cleanup();
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::eval()
  {
    const Integer size_res = m_eval_result.size();
    IntegerSharedArray size(m_nb_variables);
    for (Integer ivar = 0; ivar < m_nb_variables; ivar++)
      {
        size[ivar] = m_var[ivar].size();
        if (size[ivar] > 1 && size[ivar] != size_res)
          {
            throw FatalErrorException(A_FUNCINFO,
                "Inconsistent variable sizes when evaluation");
            return;
          }
      }
    if (size_res > 1 && size[0] > 1 && size[1] > 1)
      {
        for (Integer i = 0; i < size_res; ++i)
          {
            m_eval_result[i] = eval(m_var[0][i], m_var[1][i]);

          }
        return;
      }
    if (size_res > 1 && size[0] == 1 && size[1] > 1)
      {
        for (Integer i = 0; i < size_res; ++i)
          {
            m_eval_result[i] = eval(m_var[0][0], m_var[1][i]);

          }
        return;
      }
    if (size_res > 1 && size[0] > 1 && size[1] == 1)
      {
        for (Integer i = 0; i < size_res; ++i)
          {
            m_eval_result[i] = eval(m_var[0][i], m_var[1][0]);

          }
        return;
      }
    if (size_res > 1 && size[0] == 1 && size[1] == 1)
      {
        for (Integer i = 0; i < size_res; ++i)
          {
            m_eval_result[i] = eval(m_var[0][0], m_var[1][0]);

          }
        return;
      }
    m_eval_result[0] = eval(m_var[0][0], m_var[1][0]);
    return;
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::diffeval()
  {
    IntegerSharedArray size_res(m_nb_variables);
    IntegerSharedArray size(m_nb_variables);
    for (Integer ivar = 0; ivar < m_nb_variables; ivar++)
      {
        size_res[ivar] = m_diff_result[ivar].size();
        size[ivar] = m_var[ivar].size();
        if (size[ivar] > 1 && size[ivar] != size_res[ivar])
          {
            throw FatalErrorException(A_FUNCINFO,
                "Inconsistent variable sizes when evaluation");
            return;
          }
      }
    if (size[0] > 1 && size[1] > 1)
      {
        for (Integer i = 0; i < size[0]; ++i)
          {
            m_diff_result[0][i] = diff0Eval(m_var[0][i], m_var[1][i]);
            m_diff_result[1][i] = diff1Eval(m_var[0][i], m_var[1][i]);
          }
        return;
      }
    if (size[0] == 1 && size[1] > 1)
      {
        for (Integer i = 0; i < size[1]; ++i)
          {
            m_diff_result[1][i] = diff1Eval(m_var[0][i], m_var[1][i]);
          }
        return;
      }
    if (size[0] > 1 && size[1] == 1)
      {
        for (Integer i = 0; i < size[0]; ++i)
          {
            m_diff_result[0][i] = diff0Eval(m_var[0][i], m_var[1][i]);
          }
        return;
      }
    m_diff_result[0][0] = diff0Eval(m_var[0][0], m_var[1][0]);
    m_diff_result[1][0] = diff1Eval(m_var[0][0], m_var[1][0]);
    return;
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::getPieceWiseFunctionType(
      PieceWiseFunctionType& type) const
  {
    T::getInterpolatorType(type.m_interpolator_type);
  }

/*---------------------------------------------------------------------------*/
//! check row values : strictly increasing values between min and max

template<typename T>
  bool
  PieceWiseFunctionR2vR1T<T>::checkAxeDef(const RealArray & x0,
      const VectorType::eVectorType axeType)
  {
    Integer x0size = x0.size();
    Real min = m_x_scan[axeType][m_first_point_index[axeType] - 1];
    for (Integer i = 0; i < x0size; i++)
      if (x0[i] > min)
        {
          min = x0[i];
        }
      else
        {
          return false;
        }
    return (x0[x0size - 1] < m_x_scan[axeType][m_last_point_index[axeType] + 1]);
  }

  /*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::addTableVector(const RealArray & x0,
      const Real & x1, const RealArray & y, const VectorType::eVectorType vectorType)
  {
    bool isRow = vectorType == VectorType::row;
    Integer index = isRow ? 1: 0 ;

    // check vector values before filling it
    if (x0.size() != y.size())
      {
        throw FatalErrorException(A_FUNCINFO,
            String::format(
                "inconsistent length between vector (={0}) and index (={1}): should be equal",
                y.size(), x0.size()));
      }
    if (x1 < m_x[index][m_last_point_index[index]])
      {
        throw FatalErrorException(A_FUNCINFO,
            "vector index should be strictly increasing between min and max");
      }
    Integer x0size = x0.size();
    ComponentDataIterator it;
    Integer rowSize = dim1Size(m_y);
    Integer colSize = dim2Size(m_y);

    if (!m_filled)
      {
        // check vector values before filling it
        if (! checkAxeDef(x0, vectorType))
          {
            throw FatalErrorException(A_FUNCINFO,
                "indexes values should be strictly increasing between min and max");
          }
        //Table initialization, set axe definition m_x
        Integer axe = isRow ? 0: 1 ;
        it = m_x[axe].begin();
        for (Integer i = 0; i < m_first_point_index[axe]; i++) it++;
        m_x[axe].insert(it, x0.begin(), x0.end());

        it = m_x_scan[axe].begin();
        for (Integer i = 0; i < m_first_point_index[axe]; i++) it++;
        m_x_scan[axe].insert(it, x0.begin(), x0.end());

        m_inserted_point_number[axe] += x0size;
        m_total_point_number[axe] += x0size;
        m_last_point_index[axe] = m_first_point_index[axe] + x0size - 1;

        rowSize += isRow ? x0size: 0 ;
        colSize += isRow ? 0: x0size ;
        m_filled = true;
      }
    // resize table for the new vector y and fill it
    rowSize += isRow ? 0: 1 ;
    colSize += isRow ? 1: 0 ;
    resize(m_y,rowSize,colSize);
    // m_yview is the actual user table
//    Matrixindice indices;
    PWFDataTools::PWFArray2View m_yview = view(m_y,m_first_point_index[0],m_total_point_number[0],m_first_point_index[1], m_total_point_number[1]);
    if (isRow)
      {
        // fill new row in table
        for (Integer index = 0; index < y.size(); index++)
          m_yview[index][m_inserted_point_number[1]] = y[index];
      }
    else
      {
        // fill new col in table
        for (Integer index = 0; index < y.size(); index++)
          m_yview[m_inserted_point_number[0]][index] = y[index];
      }

    // add new point x1 in axe definition of table
    index = isRow ? 1: 0 ;
    it = m_x[index].begin();
    for (Integer i = 0; i < m_last_point_index[index]+1; i++) it++;
    m_x[index].insert(it, x1);

    it = m_x_scan[index].begin();
    for (Integer i = 0; i < m_last_point_index[index]+1; i++) it++;
    m_x_scan[index].insert(it, x1);
    //count axe points
    m_inserted_point_number[index]++;
    m_total_point_number[index]++;
    m_last_point_index[index] = m_first_point_index[index] + m_inserted_point_number[index]
        - 1;

    // complete or update ghost points
    updateGhostPoints();
  }


template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::initBorderCondition(
      const BorderType::eBorderType& border_type,
      const ProlongationType& prolongation_type, const Real& lower_row_limit,
      const Real& upper_row_limit, const Real& lower_col_limit,
      const Real& upper_col_limit, const BorderType::eCutType& cut_type)
  {
    if (m_boundary_defined or m_filled)
      throw FatalErrorException(A_FUNCINFO,
          "Cannot initialize border behavior after filling table");
    m_boundary_defined = true;

    //! Set border type
    initBorderType(border_type, prolongation_type, lower_row_limit,
        upper_row_limit, lower_col_limit, upper_col_limit, cut_type);

    //! Init Ghost points
    initGhostPoints();

  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::initBorderCondition()
  {
    /** Default behavior for boundary :
     * Infinite constant Prolongation
     */
    if (m_boundary_defined or m_filled)
      m_trace_mng->fatal()
          << "Cannot change border behavior or set it after filling table";
    m_boundary_defined = true;

    //! Init Ghost points
    initGhostPoints();
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::initBorderType(
      const BorderType::eBorderType& border_type,
      const ProlongationType& prolongation_type, const Real& lower_row_limit,
      const Real& upper_row_limit, const Real& lower_col_limit,
      const Real& upper_col_limit, const BorderType::eCutType& cut_type)
  {
    m_border_type = border_type;
    m_prolongation_type = prolongation_type;
    m_lower_limit[0] = lower_row_limit;
    m_upper_limit[0] = upper_row_limit;
    m_lower_limit[1] = lower_col_limit;
    m_upper_limit[1] = upper_col_limit;
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
  PieceWiseFunctionR2vR1T<T>::setLimitDerivationStrategy(
      const LimitDerivationStrategy::eLimitDerivationStrategy& derivation_strategy)
  {
    //! Derivative computation given by:
    // derivative = 0.5* (der1 + der2)
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
  PieceWiseFunctionR2vR1T<T>::initGhostPoints()
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
         m_y = (NaN, f(lower),  f(a), f(b), f(c), f(upper),NaN) Ordinates
         **/
        resize(m_y,4,4);
        for (Integer index = 0; index < m_nb_variables; ++index)
          {
            m_first_point_index[index]=2;
            m_last_point_index[index]=1;
            m_x[index].push_back(0);
            m_x[index].push_back(m_lower_limit[index]);
            m_x[index].push_back(m_upper_limit[index]);
            m_x[index].push_back(0);
            m_x_scan[index].push_back(-FloatInfo<Real>::maxValue());
            m_x_scan[index].push_back(m_lower_limit[index]);
            m_x_scan[index].push_back(m_upper_limit[index]);
            m_x_scan[index].push_back(+FloatInfo<Real>::maxValue());
            m_y[index][0] = m_out_of_bound_value;
            m_y[index][3] = m_out_of_bound_value;
            m_total_point_number[index]+=4;
          }
      }
      break;
    case (BorderType::InfiniteProlongation):
      {
        /** Table Data for Finite prolongation:
         m_x_scan = (-inf, a, b, c, +inf) Abscissa for scan (avoid test for boundary)
         m_x      = (0, a, b, c, 0) Abscissa for eval
         m_y = (0, f(a), f(b), f(c), 0) Ordinates
         **/
        resize(m_y,2,2);
        for (Integer index = 0; index < m_nb_variables; ++index)
          {
            m_first_point_index[index] = 1;
            m_last_point_index[index]=0;
            m_x[index].push_back(0);
            m_x[index].push_back(0);
            m_x_scan[index].push_back(-FloatInfo<Real>::maxValue());
            m_x_scan[index].push_back(+FloatInfo<Real>::maxValue());
            m_total_point_number[index] += 2;
          }
      }
      break;
    case (BorderType::NotProlongated):
      {
        /** Table Data for Not prolongated boundary:
         m_x_scan = (-inf, a, b, c, +inf) Abscissa for scan (avoid test for boundary)
         m_x      = (0, a, b, c, 0) Abscissa for eval
         m_y = (NaN, f(a), f(b), f(c), NaN) Ordinates
         **/
        resize(m_y,2,2);
        for (Integer index = 0; index < m_nb_variables; ++index)
          {
            m_first_point_index[index] = 1;
            m_last_point_index[index] = 0;
            m_x[index].push_back(0);
            m_x[index].push_back(0);
            m_x_scan[index].push_back(-FloatInfo<Real>::maxValue());
            m_x_scan[index].push_back(+FloatInfo<Real>::maxValue());
            m_total_point_number[index] += 2;
            m_y[index][0] = m_out_of_bound_value;
            m_y[index][1] = m_out_of_bound_value;
          }
      }
      break;
      }
  }

/*---------------------------------------------------------------------------*/

template<typename T>
  void
  PieceWiseFunctionR2vR1T<T>::updateGhostPoints()
  {
    switch (m_border_type)
      {
    case BorderType::NotProlongated:
      break;
    case BorderType::FiniteProlongation:
      {
        // compute lower/upper row/col limits
        Integer end_x0 = m_total_point_number[0] - 2;
        Integer end_x1 = m_total_point_number[1] - 2;
        ProlongationType  prolongation_type = m_prolongation_type;
        if (m_inserted_point_number[1] < 2) prolongation_type = T::ConstantProlongation;
        for (Integer i = m_first_point_index[0]; i <= m_last_point_index[0]; i++)
          {
            T::updateGhostPoints(prolongation_type, &m_x[1][2],
                &m_x[1][3], &m_y[i][2], &m_y[i][3], &m_x[1][1], &m_y[i][1]);
            T::updateGhostPoints(prolongation_type,
                &m_x[1][end_x1 - 1], &m_x[1][end_x1 - 2], &m_y[i][end_x1 - 1],
                &m_y[i][end_x1 - 2], &m_x[1][end_x1], &m_y[i][end_x1]);
          }
        prolongation_type = m_prolongation_type;
        if (m_inserted_point_number[0] < 2)prolongation_type = T::ConstantProlongation;
        for (Integer i = 1; i <= end_x1; i++)
          {
            T::updateGhostPoints(prolongation_type, &m_x[0][2],
                &m_x[0][3], &m_y[2][i], &m_y[3][i], &m_x[0][1], &m_y[1][i]);
            T::updateGhostPoints(prolongation_type,
                &m_x[0][end_x0 - 1], &m_x[0][end_x0 - 2], &m_y[end_x0 - 1][i],
                &m_y[end_x0 - 2][i], &m_x[0][end_x0], &m_y[end_x0][i]);
          }
        //fill border limits with NaN
        fillDim1(m_y,0,m_out_of_bound_value);
        fillDim1(m_y,dim2Size(m_y)-1,m_out_of_bound_value);
        fillDim2(m_y,0,m_out_of_bound_value);
        fillDim2(m_y,dim1Size(m_y)-1,m_out_of_bound_value);
        break;
      }
    case BorderType::InfiniteProlongation:
      {
        Integer end_x0 = m_total_point_number[0] - 1;
        Integer end_x1 = m_total_point_number[1] - 1;
        ProlongationType  prolongation_type = m_prolongation_type;
        if (m_inserted_point_number[1] < 2) prolongation_type = T::ConstantProlongation;
        for (Integer i = m_first_point_index[0]; i <= m_last_point_index[0]; i++)
          {
            T::updateGhostPoints(prolongation_type, &m_x[1][1],
                &m_x[1][2], &m_y[i][1], &m_y[i][2], &m_x[1][0], &m_y[i][0]);
            T::updateGhostPoints(prolongation_type,
                &m_x[1][end_x1 - 1], &m_x[1][end_x1 - 2], &m_y[i][end_x1 - 1],
                &m_y[i][end_x1 - 2], &m_x[1][end_x1], &m_y[i][end_x1]);
          }
        prolongation_type = m_prolongation_type;
        if (m_inserted_point_number[0] < 2)prolongation_type = T::ConstantProlongation;
        for (Integer i = 0; i <= end_x1; i++)
          {
            T::updateGhostPoints(prolongation_type, &m_x[0][1],
                &m_x[0][2], &m_y[1][i], &m_y[2][i], &m_x[0][0], &m_y[0][i]);
            T::updateGhostPoints(prolongation_type,
                &m_x[0][end_x0 - 1], &m_x[0][end_x0 - 2], &m_y[end_x0 - 1][i],
                &m_y[end_x0 - 2][i], &m_x[0][end_x0], &m_y[end_x0][i]);
          }
        break;
      }
      }
  }

/*---------------------------------------------------------------------------*/
