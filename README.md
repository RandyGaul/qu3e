qu3e v1.01
==========

qu3e is a compact, light-weight and fast 3D physics engine in C++. It is has been specifically created to be used in games. It is portable with no external dependencies other than various standard c header files (such as **cassert** and **cmath**). qu3e is designed to have an extremely simple interface for creating and manipulating rigid bodies.

qu3e is of particular interest to those in need of a fast and simple 3D physics engine, without spending too much time learning about how the whole engine works. In order to keep things very simple and friendly for new users, only box collision is supported. No other shapes are supported (capsules and spheres may be added in the future if requested).

Since qu3e is written in C++ is intended for users familiar with C++. The inner-code of qu3e has quite a few comments and is a great place for users to learn the workings of a 3D physics engine.

qu3e stands for "cube", since the 3 looks slightly like the letter *b* and boxes (or cubes!) are the primary type of collision object.

Screenshots
-----------

![screenshot 1](/screenshots/screen0.PNG?raw=true)
![screenshot 2](/screenshots/screen1.PNG?raw=true)
![screenshot 2](/screenshots/anim0.gif?raw=true)
![screenshot 3](/screenshots/rays.gif?raw=true)

Feature List
------------

Since the primary goal of qu3e is simplicity of use the feature list is inentionally kept to a minimum. If a more full-featured open source physics engine is required I recommend the Bullet physics library:
* Extremely simple and friendly to use API
* 3D Oriented Bounding Box (OBB) collision detection and resolution
* Discrete collision detection
* 3D Raycasting into the world (see RayPush.h in the demo for example usage)
* Ability to query the world with AABBs and points
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
* Modifiable q3Alloc and q3Free functions for custom memory allocation
* Internal heaps and dynamic arrays for memory management, uses q3Alloc/q3Free
* Scene dump -- Can output a log file of C++ code to re-create a physics scene

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

Reporting Bugs
--------------
<b>I've found a bug. How should I report it, or can I fix it myself?</b>

If you feel up for fixing bugs please do add a pull request and I'll do my best to look over and merge the request. Otherwise the github "issues" facility is great for reporting and discussing bugs.

<b>Use the q3Scene::Dump( FILE* ) feature, if possible.</b>

qu3e has a cool feature that Box2D employs: the scene can dump C++ code into a text file. This code can be directly copy/pasted into one of my own demo files. If anyone has a specific bug and is able to dump the scene just before the bug occurs, a scene file can be shared -- this allows a very easy way for myself (or others) to pinpoint and remove bugs. The scene dumping tool is also just a good way to share scene initialization in general. Note: Since the dump outputs C++ code the dump itself is not a form of proper serialization as a recompile would be necessary to run the dump's contents.


FAQ
---

<b>Can you add in this feature?</b>

One of the main goals is to keep the feature list low in order to control code bloat, complexity, and the API itself. If a more full-featured open source physics library is desired please see the Bullet physics engine. That said I'm always open to new ideas. Please use the github "issues" facility to suggest or discuss improvements -- I'll happily respond as time permits.

<b>What are the pros and cons of using your library over something like Bullet, Havok or ODE?</b>

As far as I'm aware there aren't *any* simple and fast open source modern 3D physics engines. Hopefully qu3e can fill in this type of role with pros such as: small library, easily ported to other languages, free, consistent implementation style, minimal to no dependencies. The only cons of qu3e over other engines would be small feature list, but whether this is a significant con depends on the needs of the user.

<b>Why don't you use this (insert C++ feature/library/tool) within your code?</b>

Making use of various C++ features or external C++ libraries adds to the dependencies of the whole project. The idea is that if simple C-like code is pervasive then porting to other languages becomes simple. Just because a feature exists does not warrant its use.

<b>What collision shapes are supported?</b>

Currently just boxes (width, height, depth). Spheres and capsules may be added in the future depending on if users request them. Currently any number of boxes can be used to construct an aggregate rigid body -- this assuages most collision desires that many users have. Perhaps convex hulls and meshes will be added in the far future.

Future
------

As time goes on the feature list of qu3e will intentionally not grow much larger (at least for the near future). Since the primary goal of the library is to very simple in terms of implementation, and simple to use, bloat should be avoided. However, if anyone wants to contribute to any of these areas I would be more than happy work with contributors to include their contributions to qu3e. I handle contributions on a case-by-case basis, and full credit to the contributor would be granted in the source code comments, and in this readme.

<b>Baumgarte and Post Stabilization (Full Non-linear Gauss Seidel)</b>

Currently qu3e uses the Baumgarte Stabilization method for ensuring that "constraint drift" does not occur. The Baumgarte method is very efficient and simple to implement, thus qu3e uses it. However the Baumgarte method does not look all too pleasant in many cases, and also adds extra energy into the system. This is why qu3e has trouble simulating zero restitution.

