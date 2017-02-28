$Id: README.TXT,v 1.1 2000/06/06 01:44:34 dv Exp $

This is the source code for the Meta Object Protocol (MOP) presented
at various occasions.

There are three directories:
	mop - the base MOP library
	catmop - cataloguing application
	xmlmop - XML application

The code was tested on
	Linux with gcc version 2.96 20000402 (experimental) with
	  SGI's C++ I/O library (experimental)
	Windows NT with the free Borland compiler bcc 5.5
	Windows NT with Visual C++ 6.0 (SP3 + Dinkumware patches)

To build the sample applications, first edit the makefiles in the
directories according to your environment (for VC++, a workspace
file is provided).  Then build first the library files in mop.
Afterwards you can build the applications.

If you have any questions or suggestions, just mail me at dv@vollmann.ch.

The most recent version of this software should be available at
http://www.vollmann.ch/.
