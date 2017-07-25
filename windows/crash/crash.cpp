#include "stdafx.h"
#include "CrashHandler.h"


// https://www.codeproject.com/Articles/207464/Exception-Handling-in-Visual-Cplusplus

#include <float.h>
#include <crtdbg.h>
#include <Dbghelp.h>

void sigfpe_test()
{
	// Code taken from http://www.devx.com/cplus/Article/34993/1954

	//Set the x86 floating-point control word according to what
	//exceptions you want to trap. 
	_clearfp(); //Always call _clearfp before setting the control
	//word
	//Because the second parameter in the following call is 0, it
	//only returns the floating-point control word
	unsigned int cw;
	_controlfp_s(&cw, 0, 0); //Get the default control
	//word
	//Set the exception masks off for exceptions that you want to
	//trap.  When a mask bit is set, the corresponding floating-point
	//exception is //blocked from being generating.
	cw &= ~(EM_OVERFLOW | EM_UNDERFLOW | EM_ZERODIVIDE |
		EM_DENORMAL | EM_INVALID);
	//For any bit in the second parameter (mask) that is 1, the 
	//corresponding bit in the first parameter is used to update
	//the control word.  
	unsigned int cwOriginal;
	_controlfp_s(&cwOriginal, cw, MCW_EM); //Set it.
	//MCW_EM is defined in float.h.
	//Restore the original value when done:
	//_controlfp(cwOriginal, MCW_EM);

	// Divide by zero

	float a = 1;
	float b = 0;
	float c = a / b;
	c;
}

#define BIG_NUMBER 0x1fffffff
#pragma warning(disable: 4717) // avoid C4717 warning
int RecurseAlloc()
{
	int* pi = new int[BIG_NUMBER];
	pi;
	RecurseAlloc();
	return 0;
}

class CDerived;

class CBase
{
public:
	CBase(CDerived* derived) : m_pDerived(derived)
	{
	};
	~CBase();
	virtual void function(void) = 0;

	CDerived* m_pDerived;
};

#pragma warning(disable:4355)
class CDerived : public CBase
{
public:
	CDerived() : CBase(this)
	{
	}; // C4355
	virtual void function(void)
	{
	};
};

CBase::~CBase()
{
	m_pDerived->function();
}


void __cdecl Crash_InvalidParameterHandler(
	const wchar_t* expression,
	const wchar_t* function,
	const wchar_t* file,
	unsigned int line,
	uintptr_t pReserved)
{
	//	printf("%s:%s:%s:%u", expression, function, file, line);
}

//https://msdn.microsoft.com/en-us/library/ksazx244.aspx
int crash_printf(void)
{
	// Catch invalid parameter exceptions.
	//	_set_invalid_parameter_handler(Crash_InvalidParameterHandler);
	//	_set_thread_local_invalid_parameter_handler(Crash_InvalidParameterHandler);

	// Disable the message box for assertions.  
	//	_CrtSetReportMode(_CRT_ASSERT, 0);


	__try
	{
		unsigned long input[] = {1,45,0xf001,0xffffffff};
		int i = 0;
		char buf[5] = {0};
		for (i = 0; i < _countof(input); i++)
		{
			_ultoa_s(input[i], buf, sizeof(buf), 16);
			printf("%s\n", buf);
		}
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
	}

	return 1;
}


//https://www.codeproject.com/Articles/11132/Walking-the-callstack

void printStack(void)
{
	unsigned int i;
	void* stack[100];
	unsigned short frames;
	SYMBOL_INFO* symbol;
	HANDLE process;

	process = GetCurrentProcess();

	SymInitialize(process, NULL, TRUE);

	frames = CaptureStackBackTrace(0, 100, stack, NULL);
	symbol = (SYMBOL_INFO *)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	for (i = 0; i < frames; i++)
	{
		SymFromAddr(process, (DWORD64)(stack[i]), 0, symbol);

		printf("%i: %s - 0x%0X\n", frames - i - 1, symbol->Name, symbol->Address);
	}

	free(symbol);
}

void fun1()
{
	printStack();
}

void test_backtrace()
{
	fun1();
}

// https://msdn.microsoft.com/library/aa290051.aspx#vctchcompilersecuritychecksindepthanchor5
// /GSÑ¡Ïî
int test_overflow()
{
	char text[256] = "";
	int i = 0;
	strncpy(text, text, 0);
	text[i-1] = 0;
	return i;
}

void main()
{
	CCrashHandler ch;
	ch.SetProcessExceptionHandlers();
	ch.SetThreadExceptionHandlers();

	printf("Choose an exception type:\n");
	printf("0 - SEH exception\n");
	printf("1 - terminate\n");
	printf("2 - unexpected\n");
	printf("3 - pure virtual method call\n");
	printf("4 - invalid parameter\n");
	printf("5 - new operator fault\n");
	printf("6 - SIGABRT\n");
	printf("7 - SIGFPE\n");
	printf("8 - SIGILL\n");
	printf("9 - SIGINT\n");
	printf("10 - SIGSEGV\n");
	printf("11 - SIGTERM\n");
	printf("12 - RaiseException\n");
	printf("13 - throw C++ typed exception\n");
	printf("14 - _snprintf\n");
	printf("15 - stack backtrace\n");
	printf("16 - stack overflow\n");
	printf("Your choice >  ");

	int ExceptionType = 0;
	scanf_s("%d", &ExceptionType);

	switch (ExceptionType)
	{
	case 0: // SEH
		{
			// Access violation
			int* p = 0;
#pragma warning(disable : 6011)   // warning C6011: Dereferencing NULL pointer 'p'
			*p = 0;
#pragma warning(default : 6011)
		}
		break;
	case 1: // terminate
		{
			// Call terminate
			terminate();
		}
		break;
	case 2: // unexpected
		{
			// Call unexpected
			unexpected();
		}
		break;
	case 3: // pure virtual method call
		{
			// pure virtual method call
			CDerived derived;
		}
		break;
	case 4: // invalid parameter
		{
			char* formatString;
			// Call printf_s with invalid parameters.
			formatString = NULL;
#pragma warning(disable : 6387)   // warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function 'printf'
			printf(formatString);
#pragma warning(default : 6387)
		}
		break;
	case 5: // new operator fault
		{
			// Cause memory allocation error
			RecurseAlloc();
		}
		break;
	case 6: // SIGABRT 
		{
			// Call abort
			abort();
		}
		break;
	case 7: // SIGFPE
		{
			// floating point exception ( /fp:except compiler option)
			sigfpe_test();
		}
		break;
	case 8: // SIGILL 
		{
			raise(SIGILL);
		}
		break;
	case 9: // SIGINT 
		{
			raise(SIGINT);
		}
		break;
	case 10: // SIGSEGV 
		{
			raise(SIGSEGV);
		}
		break;
	case 11: // SIGTERM
		{
			raise(SIGTERM);
		}
		break;
	case 12: // RaiseException 
		{
			// Raise noncontinuable software exception
			RaiseException(123, EXCEPTION_NONCONTINUABLE, 0, NULL);
		}
		break;
	case 13: // throw 
		{
			// Throw typed C++ exception.
			throw 13;
		}
		break;
	case 14:
		{
			crash_printf();
			getchar();
		}
		break;
	case 15:
		{
			test_backtrace();
			getchar();
		}
		break;
	case 16:
		{
			int i = test_overflow();
		}
		break;
	default:
		{
			printf("Unknown exception type specified.");
			_getch();
		}
		break;
	}
}