A much better stabilization method would be [Post Stabilization](https://www.cs.rutgers.edu/~dpai/papers/ClinePai03.pdf). The idea here is to use the velocity jacobian from the velocity constraint to form an iterative position level solver. A good discussion of various stabilization techniques (including the preferred method talked about in this paragraph) can be found in Box2D's b2Island.cpp file in the comments near the top. To implement proper Post Stabilization would require a fair amount of refactoring of the collision detection in qu3e. Right now qu3e constructs collision manifolds in world space. If these were changed to store information relative to the shapes associated with a manifold, then it would be much easier to implement post stabilization. Some other changes would be necessary to ensure many cache misses are avoided.

I would like to upgrade the stabilization method in qu3e at some point in the future, but currently don't know when the time will come since it would require a good amount of time.

<b>Manifold Reduction</b>

A collision manifold consists of contact points. Only 4 well chosen contact points are necessary (for discrete collision detection) to create a stable manifold. Many manifolds between two shapes can have much more than four contact points. Currently, if two boxes rest upon one another a total of 8 contact points may be created.

Reducing a manifold down to a constant maximum of 4 contact points will increase stability in the iterative contact solver. This reduction will also lower the memory cost of keeping manifolds in memory, thus increasing cache line efficiency as a side-effect.

Manifold reduction is not currently implemented in qu3e. Here is an algorithm overview, in case someone wants to try to implement it:
* Query 2D contact plane with a constant direction for a support point, add this first point
* Find the point farthest from the first point, this forms a line segment
* Find third point farthest from previous segment to form a triangle
* Find fourth point that maximizes area of the new quad in the 2D contact plane

For continuous collision detection the deepest point in the manifold needs be kept in the manifold. This can complicate the manifold reduction algorithm, and this complication is not discussed here.

<b>Continuous Collision Detection</b>

Continuous collision detection is not currently implemented in qu3e, but it's cool to have and great for general gameplay. Erin Catto has many free resources for implementing his Bilateral Advancement Time of Impact (TOI) solver. Erin's TOI solver can be used to find a TOI between two convex shapes. From here the Post Stabilization method (described above) can be used in conjuction with a computed TOI to prevent all tunnelining between Static/Dynamic and Static/Kinematic body pairs.

Dynamic to Dynamic or Kinematic to Dynamic continuous collision detection would involve running an O( N^2 ) algorithm with an unkown (and potentially huge) factor of N. For this reason these collision pairs are not discussed here.

At some point in the future I would love to use Erin's resources to implement some cool open source tools for continuous collision detection. Perhaps after I graduate from school this might be a good idea.

<b>Multi-Threading Support</b>

Multi-threading is an interesting topic and qu3e was written with threading in mind. A job system, or task system, would be ideal to batch together things like collision detection. Perhaps a threadpool will be added to qu3e by myself or some future contributor. A minor tweak to the q3Body island index would be needed. Some small memory alignment changes would also be needed. q3Stack can be used to allocate memory for jobs, since stack allocation is so fast.

<b>Single Instruction Multiple Data (SIMD) Support</b>

SIMD - I actually don't have experience using SIMD and all math in qu3e is scalar. This shouldn't really be a performance problem for anyone, but obviously it can be improved. However one clever bit of code is within Collide.cpp: the clipping of two 3D polygons is done via single dimensional lerps! This is a nice way of using scalar math to avoid the need for any SIMD in this particular case.

I do imagine that it would be pretty easy to swap in some open source math library for any bottlenecks, or by-hand code particular pieces.

<b>Additional Collision Shapes (spheres, capsules, convex hulls, meshes)</b>

Collision detection takes time to write, and while in school I don't have infinite time to dedicate to writing robust collision detection code. Spheres and capsules will likely be added by myself in the near future. Convex hulls and meshes are features in other physics engines, so it might not make sense for qu3e to compete -- especially since other open-source engines may just absorb code written for qu3e into their own codebases.

<b>Advanced Joints and Springs</b>

Advanced joints (springs, rods, revolute/prismatic joints) are a nice feature of other physics libraries, and qu3e might incorporate some. They aren't on any to-do list and wouldn't be added without user requests. Some joint types wouldn't clutter the library and can be fairly easy to use for those well versed with C++. The big problem with more advanced joints is setting them up. Often an editor or visual tool is the best way to setup joints, though qu3e itself would require raw C++ to be used.

Credits
-------
Developed by [Randy Gaul](http://randygaul.net).

Erin Catto's online resources, such as [Box2D](http://box2d.org) provided a major inspiration to most of the code within qu3e. Special thank you to Dirk Gregorius and Nathan Carlson for the frequent and invaluable help and advice.

License
-------

qu3e is licensed under the [zlib license](http://en.wikipedia.org/wiki/Zlib_License). See LICENSE.txt for more information.

Change Log
----------

* v1.00 : Initial version
* v1.01 :
    - Raycasting
    - AABB query
	- Point query
	- q3Scene::Dump( FILE* ); Dump contents of scene as C++ file
	- Minor internal bug fixes, mostly with transforms for body->local space
	- Updated readme with large discussion of potential future features
