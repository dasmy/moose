#include "DerivativeMultiPhaseBase.h"

template<>
InputParameters validParams<DerivativeMultiPhaseBase>()
{
  InputParameters params = validParams<DerivativeFunctionMaterialBase>();

  // Phase materials 1-n
  params.addRequiredParam<std::vector<std::string> >("fi_names", "List of free energies for the n phases");
  params.addParam<std::vector<std::string> >("hi_names", "Switching Function Materials that provide h(eta_i)");

  // All arguments to the phase free energies
  params.addCoupledVar("args", "Arguments of the fi free energies - use vector coupling");

  // Barrier
  params.addParam<std::string>("g", "g", "Barrier Function Material that provides g(eta_i)");
  params.addParam<Real>("W", 0.0, "Energy barrier for the phase transformation from A to B");

  return params;
}

DerivativeMultiPhaseBase::DerivativeMultiPhaseBase(const std::string & name,
                                                       InputParameters parameters) :
    DerivativeFunctionMaterialBase(name, parameters),
    _eta_index(_nargs, -1),
    _num_etas(coupledComponents("etas")),
    _eta_names(_num_etas),
    _eta_vars(_num_etas),
    _fi_names(getParam<std::vector<std::string> >("fi_names")),
    _num_fi(_fi_names.size()),
    _prop_Fi(_num_fi),
    _prop_dFi(_num_fi),
    _prop_d2Fi(_num_fi),
    _prop_d3Fi(_num_fi),
    _hi_names(getParam<std::vector<std::string> >("hi_names")),
    _num_hi(_hi_names.size()),
    _hi(_num_hi),
    _g_name(getParam<std::string>("g")),
    _g(getMaterialProperty<Real>(_g_name)),
    _dg(_num_etas),
    _d2g(_num_etas),
    _W(getParam<Real>("W"))
{
  // check passed in parameter vectors
  if (_num_fi != _num_hi)
    mooseError("Need to pass in as many hi_names as fi_names in DerivativeMultiPhaseBase " << name);

  // get order parameter names and libmesh variable names, set barrier function derivatives
  for (unsigned int i = 0; i < _num_etas; ++i)
  {
    _eta_names[i] = getVar("etas", i)->name();
    _eta_vars[i] = coupled("etas", i);

    // for each coupled variable we need to know if it was coupled through "etas"
    // and - if so - which coupled component of "etas" it comes from
    if (_eta_vars[i] < _number_of_nl_variables)
      _eta_index[argIndex(_eta_vars[i])] = i;

    // barrier function derivatives
    _dg[i] = &getMaterialPropertyDerivative<Real>(_g_name, _eta_names[i]);
    _d2g[i].resize(_num_etas);
    for (unsigned int j = 0; j < _num_etas; ++j)
      _d2g[i][j] = &getMaterialPropertyDerivative<Real>(_g_name, _eta_names[i], _eta_names[j]);
  }

  // reserve space and set phase material properties
  for (unsigned int n = 0; n < _num_fi; ++n)
  {
    // get phase free energy
    _prop_Fi[n] = &getMaterialProperty<Real>(_fi_names[n]);
    _prop_dFi[n].resize(_nargs);
    _prop_d2Fi[n].resize(_nargs);
    _prop_d3Fi[n].resize(_nargs);

    // get switching function
    _hi[n] = &getMaterialProperty<Real>(_hi_names[n]);

    for (unsigned int i = 0; i < _nargs; ++i)
    {
      _prop_dFi[n][i] = &getMaterialPropertyDerivative<Real>(_fi_names[n], _arg_names[i]);
      _prop_d2Fi[n][i].resize(_nargs);

      if (_third_derivatives)
        _prop_d3Fi[n][i].resize(_nargs);

      for (unsigned int j = 0; j < _nargs; ++j)
      {
        _prop_d2Fi[n][i][j] = &getMaterialPropertyDerivative<Real>(_fi_names[n], _arg_names[i], _arg_names[j]);

        if (_third_derivatives) {
          _prop_d3Fi[n][i][j].resize(_nargs);

          for (unsigned int k = 0; k < _nargs; ++k)
            _prop_d3Fi[n][i][j][k] = &getMaterialPropertyDerivative<Real>(_fi_names[n], _arg_names[i], _arg_names[j], _arg_names[k]);
        }
      }
    }
  }
}

Real
DerivativeMultiPhaseBase::computeF()
{
  Real F = 0.0;
  for (unsigned n = 0; n < _num_fi; ++n)
    F += (*_hi[n])[_qp] * (*_prop_Fi[n])[_qp];
  return F + _W * _g[_qp];
}
