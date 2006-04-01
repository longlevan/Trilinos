// @HEADER
// ***********************************************************************
// 
//    Thyra: Interfaces and Support for Abstract Numerical Algorithms
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

#include "Thyra_EpetraModelEvaluator.hpp"
#include "Thyra_EpetraThyraWrappers.hpp"
#include "Thyra_EpetraLinearOp.hpp"

namespace Thyra {

// Constructors/initializers/accessors.

EpetraModelEvaluator::EpetraModelEvaluator()
{}

EpetraModelEvaluator::EpetraModelEvaluator(
  const Teuchos::RefCountPtr<const EpetraExt::ModelEvaluator>         &epetraModel
  ,const Teuchos::RefCountPtr<LinearOpWithSolveFactoryBase<double> >  &W_factory
  )
{
  initialize(epetraModel,W_factory);
}

void EpetraModelEvaluator::initialize(
  const Teuchos::RefCountPtr<const EpetraExt::ModelEvaluator>         &epetraModel
  ,const Teuchos::RefCountPtr<LinearOpWithSolveFactoryBase<double> >  &W_factory
  )
{
  epetraModel_ = epetraModel;
  //
  W_factory_ = W_factory;
  //
  x_space_ = create_MPIVectorSpaceBase( x_map_ = epetraModel_->get_x_map() );
  f_space_ = create_MPIVectorSpaceBase( f_map_ = epetraModel_->get_f_map() );
  //
  EpetraExt::ModelEvaluator::InArgs inArgs = epetraModel_->createInArgs();
  p_map_.resize(inArgs.Np()); p_space_.resize(inArgs.Np());
  for( int l = 0; l < static_cast<int>(p_space_.size()); ++l )
    p_space_[l] = create_MPIVectorSpaceBase( p_map_[l] = epetraModel_->get_p_map(l) );
  //
  EpetraExt::ModelEvaluator::OutArgs outArgs = epetraModel_->createOutArgs();
  g_map_.resize(outArgs.Ng()); g_space_.resize(outArgs.Ng());
  for( int j = 0; j < static_cast<int>(g_space_.size()); ++j )
    g_space_[j] = create_MPIVectorSpaceBase( g_map_[j] = epetraModel_->get_g_map(j) );
}

Teuchos::RefCountPtr<const EpetraExt::ModelEvaluator> EpetraModelEvaluator::getEpetraModel() const
{
  return epetraModel_;
}

void EpetraModelEvaluator::uninitialize(
  Teuchos::RefCountPtr<const EpetraExt::ModelEvaluator>         *epetraModel
  ,Teuchos::RefCountPtr<LinearOpWithSolveFactoryBase<double> >  *W_factory
  )
{
  if(epetraModel) *epetraModel = epetraModel_;
  if(W_factory) *W_factory = W_factory_;
  epetraModel_ = Teuchos::null;
  W_factory_ = Teuchos::null;
  if(W_factory_.get()) {
    W_factory_->setOStream(this->getOStream());
  }
}

// Overridden from ModelEvaulator.

int EpetraModelEvaluator::Np() const
{
  return p_space_.size();
}

int EpetraModelEvaluator::Ng() const
{
  return g_space_.size();
}

Teuchos::RefCountPtr<const VectorSpaceBase<double> >
EpetraModelEvaluator::get_x_space() const
{
  return x_space_;
}

Teuchos::RefCountPtr<const VectorSpaceBase<double> >
EpetraModelEvaluator::get_f_space() const
{
  return f_space_;
}

Teuchos::RefCountPtr<const VectorSpaceBase<double> >
EpetraModelEvaluator::get_p_space(int l) const
{
  TEST_FOR_EXCEPT( ! ( 0 <= l && l < this->Np() ) );
  return p_space_[l];
}

Teuchos::RefCountPtr<const VectorSpaceBase<double> >
EpetraModelEvaluator::get_g_space(int j) const
{
  TEST_FOR_EXCEPT( ! ( 0 <= j && j < this->Ng() ) );
  return g_space_[j];
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_x_init() const
{
  return create_MPIVectorBase( epetraModel_->get_x_init(), x_space_ );
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_p_init(int l) const
{
  TEST_FOR_EXCEPT( ! ( 0 <= l && l < this->Np() ) );
  return create_MPIVectorBase( epetraModel_->get_p_init(l), p_space_[l] );
}

double EpetraModelEvaluator::get_t_init() const
{
  return epetraModel_->get_t_init();
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_x_lower_bounds() const
{
  return create_MPIVectorBase( epetraModel_->get_x_lower_bounds(), x_space_ );
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_x_upper_bounds() const
{
  return create_MPIVectorBase( epetraModel_->get_x_upper_bounds(), x_space_ );
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_p_lower_bounds(int l) const
{
  TEST_FOR_EXCEPT( ! ( 0 <= l && l < this->Np() ) );
  return create_MPIVectorBase( epetraModel_->get_p_lower_bounds(l), p_space_[l] );
}

Teuchos::RefCountPtr<const VectorBase<double> >
EpetraModelEvaluator::get_p_upper_bounds(int l) const
{
  TEST_FOR_EXCEPT( ! ( 0 <= l && l < this->Np() ) );
  return create_MPIVectorBase( epetraModel_->get_p_upper_bounds(l), p_space_[l] );
}

double EpetraModelEvaluator::get_t_lower_bound() const
{
  return epetraModel_->get_t_lower_bound();
}

double EpetraModelEvaluator::get_t_upper_bound() const
{
  return epetraModel_->get_t_upper_bound();
}

Teuchos::RefCountPtr<LinearOpWithSolveBase<double> >
EpetraModelEvaluator::create_W() const
{
  TEST_FOR_EXCEPTION(
    W_factory_.get()==NULL, std::logic_error
    ,"Thyra::EpetraModelEvaluator::create_W(): Error, the client did not set a LinearOpWithSolveFactoryBase"
    " object for W!"
    );
  W_factory_->setOStream(this->getOStream());
  return W_factory_->createOp();
}

Teuchos::RefCountPtr<LinearOpBase<double> >
EpetraModelEvaluator::create_DfDp_op(int l) const
{
  TEST_FOR_EXCEPT(true);
  return Teuchos::null;
}

ModelEvaluatorBase::DerivativeMultiVector<double>
EpetraModelEvaluator::create_DfDp_mv(int l, EDerivativeMultiVectorOrientation orientation) const
{
  return DerivativeMultiVector<double>(
    orientation == DERIV_MV_BY_COL
    ?create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*f_map_,p_map_[l]->NumGlobalElements()))
      ,f_space_,p_space_[l] )
    :create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*p_map_[l],f_map_->NumGlobalElements()))
      ,p_space_[l],f_space_ )
    ,orientation
    );
}

