Instructions
------------

To compile the program, run the following in the main project directory (where "Makefile" is):

    make KP-anim-modele

The resulting program can be executed:

    ./KP-anim-modele
    
The program should compile and run correctly both on Linux and Mac OSX.


Usage
------------

The program will display the default mesh and wait for user input. You can interact with the program through keyboard input:

    >   zoom in
    <   zoom out
    1
    2
    …
    8   animate the face to display one of the 8 faces:
        1   neutral
        2   happiness
        3   sadness
        4   surprise
        5   anger
        6   disgust
        7   fear
        8   default mesh (does not correspond to any emotion)
    +   increment the animation duration by 0.25s (default duration: 1.0 s)
    -   decrement the animation duration by 0.25s (default duration: 1.0 s)
    q   use the easeInOutQuad easing function for the animation
    w   use the linear easing function for the animation
    a   use the inverse-distance weighting scheme to position vertices w.r.t key points
    s   use the linear weighting scheme to position vertices w.r.t key points
    
Enjoy!


Contact
------------
Cédric Foucault
cedric.foucault@gmail.com
