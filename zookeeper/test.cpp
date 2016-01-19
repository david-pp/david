
#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <cstdarg>

typedef std::map<std::string, int> Context;

struct CallBack
{
	CallBack() {}
	CallBack(const Context& c) : context(c) {}

	virtual void call(int err, int value) = 0;

	Context context;
};

namespace async {

	void get(int key, CallBack* cb, const Context& context)
	{
		cb->context = context;
	}

	template <typename CB>
	void get2(int key, const Context& context)
	{
		get(key, new CB());
	}

	void serials(CallBack** cbs, CallBack* cb)
	{

	}

	void serials(std::vector<CallBack*> cbs, CallBack* cb, const Context& context)
	{

	}
}

#define DO(CB) \
struct CB : CallBack { \
	void call(int err, int value) \


#define WHEN_GET(CB, KEY, CONTEXT) \
}; \
async::get(KEY, new CB, CONTEXT);

#define WHEN_SERIALS(CB, CBS, CONTEXT) \
}; \
async::serials(CBArray CBS, new CB, CONTEXT);


std::vector<CallBack*> CBArray(CallBack* cb1, ...)  
{
	std::vector<CallBack*> cbs;
	cbs.push_back(cb1);

	CallBack* cblast = cb1;

	va_list ap;
	va_start(ap, cb1);

	while (cblast != NULL)
	{
		cblast = va_arg(ap, CallBack*);
		cbs.push_back(cblast);
	}

	va_end(ap);

	return cbs;
}   

int main()
{
	Context c;
	c["key1"] = 2;

	// C++ struct
	struct CallBack1 : CallBack {
		void call(int err, int value) {
			struct CallBack2 : CallBack {
				void call(int err, int value) {
				}
			};

			async::get(2, new CallBack2, context);
		}
	};

	async::get(1, new CallBack1, c);

	// C++ 宏
	DO(GetA)
	{
		context["A"] = 222;

		DO(GetB)
		{

		} WHEN_GET(GetB, 3, context);

		//..
	} WHEN_GET(GetA, 2, c);


	// serials struct
	struct CB3 : CallBack {
		void call(int err, int value) {}
	};
	struct CB4 : CallBack {
		void call(int err, int value) {}
	};


	CallBack* cbs[] = { new CB3, new CB4, new CB3, NULL};

	struct CB5 : CallBack {
		void call(int err, int value) {}
	};
	async::serials(CBArray(new CB3, new CB4, new CB3, NULL), new CB5, c);

	// serials macro
	DO(EveryIsDone)
	{
	} WHEN_SERIALS(EveryIsDone, (new CB3, new CB4, new CB4, NULL), c);

	return 0;
}