Teuchos::RefCountPtr<LinearOpBase<double> >
EpetraModelEvaluator::create_DgDx_op(int j) const
{
  TEST_FOR_EXCEPT(true);
  return Teuchos::null;
}

ModelEvaluatorBase::DerivativeMultiVector<double>
EpetraModelEvaluator::create_DgDx_mv(int j, EDerivativeMultiVectorOrientation orientation) const
{
  return DerivativeMultiVector<double>(
    orientation == DERIV_MV_BY_COL
    ?create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*g_map_[j],x_map_->NumGlobalElements()))
      ,g_space_[j],x_space_ )
    :create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*x_map_,g_map_[j]->NumGlobalElements()))
      ,x_space_,g_space_[j] )
    ,orientation
    );
}

Teuchos::RefCountPtr<LinearOpBase<double> >
EpetraModelEvaluator::create_DgDp_op( int j, int l ) const
{
  TEST_FOR_EXCEPT(true);
  return Teuchos::null;
}

ModelEvaluatorBase::DerivativeMultiVector<double>
EpetraModelEvaluator::create_DgDp_mv( int j, int l, EDerivativeMultiVectorOrientation orientation ) const
{
  return DerivativeMultiVector<double>(
    orientation == DERIV_MV_BY_COL
    ?create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*g_map_[j],p_map_[l]->NumGlobalElements()))
      ,g_space_[j],p_space_[l] )
    :create_MPIMultiVectorBase(
      Teuchos::rcp(new Epetra_MultiVector(*p_map_[l],g_map_[j]->NumGlobalElements()))
      ,p_space_[l],g_space_[j] )
    ,orientation
    );
}

