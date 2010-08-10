// @HEADER
// ***********************************************************************
// 
//                    Teuchos: Common Tools Package
//                 Copyright (2004) Sandia Corporation
// 
// Under terms of Contract DE-AC04-94AL85000, there is a non-exclusive
// license for use of this work by or on behalf of the U.S. Government.
// 
// This library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as
// published by the Free Software Foundation; either version 2.1 of the
// License, or (at your option) any later version.
//  
// This library is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//  
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
// USA
// Questions? Contact Michael A. Heroux (maherou@sandia.gov) 
// 
// ***********************************************************************
// @HEADER


#ifndef TEUCHOS_STANDARDCONDITION_HPP_
#define TEUCHOS_STANDARDCONDITION_HPP_

/*! \file Teuchos_StandardConditions.hpp
    \brief Standard Conditions to be used.
*/

#include "Teuchos_Condition.hpp"
#include "Teuchos_InvalidConditionException.hpp"
#include "Teuchos_ParameterList.hpp"

namespace Teuchos{

/**
 * \brief An abstract parent class for all Binary Logic Conditions.
 *
 * Binary Logic Conditions return the result of performing some
 * Logical operation on a set of conditions. Note that although the
 * name implies the evaluation of two conditions, Binary Logic Conditions
 * can actually evaluate and arbiturary number of conditions.
 */
class BinaryLogicalCondition : public Condition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a BinaryLogicCondition
   *
   * \param conditions The conditions to be evaluated.
   */
  BinaryLogicalCondition(ConstConditionList& conditions);

  //@}

  /**
   * \brief Deconstructor for a BinaryLogicCondition
   */
  virtual ~BinaryLogicalCondition(){}

  //@}
  
  /** \name Modifier Functions */

  //@{

  /**
   * \brief Adds a Condition to the list of conditions that will
   * be evaluated by this Binary Logical Condition.
   *
   * \param toAdd The condition to be added to the list of
   * conditions this Binary Logic Condition will evaluate.
   */
  void addCondition(RCP<const Condition> toAdd);

  //@}

  //! @name Attribute/Query Methods 
  //@{

  /**
   * \brief Applies a Binary Logic operator to two operands and returns the
   * result.
   *
   * \param op1 The first operand.
   * \param op2 The second operand.
   * \return The result of applying a binary logical operator to
   * the two operands.
   */
  virtual bool applyOperator(bool op1, bool op2) const = 0;

  /**
   * \brief Gets a list of all conditions that are a part of this 
   * BinaryLogicalCondition/
   */
  inline
  const ConstConditionList& getConditions() const{
    return conditions_;
  }

  //@}

  /** \name Overridden from Condition */
  //@{

  /** \brief . */
  virtual bool isConditionTrue() const;

  /** \brief . */
  bool containsAtLeasteOneParameter() const;

  /** \brief . */
  Dependency::ConstParameterEntryList getAllParameters() const;

  //@}

private:

  /** \name Private Members */
  //@{
  
  /*
   * \brief A list of conditions on which to perform some logic operation.
   */
  ConstConditionList conditions_;

  //@}

};

/**
 * \brief A Binary Logic Condition that returns the result
 * or perfroming a logical OR on the conditions.
 */
class OrCondition : public BinaryLogicalCondition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs an Or Condition
   *
   * @param conditions The conditions to be evaluated.
   */
  OrCondition(ConstConditionList& conditions);

  /**
   * \brief Deconstructs an Or Condition.
   */
  virtual ~OrCondition(){}

  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return "orCondition";
  }
  
  //@}

  /** \name Overridden from BinaryLogicalCondition */
  //@{

  /** \brief . */
  bool applyOperator(bool op1, bool op2) const;
  
  //@}

};

/**
 * \brief A Binary Logic Condition that returns the result
 * or perfroming a logical AND on the conditions.
 */
class AndCondition : public BinaryLogicalCondition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs an And Condition
   *
   * @param conditions The conditions to be evaluated.
   */
  AndCondition(ConstConditionList& conditions);

  /**
   * \brief Deconstructs an And Condition.
   */
  virtual ~AndCondition(){}
  
  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return "andCondition";
  }
  
  //@}


  /** \name Overridden from BinaryLogicalCondition */
  //@{

  /** \brief . */
  bool applyOperator(bool op1, bool op2) const;
  
  //@}

};

/**
 * \brief A Binary Logic Condition that returns the result
 * or perfroming a logical EQUALS on the conditions.
 */
