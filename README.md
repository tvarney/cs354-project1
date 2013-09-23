cs354-project1
==============

First project for CS354 - Computer Graphics

Requirements
============
  * A relatively modern c++ compiler. (g++ 4.6.3 was used)
  * Flex
  * Bison

Features
========
  * Wavefront object and material loader.
  * Extra display modes respect DS_WIRE (VRML and calculated cone).
  * Loading, compiling and linking of fragment and vertex shaders.
  * Texture loading from PNG files
  * Model and Shader paths can be specified on the command line.

Shader Details
==============
The project code attempts to bind the following uniforms:
  "Ka", vec3
  "Kd", vec3
  "Ks", vec3
  "Tr", float
  "Ns", float
  "Light.Position", vec4
  "Light.La", vec3
  "Light.Ld", vec3
  "Light.Ls", vec3
The default shader ignores "Tr", but it is available for use.