EpetraModelEvaluator::InArgs<double> EpetraModelEvaluator::createInArgs() const
{
  const EpetraExt::ModelEvaluator &epetraModel = *epetraModel_;
  InArgsSetup<double> inArgs;
  typedef EpetraExt::ModelEvaluator EME;
  EME::InArgs epetraInArgs = epetraModel.createInArgs();
  inArgs.setModelEvalDescription(this->description());
  inArgs.set_Np(epetraInArgs.Np());
  inArgs.setSupports(IN_ARG_x_dot,epetraInArgs.supports(EME::IN_ARG_x_dot));
  inArgs.setSupports(IN_ARG_x,epetraInArgs.supports(EME::IN_ARG_x));
  inArgs.setSupports(IN_ARG_x_dot_poly,
		     epetraInArgs.supports(EME::IN_ARG_x_dot_poly));
  inArgs.setSupports(IN_ARG_x_poly,epetraInArgs.supports(EME::IN_ARG_x_poly));
  inArgs.setSupports(IN_ARG_t,epetraInArgs.supports(EME::IN_ARG_t));
  inArgs.setSupports(IN_ARG_alpha,epetraInArgs.supports(EME::IN_ARG_alpha));
  inArgs.setSupports(IN_ARG_beta,epetraInArgs.supports(EME::IN_ARG_beta));
  return inArgs;
}

EpetraModelEvaluator::OutArgs<double> EpetraModelEvaluator::createOutArgs() const
{
  const EpetraExt::ModelEvaluator &epetraModel = *epetraModel_;
  OutArgsSetup<double> outArgs;
  typedef EpetraExt::ModelEvaluator EME;
  EME::InArgs  epetraInArgs  = epetraModel.createInArgs();
  EME::OutArgs epetraOutArgs = epetraModel.createOutArgs();
  const int Np = epetraOutArgs.Np(), Ng = epetraOutArgs.Ng();
  outArgs.setModelEvalDescription(this->description());
  outArgs.set_Np_Ng(Np,Ng);
  outArgs.setSupports(OUT_ARG_f,epetraOutArgs.supports(EME::OUT_ARG_f));
  outArgs.setSupports(OUT_ARG_W,epetraOutArgs.supports(EME::OUT_ARG_W));
  outArgs.set_W_properties(convert(epetraOutArgs.get_W_properties()));
  for(int l=0; l<Np; ++l) {
    outArgs.setSupports(OUT_ARG_DfDp,l,convert(epetraOutArgs.supports(EME::OUT_ARG_DfDp,l)));
    outArgs.set_DfDp_properties(l,convert(epetraOutArgs.get_DfDp_properties(l)));
  }
  for(int j=0; j<Ng; ++j) {
    outArgs.setSupports(OUT_ARG_DgDx,j,convert(epetraOutArgs.supports(EME::OUT_ARG_DgDx,j)));
    outArgs.set_DgDx_properties(j,convert(epetraOutArgs.get_DgDx_properties(j)));
  }
  for(int j=0; j<Ng; ++j) for(int l=0; l<Np; ++l) {
    outArgs.setSupports(OUT_ARG_DgDp,j,l,convert(epetraOutArgs.supports(EME::OUT_ARG_DgDp,j,l)));
    outArgs.set_DgDp_properties(j,l,convert(epetraOutArgs.get_DgDp_properties(j,l)));
  }
  outArgs.setSupports(OUT_ARG_f_poly,epetraOutArgs.supports(EME::OUT_ARG_f_poly));
  return outArgs;
}

