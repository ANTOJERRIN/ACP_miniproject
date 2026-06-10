# Mini Project Report: Menu-Driven 2D Graphics Editor

## 1. Aim
To design and implement a menu-driven 2D Graphics Editor using the C programming language that allows users to create, view, modify, and delete basic geometric shapes (Lines, Rectangles, Circles, and Triangles) on a character-based canvas.

---

## 2. Problem Statement
In computer graphics, rendering vector shapes on a pixel grid requires rasterization algorithms. This project simulates a 2D graphics screen using a character grid in the console window. The editor must maintain an active database of drawn shapes, allow interactive modifications (insertion, modification, and deletion), and automatically update the rendered output on the canvas, while being resilient to invalid user inputs and out-of-bounds drawing coordinates.

---

## 3. Data Structures Used

### A. 2D Character Canvas
The rendering screen is represented as a static 2D character array of fixed dimensions:
```c
char canvas[HEIGHT][WIDTH];
```
- **WIDTH** is set to `60` columns.
- **HEIGHT** is set to `20` rows.
- Unfilled pixels are initialized with the `EMPTY` character (`_`).
- Pixels belonging to drawn shapes are set to the `DRAW` character (`*`).

### B. Object Structure Array
To facilitate shape management (deletion and modification), shape parameters are stored in an array of structures. This decouples the mathematical description of shapes from their rendered output on the character grid.

- **Polymorphic Shape Unions**: Geometric parameters for each shape type are defined in individual structures and grouped in a `union`:
  ```c
  typedef struct { int x1, y1, x2, y2; } LineData;
  typedef struct { int x, y, width, height; } RectData;
  typedef struct { int cx, cy, radius; } CircleData;
  typedef struct { int x1, y1, x2, y2, x3, y3; } TriangleData;
  ```
- **Unified Graphic Object Structure**: Declared as `struct GraphicObject`, this contains a unique ID, shape type indicator, and the parameter union:
  ```c
  struct GraphicObject {
      int id;
      ShapeType type;
      union {
          LineData line;
          RectData rect;
          CircleData circle;
          TriangleData triangle;
      } data;
  };
  ```
- **Active Database**: An array of structures `struct GraphicObject objects[MAX_OBJECTS]` tracks all active shapes in memory, where `MAX_OBJECTS` is set to `100`.

---

## 4. Functions Used

The project is structured modularly with functions categorized into core rendering, shape drawing, object database operations, and user interfaces:

| Function Category | Function Signature | Description |
| :--- | :--- | :--- |
| **Core Rendering** | `initCanvas(canvas)` | Fills the canvas grid with `EMPTY` background characters (`_`). |
| | `displayCanvas(canvas)` | Prints the canvas with vertical row indices and column rulers to the console. |
| | `setPixel(canvas, x, y)` | Draws a single `*` on the canvas if `(x, y)` lies within boundaries `[0, WIDTH-1]` and `[0, HEIGHT-1]`. |
| **Shape Drawing** | `drawLine(canvas, x1, y1, x2, y2)` | Renders a line between two points using Bresenham's algorithm. |
| | `drawRectangle(canvas, x, y, w, h)` | Renders top, bottom, left, and right borders of a rectangle. |
| | `drawCircle(canvas, cx, cy, r)` | Renders a circle boundary using the Midpoint Circle algorithm. |
| | `drawTriangle(canvas, x1, y1, ...)` | Connects three coordinates using three calls to `drawLine()`. |
| **Object Database**| `addObject(objects, count, newObj)` | Assigns an auto-incremented ID and appends the new shape. |
| | `deleteObject(objects, count, id)` | Removes a shape matching the ID and shifts memory contiguously. |
| | `modifyObject(objects, count, id, obj)` | Replaces shape parameters for the matching ID. |
| | `redrawCanvas(canvas, objects, count)` | Clears the canvas and replays all active shapes from scratch. |
| **User Interface** | `getIntegerInput(prompt, min, max)` | Standard Console: Parses input lines securely with range checking. |
| | `getNcursesInteger(prompt, ...)` | Ncurses Mode: Validates inputs safely in fullscreen window layouts. |

---

## 5. Menu Options

The application presents a user-friendly console menu loop:
1. **Add Object**: Prompts the user for a shape type, collects coordinates, appends the shape to the database, and redraws the canvas.
2. **Delete Object**: Displays the list of active shapes, asks the user for the unique ID to delete, shifts database elements, and clears + redraws the canvas.
3. **Modify Object**: Asks the user for the ID to modify, preserves its shape type, prompts for new coordinates, and redraws the canvas.
4. **Display Canvas**: Prints the 2D grid with horizontal column rulers and vertical row labels for precise spatial visualization.
5. **List Objects**: Prints a clean tabular list of all active shape descriptions and coordinate values currently stored in memory.
6. **Exit**: Gracefully exits the application.

---

## 6. Operation Logic (Add, Delete, Modify)

1. **Addition**: The coordinates are validated to ensure reasonable constraints. When added, the shape is placed at index `objectCount` in the objects array, and `objectCount` is incremented.
2. **Deletion**: When a shape is deleted, the array is scanned for the target ID. Once found, subsequent structures in the array are shifted left by one index to keep the database memory contiguous, and `objectCount` is decremented.
3. **Modification**: The array is scanned for the target ID. Once located, its coordinate fields are overwritten with new inputs while retaining the original ID and shape type.
4. **Re-rendering (The Redraw Pipeline)**: The canvas character array does not store vector data. Therefore, whenever an object is added, deleted, or modified, the canvas is completely wiped using `initCanvas()`. The program loops from `0` to `objectCount - 1` and calls the respective drawing functions to redraw the active shapes. This prevents deleted elements from permanently remaining on the screen.

---

## 7. Algorithms Used

### Bresenham's Line Algorithm
The `drawLine()` function uses Bresenham's algorithm to render lines on a grid using integer-only math (which is highly optimized and avoids floating-point roundoff errors). It calculates a decision parameter to check if the next coordinate should step horizontally, vertically, or diagonally.

### Midpoint Circle Algorithm
The `drawCircle()` function uses the Midpoint Circle algorithm. It computes coordinates for a single octant (starting from $x=0$, $y=radius$) and mirrors those pixel coordinates across all eight symmetric regions of the circle relative to the center $(cx, cy)$.

---

## 8. Sample Output Description
When a user launches the program:
- Choosing **Display Canvas (4)** outputs a blank grid bounded by `+` and `-` borders, showing column ticks `0123...59` on top and row ticks `00` through `19` on the left.
- Choosing **Add Object (1)** -> **Circle (3)** -> Center `(30, 10)`, Radius `6` renders a circular perimeter of `*` characters in the center of the grid.
- Attempting to draw coordinates outside the grid range (e.g. Center `(59, 19)`, Radius `5`) safely draws only the visible parts on the screen, showing clipping behaviour without memory faults.

---

## 9. Conclusion
This project demonstrates modular programming and data structures in C. By decoupling shape vector storage from the character raster grid, it shows how modern computer graphics software handles object pipelines. The implementation of Bresenham's and Midpoint circle algorithms using integer math provides a foundational understanding of rasterization techniques.
