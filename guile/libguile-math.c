#include <math.h>
#include <libguile.h>

SCM sqrt_wrapper(SCM x)
{
	return scm_from_double(sqrt(scm_to_double(x)));
}

SCM square_wrapper(SCM x)
{
	double t = scm_to_double(x);
	return scm_from_double(t*t);
}

void init_math ()
{
	scm_c_define_gsubr("mysqrt", 1, 0, 0, sqrt_wrapper);
	scm_c_define_gsubr("mysquare", 1, 0, 0, square_wrapper);
}