void EpetraModelEvaluator::evalModel( const InArgs<double>& inArgs, const OutArgs<double>& outArgs ) const
{

  using Thyra::get_Epetra_Vector;
  using Teuchos::RefCountPtr;
  using Teuchos::OSTab;

  typedef EpetraExt::ModelEvaluator EME;

  const Teuchos::RefCountPtr<Teuchos::FancyOStream> out       = this->getOStream();
  const Teuchos::EVerbosityLevel                    verbLevel = this->getVerbLevel();
  Teuchos::OSTab tab(out);
  if(out.get() && static_cast<int>(verbLevel) >= static_cast<int>(Teuchos::VERB_LOW))
    *out << "\nEntering Thyra::EpetraModelEvaluator::evalModel(...) ...\n";

  typedef Teuchos::VerboseObjectTempState<LinearOpWithSolveFactoryBase<double> > VOTSLOWSF;
  VOTSLOWSF W_factory_outputTempState(W_factory_,out,verbLevel);
  
  // InArgs

  EME::InArgs epetraInArgs = epetraModel_->createInArgs();

  RefCountPtr<const VectorBase<double> > x_dot;
  if( inArgs.supports(IN_ARG_x_dot) && (x_dot = inArgs.get_x_dot()).get() )
    epetraInArgs.set_x_dot(get_Epetra_Vector(*x_map_,x_dot));

  RefCountPtr<const VectorBase<double> > x;
  if( inArgs.supports(IN_ARG_x) && (x = inArgs.get_x()).get() )
    epetraInArgs.set_x(get_Epetra_Vector(*x_map_,x));

  if(1) {
    RefCountPtr<const VectorBase<double> > p_l;
    for(int l = 0;  l < outArgs.Np(); ++l ) {
      p_l = inArgs.get_p(l);
      if(p_l.get()) epetraInArgs.set_p(l,get_Epetra_Vector(*p_map_[l],p_l));
    }
  }

  RefCountPtr<const Teuchos::Polynomial< VectorBase<double> > > x_dot_poly;
  Teuchos::RefCountPtr<Epetra_Vector> epetra_ptr;
  if( inArgs.supports(IN_ARG_x_dot_poly) && \
      (x_dot_poly = inArgs.get_x_dot_poly()).get() ) {
    RefCountPtr<Teuchos::Polynomial<Epetra_Vector> > epetra_x_dot_poly = 
      Teuchos::rcp(new Teuchos::Polynomial<Epetra_Vector>(x_dot_poly->degree()));
    for (unsigned int i=0; i<=x_dot_poly->degree(); i++) {
      epetra_ptr = 
	Teuchos::rcp_const_cast<Epetra_Vector>(get_Epetra_Vector(*x_map_, x_dot_poly->getCoefficient(i)));
      epetra_x_dot_poly->setCoefficientPtr(i,epetra_ptr);
    }
    epetraInArgs.set_x_dot_poly(epetra_x_dot_poly);
  }

  RefCountPtr<const Teuchos::Polynomial< VectorBase<double> > > x_poly;
  if( inArgs.supports(IN_ARG_x_poly) && \
      (x_poly = inArgs.get_x_poly()).get() ) {
    RefCountPtr<Teuchos::Polynomial<Epetra_Vector> > epetra_x_poly = 
      Teuchos::rcp(new Teuchos::Polynomial<Epetra_Vector>(x_poly->degree()));
    for (unsigned int i=0; i<=x_poly->degree(); i++) {
      epetra_ptr = 
	Teuchos::rcp_const_cast<Epetra_Vector>(get_Epetra_Vector(*x_map_, x_poly->getCoefficient(i)));
      epetra_x_poly->setCoefficientPtr(i,epetra_ptr);
    }
    epetraInArgs.set_x_poly(epetra_x_poly);
  }

  if( inArgs.supports(IN_ARG_t) )
    epetraInArgs.set_t(inArgs.get_t());

  if( inArgs.supports(IN_ARG_alpha) )
    epetraInArgs.set_alpha(inArgs.get_alpha());

  if( inArgs.supports(IN_ARG_beta) )
    epetraInArgs.set_beta(inArgs.get_beta());

  // OutArgs

  EME::OutArgs epetraOutArgs = epetraModel_->createOutArgs();

  RefCountPtr<VectorBase<double> > f;
  if( outArgs.supports(OUT_ARG_f) && (f = outArgs.get_f()).get() )
    epetraOutArgs.set_f(get_Epetra_Vector(*f_map_,f));

  if(1){
    Teuchos::RefCountPtr<VectorBase<double> > g_j;
    for(int j = 0;  j < outArgs.Ng(); ++j ) {
      g_j = outArgs.get_g(j);
      if(g_j.get()) epetraOutArgs.set_g(j,get_Epetra_Vector(*g_map_[j],g_j));
    }
  }
  
  RefCountPtr<LinearOpWithSolveBase<double> > W;
  RefCountPtr<const LinearOpBase<double> >    fwdW;
  Teuchos::RefCountPtr<Epetra_Operator>       eW;
  if( outArgs.supports(OUT_ARG_W) && (W = outArgs.get_W()).get() ) {
    W_factory_->uninitializeOp(&*W,&fwdW);
    if( fwdW.get() ) {
      eW = const_cast<EpetraLinearOp&>(Teuchos::dyn_cast<const EpetraLinearOp>(*fwdW)).epetra_op();
    }
    else {
      eW = epetraModel_->create_W();
    }
    epetraOutArgs.set_W(eW);
  }

  if(1){
    Derivative<double> DfDp_l;
    for(int l = 0;  l < outArgs.Np(); ++l ) {
      if( !outArgs.supports(OUT_ARG_DfDp,l).none() && !(DfDp_l = outArgs.get_DfDp(l)).isEmpty() )
        epetraOutArgs.set_DfDp(l,convert(DfDp_l,f_map_,p_map_[l]));
    }
  }

  if(1){
    Derivative<double> DgDx_j;
    for(int j = 0;  j < outArgs.Ng(); ++j ) {
      if( !outArgs.supports(OUT_ARG_DgDx,j).none() && !(DgDx_j = outArgs.get_DgDx(j)).isEmpty() )
        epetraOutArgs.set_DgDx(j,convert(DgDx_j,g_map_[j],x_map_));
    }
  }

  if(1){
    Derivative<double> DgDp_j_l;
    for(int j = 0;  j < outArgs.Ng(); ++j ) {
      for(int l = 0;  l < outArgs.Np(); ++l ) {
        if( !outArgs.supports(OUT_ARG_DgDp,j,l).none() && !(DgDp_j_l = outArgs.get_DgDp(j,l)).isEmpty() )
          epetraOutArgs.set_DgDp(j,l,convert(DgDp_j_l,g_map_[j],p_map_[l]));
      }
    }
  }

  RefCountPtr<const Teuchos::Polynomial< VectorBase<double> > > f_poly;
  if( outArgs.supports(OUT_ARG_f_poly) && \
      (f_poly = outArgs.get_f_poly()).get() ) {
    RefCountPtr<Teuchos::Polynomial<Epetra_Vector> > epetra_f_poly = 
      Teuchos::rcp(new Teuchos::Polynomial<Epetra_Vector>(f_poly->degree()));
    for (unsigned int i=0; i<=f_poly->degree(); i++) {
      epetra_ptr = 
	Teuchos::rcp_const_cast<Epetra_Vector>(get_Epetra_Vector(*f_map_,
								 f_poly->getCoefficient(i)));
      epetra_f_poly->setCoefficientPtr(i,epetra_ptr);
    }
    epetraOutArgs.set_f_poly(epetra_f_poly);
  }

  // Do the evaluation

  epetraModel_->evalModel(epetraInArgs,epetraOutArgs);

  // Postprocess arguments

  if( W.get() ) {
    if( !fwdW.get() ) {
      fwdW = Teuchos::rcp(new EpetraLinearOp(eW));
    }
    W_factory_->initializeOp(fwdW,&*W);
    W->setOStream(this->getOStream());
  }

  if(out.get() && static_cast<int>(verbLevel) >= static_cast<int>(Teuchos::VERB_LOW))
    *out << "\nLeaving Thyra::EpetraModelEvaluator::evalModel(...) ...\n";
  
}

