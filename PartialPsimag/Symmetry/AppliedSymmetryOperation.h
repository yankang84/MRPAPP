//-*-C++-*-

#ifndef  Psimag_APPLIED_SYMMETRY_ELEMENT
#define  Psimag_APPLIED_SYMMETRY_ELEMENT

/** \ingroup symmetryConcepts */
/*@{*/

/** \file AppliedSymmetryElement.h
 *
 *  Contains a class for implementing symmetry elements.
 *
 */
 
#include <iostream>
#include <sstream>
#include <string>

#include "Vec.h"
#include "Mat.h"
#include "PSIMAGAssert.h"

#include "FieldConvert.h"

#include "SeitzMatrix.h"
#include "SeitzVectors.h"
#include "CellPosition.h"
#include "LatticeCoordinates.h"
#include "CellTranslation.h"
#include "CellRotation.h"
#include "SymmetryElement.h"

namespace psimag {
  

  /** \ingroup symmetryConcepts
   *
  * \brief A class for implementing SymmetryElements which record
  *        their application to a pattern.
  *
  * When a SymmetryElement is applied to a LatticeWithPattern we can construct:
  * 
  * - A SymmetryOperation which represents the SymmetryElement applied
  *   to a given Lattice,
  *
  * - A TestPattern which is the image of the given Pattern under this
  *   objects SymmetryOperation.
  */
  template<typename Field, size_t DIM, 
	   typename Ocupant, 
	   typename LatticeType,
	   typename Algorithms>
  class AppliedSymmetryElement: 
    public SymmetryElement< Field, DIM, Algorithms> 
  {
    
  public:

    typedef SeitzMatrix< Field, DIM >                              MatrixType;
    typedef SymmetryOperation<Field,DIM,Algorithms>                SymmetryOperationType;
    typedef SymmetryElement<Field,DIM,Algorithms>                  SymmetryElementType;
    typedef TestPattern<Field,DIM,Occupant,LatticeType,Algorithms> TestPatternType;
    typedef CellTranslation<Field,DIM>                             CellTranslationType;
    typedef CartesianTranslation<Field,DIM>                        CartesianTranslationType;
    typedef CartesianPosition<Field,DIM>                           CartesianPositionType;
    typedef LatticeCoordinates<DIM>                                LatticeCoordinatesType;

    CartesianTranslationType cartesianTranslation;
    CartesianPositionType    cartesianPosition;

    SymmetryOperationType    operation;

    TestPatternType          testPattern;
    Field                    latticeDistance;
    Field                    patternDistance;
    Field                    distance;
    int                      id;

    //====================================================================== Constructors
    /**
    * \brief  The default constructor sets to identity
    */
    AppliedSymmetryElement(): 
      SymmetryElementType  (),
      cartesianTranslation (),
      cartesianPosition    (),
      operation            (),
      testPattern          (),
      latticeDistance      (Field(0)),
      patternDistance      (Field(0)),
      distance             (Field(0)),
      id                   (-1)
    {}

    /**
    * \brief  The copy constructor 
    */
    AppliedSymmetryElement(const AppliedSymmetryElement& other): 
      SymmetryElementType  (other), 
      cartesianTranslation (other.cartesianTranslation),
      cartesianPosition    (other.cartesianPosition),
      operation            (other.operation),
      testPattern          (other.testPattern),
      latticeDistance      (other.latticeDistance),
      patternDistance      (other.patternDistance),
      distance             (other.distance),
      id                   (other.id)
    {}

    /**
    * \brief The constructor which applies a given SymmetryElement
    *        (and its associated SymmetryOperation) to a
    *        latticeWithPattern.
    */
    AppliedSymmetryOperation(int index,
			     const LatticeWithPatternType& latpat,
			     const SymmetryElement& symEl): 
      SymmetryElementType (symEl), 
      cartesianTranslation(latpat.cartesianTranslation(this->netDirection)),
      cartesianPosition   (latpat.cartesianPosition   (this->cellOffset)  ),
      operation           (symEl.operation(latpat)),
      testPattern         (latpat,this->operation),
      latticeDistance     (latpat.symmetryDistance(this->operation)),
      patternDistance     (this->testPattern.distanceTo(latpat)),
      distance            (std::max(this->latticeDistance, this->patternDistance)),
      id                  (index)
    {}

    //======================================================================
    //
    /**
    * \brief Assignment OperatorSet the fields representing the 'distance' between a
    *        target pattern generated by this operations and a given
    *        pattern.
    * 
    */
    AppliedSymmetryElelment& operator = (const AppliedSymmetryElement& op) {

      SymmetryElementType&       tgtElement = (*this);
      const SymmetryElementType& srcElelment= op;

      cartesianTranslation = op.cartesianTranslation;
      cartesianPosition    = op.cartesianPosition;
      operation            = op.operation;
      testPattern          = op.testPattern;
      latticeDistance      = op.latticeDistance;
      patternDistance      = op.patternDistance;
      distance             = op.distance;
      id                   = op.id;
      tgtElement           = srcElelment;

      return (*this);
    }
  };
    
  //====================================================================== 
  /** \ingroup XML
   *
   * SpaceGroup  XML output
   **/
  template<typename Field, size_t DIM, typename Occupant, typename LatticeType, typename Algorithms> 
  Tag toXML(const AppliedSymmetryOperation<Field,DIM,Occupant,LatticeType,Algorithms>& symEl,
	    std::string name="AppliedSymmetryOperation") {

    typedef SymmetryElement<Field,DIM,Algorithms>   SymmetryElementType;
    SymmetryElementType&       asElement = symEl;

    Tag result = toXML(asElement, name);

    result["latticeDistance"]  = symEl.latticeDistance;
    result["patternDistance"]  = symEl.patternDistance;
    result["distance"]         = symEl.distance;
    result["id"]               = symEl.id;

    result.add(toXML(symEl.cartesianTranslation,"CartesianDirection"));
    result.add(toXML(symEl.cartesianPosition   ,"CartesianPosition"));
    result.add(toXML(symEl.operation));
    result.add(toXML(symEl.testPattern));

  }

  //====================================================================== 
  /** \ingroup XML
   *
   * SpaceGroup  XML output
   **/
  template<typename Field, size_t DIM, typename Occupant, typename LatticeType, typename Algorithms, > 
  Tag toXML(const AppliedSymmetryElement<Field,DIM,Occupant,LatticeType,Algorithms>& symOp,
	    std::string name="AppliedSymmetryElement") {

    typedef SymmetryElement<Field,DIM,Algorithms>   SymmetryElementType;
    
    SymmetryElementType& asElement = (*this);
    Tag                  result = toXML(asElement,name);

    result.add(toXML(lat.cartesianTranslation(symOp.element.netDirection),"CartesianDirection"));
    result.add(toXML(lat.cartesianPosition   (symOp.element.cellOffset)  ,"CartesianPosition"));

    return result;
  }

  //====================================================================== 
  /** \ingroup ostream
   *
   * \brief  SymmetryOperation output stream operator.
   *
   */
  template<typename Field,size_t DIM, typename Algorithms>
  std::ostream& operator << (std::ostream& os, const SymmetryOperation<Field,DIM,Algorithms>& S) {
      
    os << "Symmetry Operation:  " << S.name << "------------------------------" << std::endl;
    os << static_cast<SeitzMatrix< Field, DIM > >(S) << std::endl;
    os << "End Symmetry Operation ------------------------------" << std::endl;
    return os;
  }

} /* namespace psimag */

#endif   //Psimag_SYMMETRY_OPERATION

/*@}*/