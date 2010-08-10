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

namespace Teuchos{

RCP<Dependency>
DependencyXMLConverter::fromXMLtoDependency(const XMLObject &xmlObj) const
{
  Dependency::ParameterParentMap dependees;
  Dependency::ParameterParentMap dependents;

  TEST_FOR_EXCEPTION(xmlObj.findFirstChild(getDependeeTagName()) == null,
    MissingDependeesException,
    "Could not find any dependees for a dependency!" 
    <<std::endl <<std::endl);

  TEST_FOR_EXCEPTION(xmlObj.findFirstChild(getDependentTagName()) == null,
    MissingDependentsException,
    "Could not find any dependents for a dependency!" 
    <<std::endl <<std::endl);

  for(int i=0; i<xmlObj.numChildren(); ++i){
    XMLObject child = xmlObj.getChild(i);
    if(child.getTag() == getDependeeTagName()){
      ParameterEntry::ParameterEntryID dependeeID = 
        child.getAttribute(getParameterIDAttributeName());

      dependees.insert(ParameterEntry::getParameterEntry(dependeeID));
    }
    else if(child.getTag() == getDependentTagName()){
      ParameterEntry::ParameterEntryID dependentID = 
        child.getAttribute(getParameterIDAttributeName());

      dependents.insert(ParameterEntry::getParameterEntry(dependentID));
    }
  }

  return convertXML(xmlObj, dependees, dependents);
}

XMLObject
DependencyXMLConverter::fromDependencytoXML(const RCP<const Dependency> dependency) const
{
  XMLObject toReturn(Dependency::getXMLTagName());

  toReturn.addAttribute(getTypeAttributeName(), getTypeAttributeValue());

  Dependency::ParameterParentMap::const_iterator it =
    dependency->getDependees().begin();

  for(;it != dependency.getDependees().end(): ++it){
    XMLObject currentDependee(getDependeeTagName());
    currentDependee.setAttribute(getParameterIDAttributeName(), 
      ParameterEntry::getParameterID(*it));
    toReturn.addChild(currentDependee);
  }

  it = dependency->getDependents().begin();
  for(; it != dependency->getDependents().end(); ++it){
    XMLObject currentDependent(getDependentTagName());
    currentDependent.setAttribute(getParameterIDAttributeName(), 
      ParameterEntry::getParameterID(*it));
    toReturn.addChild(currentDependent);
  }

  convertDependency(dependency, XMLObject);

  return toReturn;
}


} // namespace Teuchos