class EqualsCondition : public BinaryLogicalCondition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs an Equals Condition
   *
   * @param conditions The conditions to be evaluated.
   */
  EqualsCondition(ConstConditionList& conditions);

  /**
   * \brief Deconstructs an Equals Condition.
   */
  virtual ~EqualsCondition(){}
  
  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return "equalsCondition";
  }
  
  //@}

  /** \name Overridden from BinaryLogicalCondition */
  //@{

  /** \brief . */
  bool applyOperator(bool op1, bool op2) const;
  
  //@}

};

/**
 * \brief A Not condition returns the result of
 * performing a logical NOT on a given
 * condition.
 */
class NotCondition : public Condition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a Not Condition
   *
   * @param condition The condition to be evaluated.
   */
  NotCondition(RCP<const Condition> condition);

  /**
   * \brief Deconstructs a Not Condition.
   */
  virtual ~NotCondition(){}
  
  //@}

  /** \name Attribute/Query Functions */
  //@{

  /** \brief Retrieve the child condition */
  RCP<const Condition> getChildCondition(){
    return childCondition_;
  }
  
  //@}

  /** \name Overridden from Condition */
  //@{

  /** \brief . */
  bool isConditionTrue() const;

  /** \brief . */
  bool containsAtLeasteOneParameter() const;

  /** \brief . */
  Dependency::ConstParameterEntryList getAllParameters() const;

  std::string getTypeAttributeValue() const{
    return "notCondition";
  }
  
  //@}

private:

  /** \name Private Members */
  //@{
  
  /**
   * The condition on which to perfrom the logical NOT.
   */
  RCP<const Condition> childCondition_;
  
  //@}


};

/**
 * \brief An Abstract Base class for all ParameterConditions.
 *
 * A Parmaeter Condition examines the value of a given
 * parameter and returns a bool based on the condition of
 * that value.
 */
class ParameterCondition : public Condition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a Parameter Condition.
   *
   * @param Parameter The parameter to be evaluated.
   * @param whenParamEqualsValue Indicates whether the condition 
   * should be true when the evaluation
   * results in a true or when the evaluation results in a false. 
   * When set to true, if the parameter
   * evaluates to true then the condition will evaluate to true. If 
   * set to false if the parameter
   * evaluates to false, then the condition will evaluate to true.
   */
  ParameterCondition(
    RCP<ParameterEntry> parameter, bool whenParamEqualsValue);

  virtual ~ParameterCondition(){}
  
  //@}

  //! @name Attribute/Query Methods 
  //@{

  /**
   * Evaluate the current condition of a paramtere and
   * return the result.
   *
   * @param The result of evaluating the current condition
   * of the parameter.
   */
  virtual bool evaluateParameter() const = 0;

  /** \brief Gets a const pointer to the Parameter being
   *  evaluated by this ParameterCondition
   */
  inline RCP<const ParameterEntry> getParameter() const{
    return parameterEntry_.getConst();
  }

  /** \brief Gets the WhenParamEqualsValue */
  inline
  bool getWhenParamEqualsValue() const{
    return whenParamEqualsValue_;
  }
  
  //@}

  /** \name Overridden from Condition */
  //@{

  bool isConditionTrue() const{
    if((whenParamEqualsValue_ && evaluateParameter()) || 
      (!whenParamEqualsValue_ && evaluateParameter()))
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool containsAtLeasteOneParameter() const{
    return true;
  }

  Dependency::ConstParameterEntryList getAllParameters() const;                
  
  //@}

private:

  /** \name Private Members */
  //@{
  
  /**
   * Parameter to be evaluated.
   */
  RCP<ParameterEntry> parameterEntry_;


  /**
   * Wether or not the condition should evaluate to 
   * true if the parameter evaluated to true.
   */
  bool whenParamEqualsValue_;

  //@}

};

/**
 * \brief A String Condition is a Parameter Condition that evaluates
 * whether or not a string parameter has taken on a particular
 * value or set of values.
 */
class StringCondition : public ParameterCondition{

public:

  /** \name Public types */
  //@{

  /**
   * \brief Convience typedef representing an array of strings.
   */
  typedef Array<std::string> ValueList; 
  
  //@}

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a String Condition.
   *
   * @param parameter The parameter to be evaluated.
   * be evaluated.
   * @param value The value to compare the parameter's value against.
   * @param whenParamEqualsValue Indicates whether the condition 
   * should be true when the evaluation
   * results in a true or when the evaluation results in a false. 
   * When set to true, if the parameter
   * evaluates to true then the condition will evaluate to true.
   * If set to false if the parameter
   * evaluates to false, then the condition will evaluate to true.
   */
  StringCondition(
   RCP<ParameterEntry> parameter,
   std::string value, 
   bool whenParamEqualsValue=true);

