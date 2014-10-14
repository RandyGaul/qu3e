qu3e
====

qu3e is a compact, light-weight and fast 3D physics engine in C++. It is has been specifically created to be used in games. It is portable with no external dependencies other than various standard c header files (such as **cassert** and **cmath**). qu3e is designed to have an extremely simple interface for creating and manipulating rigid bodies.

qu3e is of particular interest to those in need of a fast and simple 3D physics engine, without spending too much time learning about how the whole engine works. In order to keep things very simple and friendly for new users, only box collision is supported. No other shapes are supported (capsules and spheres may be added in the future if requested).

Since qu3e is written in C++ is intended for users familiar with C++. The inner-code of qu3e has quite a few comments and is a great place for users to learn the workings of a 3D physics engine.

qu3e stands for "cube", since the 3 looks slightly like the letter *b* and boxes (or cubes!) are the primary type of collision object.

Screenshots
-----------

![screenshot 1](/screenshots/screen0.PNG?raw=true)
![screenshot 2](/screenshots/screen1.PNG?raw=true)
![screenshot 2](/screenshots/anim0.gif?raw=true)

Feature List
------------

Since the primary goal of qu3e is simplicity of use the feature list is inentionally kept to a minimum. If a more full-featured open source physics engine is required I recommend the Bullet physics library:
* Extremely simple and friendly to use API
* 3D Oriented Bounding Box (OBB) collision detection and resolution
* Discrete collision detection
* 3D Raycasting into the world *
* Ability to query the world with AABBs and points *
* Callbacks for collision events
* Sensors (collision volumes)
* Ability to create an aggregate rigid body composed of any number of boxes
* Box stacking
* Islanding and sleeping for CPU optimization
* Renderer agnostic debug drawing interface
* Dynamic AABB tree broad phase
* Highly accurate collision manifold generation via the Separating Axis Theorem
* Collision layers
* Axis of rotation locking (x, y or z axes)
* *These are not quite done yet

Using qu3e
----------

Using qu3e is about creating rigid bodies and attaching boxes to them. When a rigid body is initially created it exists without any collision information. Any number of shapes can be attached to a rigid body. All shapes are defined relative to their owning rigid body, or in other words: all shapes are defined in model space, where model space is the reference frame of the parent rigid body.

The only header that should need to be included is **q3.h**. First create a physics scene. Usually only one physics scene needs to ever be created. The user must specify a fixed timestep upon scene creation:

    #include "q3.h"
	
    q3Scene scene( 1.0 / 60.0 );

A rigid body is created from a physics scene, and rigid bodies (or shapes) cannot be shared between different scenes. Rigid bodies are created by first creating a **q3BodyDef** object. Def objects can be created right on the stack:

    q3BodyDef bodyDef;
	q3Body* body = scene.CreateBody( bodyDef );

The **q3BodyDef** can be passed to a scene in return for a new rigid body. The body definition lets the user specify many settings. See q3Body.h for details. Once a rigid body is constructed any number of boxes can be added to it by providing the body with a **q3BoxDef**:

    q3BoxDef boxDef; // See q3Box.h for settings details
	q3Transform localSpace; // Contains position and orientation, see q3Transform.h for details
	q3Identity( localSpace ); // Specify the origin, and identity orientation
	
	// Create a box at the origin with width, height, depth = (1.0, 1.0, 1.0)
	// and add it to a rigid body. The transform is defined relative to the owning body
	boxDef.Set( localSpace, q3Vec3( 1.0, 1.0, 1.0 ) );
	body->AddBox( boxDef );
	
To simulate the scene simply call **scene.Step( )**. This will simulate the world forward in time by the timestep specified at the scene's construction (usually 1/60 or 1/30).

Future
------

As time goes on the feature list of qu3e will intentionally not grow much larger. Since the primary goal of the library is to very simple in terms of implementation, and simple to use, bloat should be avoided.

Multi-threading is an interesting topic and qu3e was written with threading in mind. A job system, or task system, would be ideal to batch together things like collision detection. Perhaps a threadpool will be added to qu3e by myself or some future contributor.

SIMD - I actually don't have experience using SIMD and all math in qu3e is scalar. This shouldn't really be a performance problem for anyone, but obviously it can be improved. However one clever bit of code is within Collide.cpp: the clipping of two 3D polygons is done via single dimensional lerps! This is a nice way of using scalar math to avoid the need for any SIMD in this particular case.

Advanced joints (springs, rods, revolute/prismatic joints) are a nice feature of other physics libraries, and qu3e might incorporate some. They aren't on any to-do list and wouldn't be added without user requests. Some joint types wouldn't clutter the library and can be fairly easy to use for those well versed with C++. The big problem with more advanced joints is setting them up. Often an editor or visual tool is the best way to setup joints, though qu3e itself would require raw C++ to be used.

Credits
-------
Developed by [Randy Gaul](http://randygaul.net).

Erin Catto's online resources, such as [Box2D](http://box2d.org) provided a major inspiration to most of the code within qu3e. Special thank you to Dirk Gregorius and Nathan Carlson for the frequent and invaluable help and advice.

License
-------

qu3e is licensed under the [zlib license](http://en.wikipedia.org/wiki/Zlib_License). See LICENSE.txt for more information.