// Public functions overridden from Teuchos::Describable

std::string EpetraModelEvaluator::description() const
{
  std::ostringstream oss;
  oss << "Thyra::EpetraModelEvaluator{";
  oss << "epetraModel=";
  if(epetraModel_.get())
    oss << "\'"<<epetraModel_->description()<<"\'";
  else
    oss << "NULL";
  oss << ",W_factory=";
  if(W_factory_.get())
    oss << "\'"<<W_factory_->description()<<"\'";
  else
    oss << "NULL";
  oss << "}";
  return oss.str();
}

} // namespace Thyra

//
// Non-member utility functions
//

Thyra::ModelEvaluatorBase::EDerivativeMultiVectorOrientation
Thyra::convert( const EpetraExt::ModelEvaluator::EDerivativeMultiVectorOrientation &mvOrientation )
{
  switch(mvOrientation) {
    case EpetraExt::ModelEvaluator::DERIV_MV_BY_COL :
      return ModelEvaluatorBase::DERIV_MV_BY_COL;
    case EpetraExt::ModelEvaluator::DERIV_TRANS_MV_BY_ROW :
      return ModelEvaluatorBase::DERIV_TRANS_MV_BY_ROW;
    default:
      TEST_FOR_EXCEPT(true);
  }
  return ModelEvaluatorBase::DERIV_MV_BY_COL; // Should never be called!
}

