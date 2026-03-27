#ifndef BAYES_UPDATOR_H
#define BAYES_UPDATOR_H

/**
 * @brief Binary Bayes updator
 * @details https://blog.csdn.net/haishaoli/article/details/79647594
 * @param  value_update: pretime possibility
 *         value_observe: measurement possibility
 */

inline double BayesUpdator(double value_update, double value_observe, int miscount){
	double tem_value = 0;
  	if (value_update <=0 ) {
    	tem_value = value_observe;
    	return tem_value;
  	}

  	double tem_odds  = log(value_update/(1-value_update)) + miscount*log(value_observe/(1-value_observe));

  	tem_value = 1 - 1/(1+exp(tem_odds));

  	if(tem_value>0.9) tem_value = 0.9;
  	if(tem_value<0.2) tem_value = 0.2;
  	return tem_value;
}
   
#endif
