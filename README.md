# Elite Dangerous Engineer Path Finder

This app finds the shortest path through all relevant engineers when upgrading a list of modules. I wrote this because with each new ship loadout, I found was bouncing back and forth across the bubble upgrading each module. I figured there probably exists a shortest path that would take me through all of them that would save me some time.

## Download

Check the latest release or [**download**](https://github.com/int-Frank/EDEPathFinder/releases/download/Release_1.2.0/EDEngineerPaths_1.2.0.zip) it here.

## Use
 
 2. Choose the modules you want to upgrade. For each module:
 a. select a grade. If you only want the experimental, set the grade to **1**.
 b. set a priority. Modules with a higher priority will be upgraded first. Try not to overuse though. This was really only designed to pick out one or two critical components to upgrade first, like the FSD and Power Plant. For the underlying routine to work it needs to be able to move things around.
 3. Select all the engineers you have unlocked.
 4. Select the system you want to start in. You can add additional systems by editing **EDEPaths.json**.
 5. Run!
 
## Example

![](images/demo1.png)

## Background

This is a classic [travelling salesman problem](https://en.wikipedia.org/wiki/Travelling_salesman_problem). I have used a dynamic programming approach, which should give the exact solution. However, the time it takes to find a solution grows exponentially with the number of systems. This should not be noticeable with regular use, but for example if you select every module and every engineer in the bubble, it will take a few minutes to find a solution.

Straight line distance between engineer systems is used. Building a path of every system you need to jump to in between is a far more complicated problem; not really neccessary; clunky; and beyond the scope of what this app sets out to do.

For the case where there are multiple choices of engineer when upgrading a particular module (Felicity Farseer and Elvira Martuuk for the FSD for example), both paths are constructed and the shorter of the two is chosen.

## Feedback

Feel free to submit any feedback! If you want to submit an issue, use one of the following tags:

 - **Bug** - crashes, anything that seems broken, output that seems incorrect or does not make sense.
   - Example: Selecting bubble engineers still takes me to Colonia.
 - **Enhancement** - features and ideas you would like to see.
   - Example: I want the ability to add my own start and end system.
   
## Credits

[ImGui](https://github.com/ocornut/imgui/): A great immediate GUI library.

[Premake](https://premake.github.io/): For simplifying project file creation

[Glad](https://glad.dav1d.de/): OpenGL loader

[SDL2](https://www.libsdl.org/): Development library for graphics and peripherals

[Armin Zare Zadeh](https://github.com/akzare) for his [c++ solution](https://github.com/akzare/Algorithms/blob/master/src/main/cpp/algorithms/graphtheory/TspDynamicProgrammingIterative.h) to the TSP problem