  /**
   * \brief Constructs a String Condition.
   *
   * @param parameter The parameter to be evaluated.
   * @param values The values to compare the parameter's value against.
   * @param whenParamEqualsValue Indicates 
   * whether the condition should be true when the evaluation
   * results in a true or when the evaluation results in a false.
   * When set to true, if the parameter
   * evaluates to true then the condition will evaluate to true. If set
   * to false if the parameter
   * evaluates to false, then the condition will evaluate to true.
   */
  StringCondition(
    RCP<ParameterEntry> parameter, 
    ValueList values, 
    bool whenParamEqualsValue=true);

  virtual ~StringCondition(){}
  
  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return "stringCondition";
  }
  
  //@}

  /** \name Overridden from ParameterCondition */
  //@{

  bool evaluateParameter() const;
  
  //@}

  /** \name Attribute/Query Functions */
  //@{

    /** \brief Returns the value list being used with this StringCondition. */
    const ValueList& getValueList() const{
      return values_;
    }

  //@}

private:

  /** \name Private Members */
  //@{
  
  /**
   * A list of values against which to evaluate the parameter's value.
   */
  ValueList values_;
  
  //@}
  
};

/**
 * \brief A Number Condition is a Parameter Condition that evaluates
 * whether or not a number parameter is greater 0. 
 *
 * If the parameter is
 * greater than 0 this is interperted as the condition being "true".
 * Otherwise the oncidiont is interperted as false.
 */
template<class T>
class NumberCondition : public ParameterCondition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a Number Condition.
   *
   * @param parameterName The name of the parameter to be evaluated.
   * @param func A function to run the value of the parameter through. 
   * If the function returns a value
   * greater than 0, this will be interperted as the condition being "true". 
   * If the function returns a value of 0 or less, this will be interperted 
   * as the condition being false.
   * @param whenParamEqualsValue Indicates whether the condition should be 
   * true when the evaluation results in a true or when the evaluation results 
   * in a false. When set to true, if the parameter evaluates to true then 
   * the condition will evaluate to true. If seet to false if the parameter
   * evaluates to false, then the condition will evaluate to true.
   */
  NumberCondition(
    RCP<ParameterEntry> parameter,
    T (*func)(T)=0, 
    bool whenParamEqualsValue=true):
    ParameterCondition(parameter, whenParamEqualsValue), 
    func_(func)
  {
    checkForNumberType();
  }

  virtual ~NumberCondition(){}
  
  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return TypeNameTraits<T>::name() + "NumberCondition";
  }
  
  //@}

  /** \name Overridden from ParameterCondition */
  //@{

  bool evaluateParameter() const{
    return (runFunction(getValue<T>(*getParameter())) > 0);
  }
  
  //@}

private:

  /** \name Private Members */
  //@{
  
  /** \brief . */
  T (*func_)(T);   

  /**
   * \brief Runs the function associated with this condition and
   * returns the result.
   *
   * @param argument The value upon which to run the function.
   */
  inline T runFunction(T argument) const{
    if(func_ !=0)
      return (*func_)(argument);
    else
      return argument;
  }  

  /** \brief Checks to make sure the given parameter is a 
   * number type
   */
  void checkForNumberType() const{
  RCP<const ParameterEntry> toCheck = getParameter();
    TEST_FOR_EXCEPTION(
      !toCheck->isType<int>() &&
      !toCheck->isType<short>() &&
      !toCheck->isType<double>() &&
      !toCheck->isType<float>(),
      InvalidConditionException,
      "The parameter of a Number Condition "
      "must be of a supported number type!" << std::endl <<
      "Actual Parameter type: " << getParameter()->getAny().typeName() );
  }
  
  //@}

};

/**
 * \brief A Bool Condition is a Parameter Condition that evaluates
 * whether or not a Boolean parameter is ture.
 * */
class BoolCondition : public ParameterCondition{

public:

  /** \name Constructors/Destructor */
  //@{

  /**
   * \brief Constructs a Bool Condition.
   *
   * @param parameterName The name of the parameter to be evaluated.
   * @param whenParamEqualsValue Indicates whether the condition 
   * should be true when the evaluation
   * results in a true or when the evaluation results in a false. 
   * When set to true, if the parameter
   * evaluates to true then the condition will evaluate to true. 
   * If set to false if the parameter
   * evaluates to false, then the condition will evaluate to true.
   */
  BoolCondition(
    RCP<ParameterEntry>,
    bool whenParamEqualsValue=true);

  virtual ~BoolCondition(){}
  
  //@}

  /** \name Overridden from Condition */
  //@{

  std::string getTypeAttributeValue() const{
    return "boolCondition";
  }
  
  //@}

  /** \name Overridden from ParameterCondition */
  //@{

  bool evaluateParameter() const;
  
  //@}

};


} //namespace Teuchos


#endif //TEUCHOS_STANDARDCONDITION_HPP_
