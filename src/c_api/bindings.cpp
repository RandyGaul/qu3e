#include "../scene/q3Scene.h"
#include "../dynamics/q3Body.h"
#include "../collision/q3Box.h"

#define Q3_C_API_NOT_HEADER

#define EXPOSE0(CLASS, RETURN_TYPE, FUNC_NAME) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self)\
{\
    return self->FUNC_NAME();\
}

#define EXPOSE1(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1)\
{\
    return self->FUNC_NAME(arg1);\
}

#define EXPOSE2(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1, ARG2_TYPE arg2)\
{\
    return self->FUNC_NAME(arg1, arg2);\
}

#define EXPOSE3(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3)\
{\
    return self->FUNC_NAME(arg1, arg2, arg3);\
}

// C++ reference workaround

#define PXPOSE1(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1)\
{\
    return self->FUNC_NAME(*arg1);\
}

#define PXPOSE2(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1, ARG2_TYPE arg2)\
{\
    return self->FUNC_NAME(*arg1, *arg2);\
}

#define PXPOSE3(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE) \
RETURN_TYPE CLASS ## FUNC_NAME(CLASS *self, ARG1_TYPE arg1, ARG2_TYPE arg2, ARG3_TYPE arg3)\
{\
    return self->FUNC_NAME(*arg1, *arg2, *arg3);\
}

#include "../q3_c.h"

#ifdef __cplusplus
extern "C" {
#endif

q3Scene * q3SceneCreate(r32 dt)
{
	return new q3Scene(dt);
}

q3BoxDef * q3BoxDefCreate()
{
	return new q3BoxDef();
}

#ifdef __cplusplus
} // extern "C"
#endif
