#pragma once

#include "perception_fusion_common/Xlidar_mot/kalman/ekfilter.h"
using namespace Kalman;

#define  Num_Measure  8 //tracker measure num.
#define  Num_State    6 //tracker State num.

//!  template<typename T, K_UINT_32 BEG, bool OQ = false, bool OVR = false, bool DBG = true>
//! - \c T : Type of elements contained in matrices and vectors. Usually
//!          \c float or \c double.
//! - \c BEG : Starting index of matrices and vectors. Can be either 0 or 1.
//! - \c OQ : Optimize calculations on \a Q. This can be turned on if \a Q
//!           is diagonal.
//! - \c OVR : Optimize calculations on \a V and \a R. This can be turned on
//!            if \a V and \a R are both diagonal matrices.
//! - \c DGB : Debug flag. If \c true, then bound-checking will be performed,
//!            and \c OutOfBoundError exceptions can be thrown.

namespace avos {
namespace perception {
namespace xlidarMOT{

class cObjfusEKF : public Kalman::EKFilter<double, 0, true, true, true> {
public:
	cObjfusEKF();
	void resetR( Vector );
	void resetQ( Vector );
	void resetPeriod( double );

protected:
	// void makeBaseA();
	// void makeBaseH();
	void makeBaseW();
	void makeBaseQ();
	void makeBaseV();
	//void makeBaseR();


	void makeA();
	void makeH();
	void makeR();
	void makeProcess();
	void makeMeasure();
	Vector tmp_R;
	Vector tmp_Q;
	double Period;
	double half_p_2;
};
typedef cObjfusEKF::Vector MyVector;
typedef cObjfusEKF::Matrix MyMatrix;

}
}
}

