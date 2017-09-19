# Predator-Prey
C (with OpenGL) implementation of the Predator and Prey cellular automata
## RULES
##### PREDATOR AND PREY CELLULAR AUTOMATON
    The world is grid of cells, with 3 possibilities: Predator(Red), Prey(Green), or Empty(Black).
    Both predator and prey have a set health, that changes over time.
    The simulation works in steps, with the following rules:
        -For prey:
            -Tries to move in a random direction.
            -Health increases.
            -When health reaches a threshold:
                -They will reproduce, creating a new "Prey"
                -Their health resets to 1
        -For predator:
            -Tries to move in a random direction.
            -Health decreases.
            -When health reaches 0, they die and turn into "Nothing".
            -If any adjacent square is a prey:
                -They will eat it, turning it into a "predator" (reproducing)
                -Their health will increase by the amount of health the eaten prey had
	
	
  NOTE: Rules Copied from <a href="https://github.com/Hopson97/PredatorAndPrey/blob/master/main.cpp">Hopson97's implementation</a>.
  
  I'm pretty sure my implementation is actually slightly different.
  
## PARAMETERS
### Simulation
*width*, *height* - The dimensions of the grid in the simulation

*frameRate* - Time between steps

*health_threshold* - Determines how long it takes prey to reproduce and how long it takes predators to die

### Aesthetics
*pointSize* - The size of the points rendered to the screen.

*bleedRate* - Determines how long it takes for colors to fade on a recently dead cell. A higher bleed rate will cause color to fade faster.
