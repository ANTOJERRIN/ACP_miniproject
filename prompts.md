# Project Development Prompts

This document compiles the set of prompts used to guide the implementation of the **Menu Driven 2D Graphics Editor** project.

---

### Prompt 1
I am making my ACP mini project in C. The project is a menu-driven 2D graphics editor.
My teacher asked us to use a 2D character array as the canvas. The canvas should be filled with '_' first, and objects should be drawn using '*'.
I want to keep the project simple and in only one file named graphics_editor.c.
Please help me write the code in C for:
- line
- rectangle
- circle
- triangle
The program should have a menu where I can:
1. Add object
2. Delete object
3. Modify object
4. Display canvas
5. List objects
6. Exit
I want to store objects using structures because if I delete or modify something, the canvas should be redrawn from the remaining active objects.
Please keep the code beginner-friendly because I need to understand and explain it during review.

---

### Prompt 2
I already saw a basic version from my classmate, but I want to make my own improved version.
The first version directly erased shapes from the canvas, but I think that can cause problems if objects overlap. So I want to use an array of structures to store all objects, and whenever something is deleted or modified, I want to clear the canvas and redraw only active objects.
Please build this in C using one file only: graphics_editor.c.
Use:
#define WIDTH 60
#define HEIGHT 20
#define MAX_OBJECTS 100
#define EMPTY '_'
#define DRAW '*'
Please include functions like:
initCanvas
displayCanvas
setPixel
drawLine
drawRectangle
drawCircle
drawTriangle
redrawCanvas
addObject
deleteObject
modifyObject
listObjects
Also add comments in the code so I can understand what each part does.

---

### Prompt 3
I am testing my graphics editor and I got confused with coordinates.
Please make the coordinate input clear for a 60 x 20 canvas.
For visible drawing:
- x should be from 0 to 59
- y should be from 0 to 19
For rectangle, I want the input to be:
- top-left x
- top-left y
- width
- height
Please add validation so the rectangle does not go outside the canvas. If it goes outside, show a simple error message and ask again or reject the input.
Keep the code simple and suitable for a C mini project.
I tested rectangle with x=5, y=3, width=20, height=8.
It worked.
Then I tested x=50, width=20 and noticed it goes outside, so I added validation.

---

### Prompt 4
I compiled my graphics_editor.c file and I want to check whether the logic is correct.
Please review and fix the code without changing the basic project idea.
Main requirements:
- Canvas must be a 2D char array.
- Empty area should be '_'.
- Drawn part should be '*'.
- Add, delete, and modify should work properly.
- Objects should be stored in a struct array.
- After delete or modify, the full canvas should be redrawn from active objects.
- The program should not crash on wrong input.
- Keep it as one C file only.
Please explain the important fixes shortly after the code.

---

### Prompt 5
My project requirement says ncurses can be used optionally for menus and display.
First keep the normal stdio version working. Then show me how I can add a simple ncurses version if needed.
I do not want the project to become too advanced because I need to explain it myself. So please keep the ncurses part optional and simple.
Mention the compile command also:
gcc graphics_editor.c -o graphics_editor -lncurses

---

### Prompt 6
I need to write a short explanation for my ACP mini project.
Project title: Menu Driven 2D Graphics Editor in C
Please write it in simple student language, not too professional.
Include:
- Aim
- Problem statement
- Data structure used
- Why I used a 2D character array
- Why I used structures for objects
- Functions used
- How add, delete, and modify work
- Algorithms used for line and circle
- Sample input/output explanation
- Conclusion
Keep it understandable so I can explain it in viva. This should be done in readme.md file
