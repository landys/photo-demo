#include <iostream> 
#include <iomanip> 
#include <strstream> 
#include <math.h> 

#include "erf.h" 
#if 1 
#define signbit __signbit 
__inline int __signbit(long double fe) 
{ return (sizeof(long double) == sizeof(double)) 
? ((short *)&(fe))[3] & 0x8000 
: ((short *)&(fe))[4] & 0x8000; 
} 
#else 
#define signbit(fe)     (sizeof(fe) == sizeof(float) ?                  \ 
(int)(((short *)&(fe))[1] & 0x8000) :       \ 
(sizeof(fe) == sizeof(double) ? (int)(((short *)&(fe))[3] & 
 0x8000)  \ 
 : (int)(((short *)&(fe))[4] & 0x8000)) \ 
 ) 
#endif 
 static const double rel_error= 1E-12; //calculate 12 significant figures 
//you can adjust rel_error to trade off between accuracy and speed 
//but don't ask for > 15 figures (assuming usual 52 bit mantissa in a double) 

double erf(double x) 
//erf(x) = 2/sqrt(pi)*integral(exp(-t^2),t,0,x) 
// = 2/sqrt(pi)*[x - x^3/3 + x^5/5*2! - x^7/7*3! + ...] 
// = 1-erfc(x) 
{ 
	static const double two_sqrtpi= 1.128379167095512574; // 2/sqrt(pi) 
	if (fabs(x) > 2.2) { 
		return 1.0 - erfc(x); //use continued fraction when fabs(x) > 2.2 

	} 

	double sum= x, term= x, xsqr= x*x; 
	int j= 1; 
	do { 
		term*= xsqr/j; 
		sum-= term/(2*j+1); 
		++j; 
		term*= xsqr/j; 
		sum+= term/(2*j+1); 
		++j; 
	} while (fabs(term/sum) > rel_error); // CORRECTED LINE 

	return two_sqrtpi*sum; 

} 

double erfc(double x) 
//erfc(x) = 2/sqrt(pi)*integral(exp(-t^2),t,x,inf) 
// = exp(-x^2)/sqrt(pi) * [1/x+ (1/2)/x+ (2/2)/x+ (3/2)/x+ (4/2)/x+ ...] 
// = 1-erf(x) 
//expression inside [] is a continued fraction so '+' means add to denominator only 
{ 
	static const double one_sqrtpi= 0.564189583547756287; // 1/sqrt(pi) 
	if (fabs(x) < 2.2) { 
		return 1.0 - erf(x); //use series when fabs(x) < 2.2 
	} 

	if (signbit(x)) { //continued fraction only valid for x>0 
		return 2.0 - erfc(-x); 
	} 

	double a=1, b=x; //last two convergent numerators 
	double c=x, d=x*x+0.5; //last two convergent denominators 
	double q1, q2= b/d; //last two convergents (a/c and b/d) 
	double n= 1.0, t; 
	do { 
		t= a*n+b*x; 
		a= b; 
		b= t; 
		t= c*n+d*x; 
		c= d; 
		d= t; 
		n+= 0.5; 
		q1= q2; 
		q2= b/d; 
	} while (fabs(q1-q2)/q2 > rel_error); 

	return one_sqrtpi*exp(-x*x)*q2; 
}