#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>

// NOTE TO PEOPLE WHO ARE JUST IMPORTING THIS FILE:
// Just ignore all of the macros. Everything in this
// file is relevant to you, and is exposed to C.

#ifndef Q3_C_API_NOT_HEADER
#define EXPOSE0(CLASS, RETURN_TYPE, FUNC_NAME)\
    RETURN_TYPE CLASS ## FUNC_NAME(CLASS *);
#define EXPOSE1(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE)\
    RETURN_TYPE CLASS ## FUNC_NAME(CLASS *, ARG1_TYPE);
#define EXPOSE2(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE)\
    RETURN_TYPE CLASS ## FUNC_NAME(CLASS *, ARG1_TYPE, ARG2_TYPE);
#define EXPOSE3(CLASS, RETURN_TYPE, FUNC_NAME, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE)\
    RETURN_TYPE CLASS ## FUNC_NAME(CLASS *, ARG1_TYPE, ARG2_TYPE, ARG3_TYPE);

#define PXPOSE1 EXPOSE1
#define PXPOSE2 EXPOSE2
#define PXPOSE3 EXPOSE3

// bootstrapped types

typedef void q3Body;
typedef void q3Scene;
typedef void q3BoxDef;
typedef void q3Quaternion;
typedef void q3ContactListener;
typedef void q3Render;

// types

typedef float r32;
typedef double r64;
typedef float f32;
typedef double f64;
typedef signed char	i8;
typedef signed short i16;
typedef signed int i32;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

// q3Math

typedef struct q3Vec3
{
	r32 x;
	r32 y;
	r32 z;
} q3Vec3;

typedef struct q3Mat3
{
	q3Vec3 ex;
	q3Vec3 ey;
	q3Vec3 ez;
} q3Mat3;

typedef struct q3Transform
{
	q3Vec3 position;
	q3Mat3 rotation;
} q3Transform;

// common

typedef struct q3AABB
{
	q3Vec3 min;
	q3Vec3 max;
} q3AABB;

typedef struct q3HalfSpace
{
	q3Vec3 normal;
	r32 distance;
} q3HalfSpace;

typedef struct q3RaycastData
{
	q3Vec3 start;	// Beginning point of the ray
	q3Vec3 dir;		// Direction of the ray (normalized)
	r32 t;			// Time specifying ray endpoint

	r32 toi;		// Solved time of impact
	q3Vec3 normal;	// Surface normal at impact
} q3RaycastData;

// q3Body

enum q3BodyType
{
	eStaticBody,
	eDynamicBody,
	eKinematicBody
};

typedef struct q3BodyDef
{
	q3Vec3 axis;			// Initial world transformation.
	r32 angle;				// Initial world transformation. Radians.
	q3Vec3 position;		// Initial world transformation.
	q3Vec3 linearVelocity;	// Initial linear velocity in world space.
	q3Vec3 angularVelocity;	// Initial angular velocity in world space.
	r32 gravityScale;		// Convenient scale values for gravity x, y and z directions.
	i32 layers;				// Bitmask of collision layers. Bodies matching at least one layer can collide.
	void* userData;			// Use to store application specific data.

	r32 linearDamping;
	r32 angularDamping;

	// Static, dynamic or kinematic. Dynamic bodies with zero mass are defaulted
	// to a mass of 1. Static bodies never move or integrate, and are very CPU
	// efficient. Static bodies have infinite mass. Kinematic bodies have
	// infinite mass, but *do* integrate and move around. Kinematic bodies do not
	// resolve any collisions.
	enum q3BodyType bodyType;

	bool allowSleep;	// Sleeping lets a body assume a non-moving state. Greatly reduces CPU usage.
	bool awake;			// Initial sleep state. True means awake.
	bool active;		// A body can start out inactive and just sits in memory.
	bool lockAxisX;		// Locked rotation on the x axis.
	bool lockAxisY;		// Locked rotation on the y axis.
	bool lockAxisZ;		// Locked rotation on the z axis.
} q3BodyDef;

// q3Box

typedef struct q3MassData
{
	q3Mat3 inertia;
	q3Vec3 center;
	r32 mass;
} q3MassData;

typedef struct q3Box
{
	q3Transform local;
	q3Vec3 e;

	struct q3Box* next;
	q3Body* body;
	r32 friction;
	r32 restitution;
	r32 density;
	i32 broadPhaseIndex;
	void* userData;
	bool sensor;
} q3Box;
#endif

// *******  q3Scene  *******

q3Scene * q3SceneCreate(r32 dt);

// NOTICE: These are all equivalent to function declarations.
// I'm sorry that these are macros, but these are easier to
// maintain. I wrote a few examples of what is generated
// from these macros in the comment above them:

// void q3SceneStep();
EXPOSE0(q3Scene,    void,       Step);
// q3Body * q3SceneCreateBody(q3BodyDef *arg1);
PXPOSE1(q3Scene,    q3Body *,   CreateBody,             q3BodyDef *);
// void q3SceneRemoveBody(q3Body *arg1);
EXPOSE1(q3Scene,    void,       RemoveBody,             q3Body *);
// etc.
EXPOSE0(q3Scene,    void,       RemoveAllBodies);
EXPOSE1(q3Scene,    void,       SetAllowSleep,          bool);
EXPOSE1(q3Scene,    void,       SetIterations,          i32);
EXPOSE1(q3Scene,    void,       SetEnableFriction,      bool);
EXPOSE0(q3Scene,    q3Vec3,      GetGravity);
PXPOSE1(q3Scene,    void,       SetGravity,             q3Vec3 *);
EXPOSE0(q3Scene,    void,       Shutdown);
EXPOSE1(q3Scene,    void,       SetContactListener,     q3ContactListener *);
//EXPOSE2(q3Scene,    void,       QueryAABB,              q3QueryCallback *,      q3AABB *); // write this manually
//EXPOSE2(q3Scene,    void,       QueryPoint,             q3QueryCallback *,      q3Vec3 *); // write this manually
//EXPOSE2(q3Scene,    void,       RayCast,                q3QueryCallback *,      q3RaycastData *); // write this manually
EXPOSE1(q3Scene,    void,       Dump,                   FILE *);

// *******  q3Body  *******
PXPOSE1(q3Body,     const q3Box*,   AddBox,                         q3BoxDef *);
EXPOSE1(q3Body,     void,           RemoveBox,                      q3Box *);
EXPOSE0(q3Body,     void,           RemoveAllBoxes);
PXPOSE1(q3Body,     void,           ApplyLinearForce,               q3Vec3 *);
PXPOSE2(q3Body,     void,           ApplyForceAtWorldPoint,         q3Vec3 *,               q3Vec3 *);
PXPOSE1(q3Body,     void,           ApplyLinearImpulse,             q3Vec3 *);
PXPOSE2(q3Body,     void,           ApplyLinearImpulseAtWorldPoint, q3Vec3 *,               q3Vec3 *);
PXPOSE1(q3Body,     void,           ApplyTorque,                    q3Vec3 *);
EXPOSE0(q3Body,     void,           SetToAwake);
EXPOSE0(q3Body,     void,           SetToSleep);
EXPOSE0(q3Body,     bool,           IsAwake);
EXPOSE0(q3Body,     r32,            GetGravityScale);
EXPOSE1(q3Body,     void,           SetGravityScale,                r32);
PXPOSE1(q3Body,     q3Vec3,         GetLocalPoint,                  q3Vec3 *);
PXPOSE1(q3Body,     q3Vec3,         GetLocalVector,                 q3Vec3 *);
PXPOSE1(q3Body,     q3Vec3,         GetWorldPoint,                  q3Vec3 *);
PXPOSE1(q3Body,     q3Vec3,         GetWorldVector,                 q3Vec3 *);
EXPOSE0(q3Body,     q3Vec3,         GetLinearVelocity);
PXPOSE1(q3Body,     q3Vec3,         GetVelocityAtWorldPoint,  q3Vec3 *);
PXPOSE1(q3Body,     void,           SetLinearVelocity,              q3Vec3 *);
EXPOSE0(q3Body,     q3Vec3,         GetAngularVelocity);
PXPOSE1(q3Body,     void,           SetAngularVelocity,             q3Vec3 *);
EXPOSE1(q3Body,     bool,           CanCollide,                     q3Body *);
EXPOSE0(q3Body,     q3Transform,    GetTransform);
EXPOSE0(q3Body,     i32,            GetFlags);
EXPOSE1(q3Body,     void,           SetLayers,                      i32);
EXPOSE0(q3Body,     i32,            GetLayers);
EXPOSE0(q3Body,     q3Quaternion,   GetQuaternion);
EXPOSE0(q3Body,     void *,         GetUserData);

EXPOSE1(q3Body,     void,           SetLinearDamping,               r32);
EXPOSE1(q3Body,     r32,            GetLinearDamping,               r32);
EXPOSE1(q3Body,     void,           SetAngularDamping,              r32);
EXPOSE1(q3Body,     r32,            GetAngularDamping,              r32);

//EXPOSE3(q3Body,     void,           SetTransform,                   q3Vec3 *,               q3Vec3 *,       r32);
EXPOSE1(q3Body,     void,           Render,                         q3Render *);
EXPOSE2(q3Body,     void,           Dump,                           FILE *,                 i32);

EXPOSE0(q3Body,     r32,            GetMass);
EXPOSE0(q3Body,     r32,            GetInvMass);

// *******  q3BoxDef  *******

q3BoxDef * q3BoxDefCreate();

PXPOSE2(q3BoxDef,   void,   Set,            q3Transform *,  q3Vec3 *);
EXPOSE1(q3BoxDef,   void,   SetFriction,    r32);
EXPOSE1(q3BoxDef,   void,   SetRestitution,  r32);
EXPOSE1(q3BoxDef,   void,   SetDensity,     r32);
EXPOSE1(q3BoxDef,   void,   SetSensor,      bool);

// *******  q3Box  *******

EXPOSE1(q3Box,  void,   SetUserdata,    void *);
EXPOSE0(q3Box,  void *, GetUserdata);
// EXPOSE1(q3Box,  void,   SetSensor,      bool); // this actually isn't implemented in C++

PXPOSE2(q3Box,  bool,   TestPoint,      q3Transform *,  q3Vec3 *);
// EXPOSE2(q3Box,  bool,   Raycast,        q3Transform *,  q3RaycastData *); // write this manually
// EXPOSE2(q3Box,  void,   ComputeAABB,    q3Transform *,  q3AABB *); // write this manually
EXPOSE1(q3Box,  void,   ComputeMass,    q3MassData *);
// EXPOSE3(q3Box,  void,   Render,         q3Transform *,  bool,   q3Render *); // write this manually

#ifdef __cplusplus
} // extern "C"
#endif
