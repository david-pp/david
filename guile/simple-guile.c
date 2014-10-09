/* simple-guile.c --- Start Guile from C.  */

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


static void
inner_main (void *closure, int argc, char **argv)
{
	init_math();

	scm_c_eval_string("(load \"loaded.scm\")");
	scm_c_eval_string("(testfun 'hello)");

  /* preparation */
  scm_shell (argc, argv);
  /* after exit */
}

int
main (int argc, char **argv)
{
  scm_boot_guile (argc, argv, inner_main, 0);
  return 0; /* never reached, see inner_main */
}