EpetraExt::ModelEvaluator::EDerivativeMultiVectorOrientation
Thyra::convert( const ModelEvaluatorBase::EDerivativeMultiVectorOrientation &mvOrientation )
{
  switch(mvOrientation) {
    case ModelEvaluatorBase::DERIV_MV_BY_COL :
      return EpetraExt::ModelEvaluator::DERIV_MV_BY_COL;
    case ModelEvaluatorBase::DERIV_TRANS_MV_BY_ROW :
      return EpetraExt::ModelEvaluator::DERIV_TRANS_MV_BY_ROW;
    default:
      TEST_FOR_EXCEPT(true);
  }
  return EpetraExt::ModelEvaluator::DERIV_MV_BY_COL; // Should never be called!
}

Thyra::ModelEvaluatorBase::DerivativeProperties
Thyra::convert( const EpetraExt::ModelEvaluator::DerivativeProperties &derivativeProperties )
{
  ModelEvaluatorBase::EDerivativeLinearity linearity;
  switch(derivativeProperties.linearity) {
    case EpetraExt::ModelEvaluator::DERIV_LINEARITY_UNKNOWN:
      linearity = ModelEvaluatorBase::DERIV_LINEARITY_UNKNOWN;
      break;
    case EpetraExt::ModelEvaluator::DERIV_LINEARITY_CONST:
      linearity = ModelEvaluatorBase::DERIV_LINEARITY_CONST;
      break;
    case  EpetraExt::ModelEvaluator::DERIV_LINEARITY_NONCONST:
      linearity = ModelEvaluatorBase::DERIV_LINEARITY_NONCONST;
      break;
    default:
      TEST_FOR_EXCEPT(true);
  }
  ModelEvaluatorBase::ERankStatus rank;
  switch(derivativeProperties.rank) {
    case EpetraExt::ModelEvaluator::DERIV_RANK_UNKNOWN:
      rank = ModelEvaluatorBase::DERIV_RANK_UNKNOWN;
      break;
    case EpetraExt::ModelEvaluator::DERIV_RANK_FULL:
      rank = ModelEvaluatorBase::DERIV_RANK_FULL;
      break;
    case EpetraExt::ModelEvaluator::DERIV_RANK_DEFICIENT:
      rank = ModelEvaluatorBase::DERIV_RANK_DEFICIENT;
      break;
    default:
      TEST_FOR_EXCEPT(true);
  }
  return ModelEvaluatorBase::DerivativeProperties(linearity,rank,derivativeProperties.supportsAdjoint);
}

Thyra::ModelEvaluatorBase::DerivativeSupport
Thyra::convert( const EpetraExt::ModelEvaluator::DerivativeSupport &derivativeSupport )
{
  ModelEvaluatorBase::DerivativeSupport ds;
  if(derivativeSupport.supports(EpetraExt::ModelEvaluator::DERIV_LINEAR_OP))
    ds.plus(ModelEvaluatorBase::DERIV_LINEAR_OP);
  if(derivativeSupport.supports(EpetraExt::ModelEvaluator::DERIV_MV_BY_COL))
    ds.plus(ModelEvaluatorBase::DERIV_MV_BY_COL);
  if(derivativeSupport.supports(EpetraExt::ModelEvaluator::DERIV_TRANS_MV_BY_ROW))
    ds.plus(ModelEvaluatorBase::DERIV_TRANS_MV_BY_ROW);
  return ds;
}

EpetraExt::ModelEvaluator::Derivative
Thyra::convert(
  const ModelEvaluatorBase::Derivative<double>        &derivative
  ,const Teuchos::RefCountPtr<const Epetra_Map>       &fnc_map
  ,const Teuchos::RefCountPtr<const Epetra_Map>       &var_map
  )
{
  typedef ModelEvaluatorBase MEB;
  if(derivative.getLinearOp().get()) {
    return EpetraExt::ModelEvaluator::Derivative(
      Teuchos::rcp_const_cast<Epetra_Operator>(
        Teuchos::dyn_cast<const EpetraLinearOp>(*derivative.getLinearOp()).epetra_op()
        )
      );
  }
  else if(derivative.getDerivativeMultiVector().getMultiVector().get()) {
    return EpetraExt::ModelEvaluator::Derivative(
      EpetraExt::ModelEvaluator::DerivativeMultiVector(
        get_Epetra_MultiVector(
          derivative.getDerivativeMultiVector().getOrientation() == MEB::DERIV_MV_BY_COL
          ? *fnc_map : *var_map
          ,derivative.getDerivativeMultiVector().getMultiVector()
          )
        ,convert(derivative.getDerivativeMultiVector().getOrientation())
        )
      );
  }
  return EpetraExt::ModelEvaluator::Derivative();
}

