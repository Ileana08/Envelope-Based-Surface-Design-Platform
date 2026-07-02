# Envelope Continuity in moving milling tools
This project is an exploration of envelope surfaces, which are of great importance in the CNC industry.

Computer numerically controlled machining has been the leading subtractive manufacturing technique for decades. 
It is used in the automotive, aeronautic, and electronic industries, among many others. Many of these industries require high precision and smoothness in the finishing stage of their products. 
This can be difficult to achieve, especially on free-form objects. 
As such, much research has been done on improving the tool motions and tool geometry required to achieve the desired smoothness and precision of free-form objects. 
Most of this research has come from the computer aided design space, and focused on approximating the tool motions and tool geometry. 

In this study, we follow up on the research by Bassegoda (2024) and Couperus (2025).
Our goal is to determine the feasibility of a novel computer-aided design (CAD) method that integrates the two main steps of the current CAD process: designing an object and then generating a tool path that manufactures it. 

## Application Features
- Conical, drum-shaped, and Cubic Bezier defined tools moving through space
- Connect two envelopes with G0 or G1 continuity
- Connect two envelopes via an intermediary third with G0 continuity, in specific circumstances described in this thesis (To Be Published soon)

## Contributions
As mentioned, this application is an extension on the work of Bassegoda and Couperus. The most recent contributions are those of Dijkman and Untaru. Dijkman implemented 
Bezier defined tool shapes, improved user interface and visuals, and improved application performance from exponential to linear time. Untaru implemented Bezier curves and splines
for defining the tool movement and orientation.

## Installation
This application is made in C++ with the QT framework.
Use the [QT Creator](https://www.qt.io/product/development-tools) app to build and run the program.

## Related Theses

1. [C. Aranda Bassegoda: Design of continuous envelope surfaces with cylindrical tools, University of Groningen, 2024](https://fse.studenttheses.ub.rug.nl/33739/)
2. [T. Couperus: Towards Surface Design by Envelopes, University of Groningen, 2025](https://fse.studenttheses.ub.rug.nl/36627/)
