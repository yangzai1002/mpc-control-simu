#ifndef DUBINS_CURVE_H_
#define DUBINS_CURVE_H_

#define EDUBOK        (0)   /* No error */
#define EDUBCOCONFIGS (1)   /* Colocated configurations */
#define EDUBPARAM     (2)   /* Path parameterisitation error */
#define EDUBBADRHO    (3)   /* the rho value is invalid */
#define EDUBNOPATH    (4)   /* no connection between configurations with this word */

#include "site.h"
#include "geoheader.h"
#include <math.h>
using namespace geometry;
typedef int (*DubinsPathSamplingCallback)(double q[3], double t, void* user_data);

namespace geometry
{
        typedef enum 
        {
            L_SEG = 0,
            S_SEG = 1,
            R_SEG = 2
        } SegmentType;

        /* The segment types for each of the Path types */
        const SegmentType DIRDATA[][3] = {
            { L_SEG, S_SEG, L_SEG },
            { L_SEG, S_SEG, R_SEG },
            { R_SEG, S_SEG, L_SEG },
            { R_SEG, S_SEG, R_SEG },
            { R_SEG, L_SEG, R_SEG },
            { L_SEG, R_SEG, L_SEG }
        };

        typedef struct 
        {
            double alpha;
            double beta;
            double d;
            double sa;
            double sb;
            double ca;
            double cb;
            double c_ab;
            double d_sq;
        } DubinsIntermediateResults;

        typedef enum 
        {
            LSL = 0,
            LSR = 1,
            RSL = 2,
            RSR = 3,
            RLR = 4,
            LRL = 5
        } DubinsPathType;

        typedef struct 
        {
            /* the initial configuration */
            double qi[3];        
            /* the lengths of the three segments */
            double param[3];     
            /* model forward velocity / model angular velocity */
            double rho;          
            /* the path type described */
            DubinsPathType type; 
        } DubinsPath;

        class DubinCurve{
        public:
            DubinCurve(){
                //dubins_path_.clear();
            }
            ~DubinCurve(){}
            bool GenerateDubinsPath(SiteVec& list, 
              const Site& start_points, 
              const Site& goal_points, 
              double turning_radius,
              double density = 0.1, int LOrR = 0);
              inline static std::vector<Site> dubins_path_ = {};
        private:
            static int SaveConfiguration(double q[3], double x, void* user_data);
        };

}
#include "dubins.hpp"

#endif /* DUBINS_H */