/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef FACTORY_H
#define FACTORY_H

#include <vector>

#include "MooseObject.h"
#include "InputParameters.h"

/**
 * Macros
 */
#define stringifyName(name) #name
#define registerObject(name)                        Factory::instance()->reg<name>(stringifyName(name))
#define registerNamedObject(obj, name)              Factory::instance()->reg<obj>(name)

// for backward compatibility
#define registerKernel(name)                        registerObject(name)
#define registerBoundaryCondition(name)             registerObject(name)
#define registerAux(name)                           registerObject(name)
#define registerMaterial(name)                      registerObject(name)
#define registerPostprocessor(name)                 registerObject(name)
#define registerInitialCondition(name)              registerObject(name)
#define registerDamper(name)                        registerObject(name)
#define registerDiracKernel(name)                   registerObject(name)
#define registerDGKernel(name)                      registerObject(name)
#define registerExecutioner(name)                   registerObject(name)
#define registerFunction(name)                      registerObject(name)
#define registerMeshModifier(name)                  registerObject(name)
#define registerConstraint(name)                    registerObject(name)
#define registerScalarKernel(name)                  registerObject(name)
#define registerUserObject(name)                    registerObject(name)
#define registerPreconditioner(name)                registerObject(name)
#define registerIndicator(name)                     registerObject(name)
#define registerMarker(name)                        registerObject(name)
#define registerProblem(name)                       registerObject(name)

#define registerNamedKernel(obj, name)              registerNamedObject(obj, name)
#define registerNamedBoundaryCondition(obj, name)   registerNamedObject(obj, name)
#define registerNamedAux(obj, name)                 registerNamedObject(obj, name)
#define registerNamedMaterial(obj, name)            registerNamedObject(obj, name)
#define registerNamedPostprocessor(obj, name)       registerNamedObject(obj, name)
#define registerNamedInitialCondition(obj, name)    registerNamedObject(obj, name)
#define registerNamedDamper(obj, name)              registerNamedObject(obj, name)
#define registerNamedDiracKernel(obj, name)         registerNamedObject(obj, name)
#define registerNamedDGKernel(obj, name)            registerNamedObject(obj, name)
#define registerNamedExecutioner(obj, name)         registerNamedObject(obj, name)
#define registerNamedFunction(obj, name)            registerNamedObject(obj, name)
#define registerNamedMeshModifier(obj, name)        registerNamedObject(obj, name)
#define registerNamedConstraint(obj, name)          registerNamedObject(obj, name)
#define registerNamedUserObject(obj, name)          registerNamedObject(obj, name)
#define registerNamedPreconditioner(obj, name)      registerNamedObject(obj, name)
#define registerNamedIndicator(obj, name)           registerNamedObject(obj, name)
#define registerNamedMarker(obj, name)              registerNamedObject(obj, name)


/**
 * Typedef for function to build objects
 */
typedef MooseObject * (*buildPtr)(const std::string & name, InputParameters parameters);

/**
 * Typedef for validParams
 */
typedef InputParameters (*paramsPtr)();

/**
 * Typedef for registered Object iterator
 */
typedef std::map<std::string, paramsPtr>::iterator registeredMooseObjectIterator;

/**
 * Build an object of type T
 */
template<class T>
MooseObject * buildObject(const std::string & name, InputParameters parameters)
{
  return new T(name, parameters);
}


/**
 * Generic factory class for build all sorts of objects
 */
class Factory
{
public:
  /**
   * Get the instance of the factory
   * @return Pointer to the factory instance
   */
  static Factory *instance();

  virtual ~Factory();

  /**
   * Release the memory allocated by this factory
   */
  static void release();

  /**
   * Register a new object
   * @param name Name of the object to register
   */
  template<typename T>
  void reg(const std::string & name)
  {
    if (_name_to_build_pointer.find(name) == _name_to_build_pointer.end())
    {
      _name_to_build_pointer[name] = &buildObject<T>;
      _name_to_params_pointer[name] = &validParams<T>;
    }
    else
      mooseError("Object '" + name + "' already registered.");
  }

  /**
   * Get valid parameters for the object
   * @param name Name of the object whose parameter we are requesting
   * @return Parameters of the object
   */
  InputParameters getValidParams(const std::string & name);

  /**
   * Build an object (must be registered)
   * @param obj_name Type of the object being constructed
   * @param name Name for the object
   * @param parameters Parameters this object should have
   * @return The created object
   */
  virtual MooseObject *create(const std::string & obj_name, const std::string & name, InputParameters parameters);

  registeredMooseObjectIterator registeredObjectsBegin() { return _name_to_params_pointer.begin(); }
  registeredMooseObjectIterator registeredObjectsEnd() { return _name_to_params_pointer.end(); }

protected:
  std::map<std::string, buildPtr>  _name_to_build_pointer;
  std::map<std::string, paramsPtr> _name_to_params_pointer;

  static Factory *_instance;

private:
  // Private constructor for singleton pattern
  Factory() {}

};

#endif /* FACTORY_H */
