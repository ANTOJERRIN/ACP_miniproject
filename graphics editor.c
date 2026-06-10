/**
 * ============================================================================
 * Project: 2D console-based Graphics Editor
 * Course: College Programming Mini-Project
 * Language: C (Standard C99/C11 compatible)
 * Description: A menu-driven application to draw and manage 2D geometric shapes
 *              (Lines, Rectangles, Circles, Triangles) on a character grid
 *              using Bresenham's line and Midpoint circle drawing algorithms.
 *              
 * Optional Ncurses Mode:
 *   If compiled with -DUSE_NCURSES -lncurses, the program switches to an
 *   interactive, clean terminal screen interface using the ncurses library.
 *   Otherwise, it compiles into a robust standard I/O (stdio) console menu.
 *
 * Compilation Commands:
 *   Standard Stdio mode:
 *     gcc "graphics editor.c" -o graphics_editor
 *
 *   Optional Ncurses mode (requires ncurses installed on your system):
 *     gcc -DUSE_NCURSES "graphics editor.c" -o graphics_editor -lncurses
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_NCURSES
#include <ncurses.h>
#endif

// Global constants defining the drawing environment
#define WIDTH 60
#define HEIGHT 20
#define MAX_OBJECTS 100
#define EMPTY '_'
#define DRAW '*'

// Supported shapes
typedef enum {
    LINE = 1,
    RECTANGLE = 2,
    CIRCLE = 3,
    TRIANGLE = 4
} ShapeType;

// Coordinate data for Line
typedef struct {
    int x1, y1;
    int x2, y2;
} LineData;

// Coordinate and size data for Rectangle
typedef struct {
    int x, y;
    int width, height;
} RectData;

// Coordinate and size data for Circle
typedef struct {
    int cx, cy;
    int radius;
} CircleData;

// Coordinate data for Triangle
typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriangleData;

// Structure for storing graphical object details
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

// Global auto-increment counter for Graphic IDs
static int nextObjectId = 1;

// --- Independent Core Drawing Functions ---
void initCanvas(char canvas[HEIGHT][WIDTH]);
void setPixel(char canvas[HEIGHT][WIDTH], int x, int y);
void drawLine(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2);
void drawRectangle(char canvas[HEIGHT][WIDTH], int x, int y, int width, int height);
void drawCircle(char canvas[HEIGHT][WIDTH], int cx, int cy, int radius);
void drawTriangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3);
void redrawCanvas(char canvas[HEIGHT][WIDTH], struct GraphicObject objects[], int objectCount);
int addObject(struct GraphicObject objects[], int *objectCount, struct GraphicObject newObj);
int deleteObject(struct GraphicObject objects[], int *objectCount, int id);
int modifyObject(struct GraphicObject objects[], int objectCount, int id, struct GraphicObject modifiedObj);

// --- Standard I/O (Stdio) UI Mode Declarations ---
void runStdioMode(void);
void displayCanvas(char canvas[HEIGHT][WIDTH]);
void listObjects(struct GraphicObject objects[], int objectCount);
int getIntegerInput(const char *prompt, int minVal, int maxVal);

// --- Optional Ncurses UI Mode Declarations ---
#ifdef USE_NCURSES
void runNcursesMode(void);
void displayCanvasNcurses(char canvas[HEIGHT][WIDTH], int startY, int startX);
void listObjectsNcurses(struct GraphicObject objects[], int objectCount, int startY, int startX);
int getNcursesInteger(const char *prompt, int minVal, int maxVal, int promptY, int promptX);
#endif

// --- Main Entrance Point ---
int main(void) {
#ifdef USE_NCURSES
    runNcursesMode();
#else
    runStdioMode();
#endif
    return 0;
}

/**
 * Fills the canvas grid with the designated EMPTY background character ('_').
 */
void initCanvas(char canvas[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            canvas[i][j] = EMPTY;
        }
    }
}

/**
 * Sets a character on the canvas at (x, y) to DRAW ('*'),
 * strictly validating boundary constraints to protect memory safety.
 */
void setPixel(char canvas[HEIGHT][WIDTH], int x, int y) {
    if (x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT) {
        canvas[y][x] = DRAW;
    }
}

/**
 * Bresenham's Line Drawing Algorithm.
 * Approximates a straight line on a grid with integer arithmetic only.
 */
void drawLine(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        setPixel(canvas, x1, y1);
        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

/**
 * Outlines a rectangle by drawing top, bottom, left, and right borders.
 */
void drawRectangle(char canvas[HEIGHT][WIDTH], int x, int y, int width, int height) {
    if (width <= 0 || height <= 0) {
        return;
    }
    // Top border
    drawLine(canvas, x, y, x + width - 1, y);
    // Bottom border
    drawLine(canvas, x, y + height - 1, x + width - 1, y + height - 1);
    // Left border
    drawLine(canvas, x, y, x, y + height - 1);
    // Right border
    drawLine(canvas, x + width - 1, y, x + width - 1, y + height - 1);
}

/**
 * Midpoint Circle Drawing Algorithm.
 * Implements 8-way symmetric plotting to form a full circle grid.
 */
void drawCircle(char canvas[HEIGHT][WIDTH], int cx, int cy, int radius) {
    if (radius < 0) {
        return;
    }
    if (radius == 0) {
        setPixel(canvas, cx, cy);
        return;
    }

    int x = 0;
    int y = radius;
    int p = 1 - radius;

    // Symmetric plot points
    setPixel(canvas, cx + x, cy + y);
    setPixel(canvas, cx - x, cy + y);
    setPixel(canvas, cx + x, cy - y);
    setPixel(canvas, cx - x, cy - y);
    setPixel(canvas, cx + y, cy + x);
    setPixel(canvas, cx - y, cy + x);
    setPixel(canvas, cx + y, cy - x);
    setPixel(canvas, cx - y, cy - x);

    while (x < y) {
        x++;
        if (p < 0) {
            p += 2 * x + 1;
        } else {
            y--;
            p += 2 * (x - y) + 1;
        }
        setPixel(canvas, cx + x, cy + y);
        setPixel(canvas, cx - x, cy + y);
        setPixel(canvas, cx + x, cy - y);
        setPixel(canvas, cx - x, cy - y);
        setPixel(canvas, cx + y, cy + x);
        setPixel(canvas, cx - y, cy + x);
        setPixel(canvas, cx + y, cy - x);
        setPixel(canvas, cx - y, cy - x);
    }
}

/**
 * Connects 3 points with 3 straight lines to draw a triangle.
 */
void drawTriangle(char canvas[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, int x3, int y3) {
    drawLine(canvas, x1, y1, x2, y2);
    drawLine(canvas, x2, y2, x3, y3);
    drawLine(canvas, x3, y3, x1, y1);
}

/**
 * Re-draws the canvas from scratch. Invoked automatically on active objects list modifications.
 */
void redrawCanvas(char canvas[HEIGHT][WIDTH], struct GraphicObject objects[], int objectCount) {
    initCanvas(canvas);
    for (int i = 0; i < objectCount; i++) {
        switch (objects[i].type) {
            case LINE:
                drawLine(canvas, objects[i].data.line.x1, objects[i].data.line.y1,
                         objects[i].data.line.x2, objects[i].data.line.y2);
                break;
            case RECTANGLE:
                drawRectangle(canvas, objects[i].data.rect.x, objects[i].data.rect.y,
                              objects[i].data.rect.width, objects[i].data.rect.height);
                break;
            case CIRCLE:
                drawCircle(canvas, objects[i].data.circle.cx, objects[i].data.circle.cy,
                           objects[i].data.circle.radius);
                break;
            case TRIANGLE:
                drawTriangle(canvas, objects[i].data.triangle.x1, objects[i].data.triangle.y1,
                             objects[i].data.triangle.x2, objects[i].data.triangle.y2,
                             objects[i].data.triangle.x3, objects[i].data.triangle.y3);
                break;
        }
    }
}

/**
 * Appends a new graphic object to the active shapes database.
 */
int addObject(struct GraphicObject objects[], int *objectCount, struct GraphicObject newObj) {
    if (*objectCount >= MAX_OBJECTS) {
        return 0;
    }
    newObj.id = nextObjectId++;
    objects[*objectCount] = newObj;
    (*objectCount)++;
    return 1;
}

/**
 * Deletes a graphic object using its unique ID, shifting other active elements.
 */
int deleteObject(struct GraphicObject objects[], int *objectCount, int id) {
    int foundIndex = -1;
    for (int i = 0; i < *objectCount; i++) {
        if (objects[i].id == id) {
            foundIndex = i;
            break;
        }
    }
    if (foundIndex == -1) {
        return 0;
    }
    // Shift subsequent objects to keep memory contiguous
    for (int i = foundIndex; i < *objectCount - 1; i++) {
        objects[i] = objects[i + 1];
    }
    (*objectCount)--;
    return 1;
}

/**
 * Modifies an existing graphic object structure parameters matching a specific ID.
 */
int modifyObject(struct GraphicObject objects[], int objectCount, int id, struct GraphicObject modifiedObj) {
    for (int i = 0; i < objectCount; i++) {
        if (objects[i].id == id) {
            modifiedObj.id = id;
            objects[i] = modifiedObj;
            return 1;
        }
    }
    return 0;
}


/* ============================================================================
 *                    STANDARD STDOUT (STDIO) UI MODE
 * ============================================================================
 */

void runStdioMode(void) {
    char canvas[HEIGHT][WIDTH];
    struct GraphicObject objects[MAX_OBJECTS];
    int objectCount = 0;

    initCanvas(canvas);

    printf("====================================================\n");
    printf("        CONSOLE-BASED 2D GRAPHICS EDITOR            \n");
    printf("====================================================\n");

    while (1) {
        printf("\n=== Main Menu ===\n");
        printf("1. Add Object\n");
        printf("2. Delete Object\n");
        printf("3. Modify Object\n");
        printf("4. Display Canvas (with Grid Labels)\n");
        printf("5. List Objects\n");
        printf("6. Exit\n");
        printf("=================\n");
        
        int choice = getIntegerInput("Enter choice (1-6): ", 1, 6);

        if (choice == 6) {
            printf("\nExiting application. Thank you!\n");
            break;
        }

        switch (choice) {
            case 1: { // Add Object
                if (objectCount >= MAX_OBJECTS) {
                    printf("\nError: Object storage limit reached (%d max).\n", MAX_OBJECTS);
                    break;
                }
                printf("\n--- Select Object Type to Add ---\n");
                printf("1. Line\n");
                printf("2. Rectangle\n");
                printf("3. Circle\n");
                printf("4. Triangle\n");
                printf("5. Cancel and return\n");
                
                int typeChoice = getIntegerInput("Enter choice (1-5): ", 1, 5);
                if (typeChoice == 5) {
                    printf("Shape creation cancelled.\n");
                    break;
                }

                struct GraphicObject newObj;
                newObj.type = (ShapeType)typeChoice;

                // Coordinates can be entered in range [-100, 100] to show boundary checking
                if (typeChoice == 1) {
                    printf("\n--- Draw a Line ---\n");
                    newObj.data.line.x1 = getIntegerInput("Enter Start X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.line.y1 = getIntegerInput("Enter Start Y coordinate (-100 to 100): ", -100, 100);
                    newObj.data.line.x2 = getIntegerInput("Enter End X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.line.y2 = getIntegerInput("Enter End Y coordinate (-100 to 100): ", -100, 100);
                } else if (typeChoice == 2) {
                    printf("\n--- Draw a Rectangle ---\n");
                    newObj.data.rect.x = getIntegerInput("Enter Top-Left X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.rect.y = getIntegerInput("Enter Top-Left Y coordinate (-100 to 100): ", -100, 100);
                    newObj.data.rect.width = getIntegerInput("Enter Width (1 to 100): ", 1, 100);
                    newObj.data.rect.height = getIntegerInput("Enter Height (1 to 100): ", 1, 100);
                } else if (typeChoice == 3) {
                    printf("\n--- Draw a Circle ---\n");
                    newObj.data.circle.cx = getIntegerInput("Enter Center X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.circle.cy = getIntegerInput("Enter Center Y coordinate (-100 to 100): ", -100, 100);
                    newObj.data.circle.radius = getIntegerInput("Enter Radius (0 to 100): ", 0, 100);
                } else if (typeChoice == 4) {
                    printf("\n--- Draw a Triangle ---\n");
                    newObj.data.triangle.x1 = getIntegerInput("Enter Vertex 1 X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.triangle.y1 = getIntegerInput("Enter Vertex 1 Y coordinate (-100 to 100): ", -100, 100);
                    newObj.data.triangle.x2 = getIntegerInput("Enter Vertex 2 X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.triangle.y2 = getIntegerInput("Enter Vertex 2 Y coordinate (-100 to 100): ", -100, 100);
                    newObj.data.triangle.x3 = getIntegerInput("Enter Vertex 3 X coordinate (-100 to 100): ", -100, 100);
                    newObj.data.triangle.y3 = getIntegerInput("Enter Vertex 3 Y coordinate (-100 to 100): ", -100, 100);
                }

                if (addObject(objects, &objectCount, newObj)) {
                    printf("\nSuccess: Shape successfully added with ID: %d\n", objects[objectCount - 1].id);
                    redrawCanvas(canvas, objects, objectCount);
                } else {
                    printf("\nError: Failed to store shape.\n");
                }
                break;
            }
            case 2: { // Delete Object
                if (objectCount == 0) {
                    printf("\nNo objects available to delete.\n");
                    break;
                }
                listObjects(objects, objectCount);
                int idToDelete = getIntegerInput("Enter the ID of the object to delete: ", 1, nextObjectId);
                if (deleteObject(objects, &objectCount, idToDelete)) {
                    printf("\nSuccess: Object ID %d deleted.\n", idToDelete);
                    redrawCanvas(canvas, objects, objectCount);
                } else {
                    printf("\nError: Object ID %d not found.\n", idToDelete);
                }
                break;
            }
            case 3: { // Modify Object
                if (objectCount == 0) {
                    printf("\nNo objects available to modify.\n");
                    break;
                }
                listObjects(objects, objectCount);
                int idToModify = getIntegerInput("Enter the ID of the object to modify: ", 1, nextObjectId);
                
                int foundIndex = -1;
                for (int i = 0; i < objectCount; i++) {
                    if (objects[i].id == idToModify) {
                        foundIndex = i;
                        break;
                    }
                }
                if (foundIndex == -1) {
                    printf("\nError: Object ID %d not found.\n", idToModify);
                    break;
                }

                struct GraphicObject modifiedObj;
                modifiedObj.type = objects[foundIndex].type;

                printf("\n--- Modifying Object ID %d (%s) ---\n", idToModify, 
                       (modifiedObj.type == LINE) ? "Line" :
                       (modifiedObj.type == RECTANGLE) ? "Rectangle" :
                       (modifiedObj.type == CIRCLE) ? "Circle" : "Triangle");

                if (modifiedObj.type == LINE) {
                    modifiedObj.data.line.x1 = getIntegerInput("Enter New Start X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.line.y1 = getIntegerInput("Enter New Start Y coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.line.x2 = getIntegerInput("Enter New End X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.line.y2 = getIntegerInput("Enter New End Y coordinate (-100 to 100): ", -100, 100);
                } else if (modifiedObj.type == RECTANGLE) {
                    modifiedObj.data.rect.x = getIntegerInput("Enter New Top-Left X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.rect.y = getIntegerInput("Enter New Top-Left Y coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.rect.width = getIntegerInput("Enter New Width (1 to 100): ", 1, 100);
                    modifiedObj.data.rect.height = getIntegerInput("Enter New Height (1 to 100): ", 1, 100);
                } else if (modifiedObj.type == CIRCLE) {
                    modifiedObj.data.circle.cx = getIntegerInput("Enter New Center X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.circle.cy = getIntegerInput("Enter New Center Y coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.circle.radius = getIntegerInput("Enter New Radius (0 to 100): ", 0, 100);
                } else if (modifiedObj.type == TRIANGLE) {
                    modifiedObj.data.triangle.x1 = getIntegerInput("Enter New Vertex 1 X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.triangle.y1 = getIntegerInput("Enter New Vertex 1 Y coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.triangle.x2 = getIntegerInput("Enter New Vertex 2 X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.triangle.y2 = getIntegerInput("Enter New Vertex 2 Y coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.triangle.x3 = getIntegerInput("Enter New Vertex 3 X coordinate (-100 to 100): ", -100, 100);
                    modifiedObj.data.triangle.y3 = getIntegerInput("Enter New Vertex 3 Y coordinate (-100 to 100): ", -100, 100);
                }

                if (modifyObject(objects, objectCount, idToModify, modifiedObj)) {
                    printf("\nSuccess: Object ID %d modified.\n", idToModify);
                    redrawCanvas(canvas, objects, objectCount);
                } else {
                    printf("\nError: Failed to modify object.\n");
                }
                break;
            }
            case 4: { // Display Canvas
                displayCanvas(canvas);
                break;
            }
            case 5: { // List Objects
                listObjects(objects, objectCount);
                break;
            }
        }
    }
}

/**
 * Outputs the canvas to standard output, adorned with clear horizontal
 * column rulers and vertical row labels for precise user mapping.
 */
void displayCanvas(char canvas[HEIGHT][WIDTH]) {
    printf("\n");

    // Print column labels (Tens row)
    printf("     ");
    for (int j = 0; j < WIDTH; j++) {
        if (j % 10 == 0) {
            printf("%d", j / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print column labels (Units row)
    printf("     ");
    for (int j = 0; j < WIDTH; j++) {
        printf("%d", j % 10);
    }
    printf("\n");

    // Top horizontal border
    printf("   +");
    for (int j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");

    // Print rows, prefixed with 2-digit zero-padded row indices
    for (int i = 0; i < HEIGHT; i++) {
        printf("%02d |", i);
        for (int j = 0; j < WIDTH; j++) {
            printf("%c", canvas[i][j]);
        }
        printf("|\n");
    }

    // Bottom horizontal border
    printf("   +");
    for (int j = 0; j < WIDTH; j++) {
        printf("-");
    }
    printf("+\n");
}

/**
 * Outputs a formatted text summary of all active elements in memory.
 */
void listObjects(struct GraphicObject objects[], int objectCount) {
    if (objectCount == 0) {
        printf("\nNo objects created yet.\n");
        return;
    }
    printf("\n============================================================\n");
    printf("                  LIST OF ACTIVE OBJECTS                    \n");
    printf("============================================================\n");
    printf("%-4s | %-10s | %-40s\n", "ID", "Type", "Details/Coordinates");
    printf("------------------------------------------------------------\n");
    for (int i = 0; i < objectCount; i++) {
        switch (objects[i].type) {
            case LINE:
                printf("%-4d | %-10s | Start: (%d, %d), End: (%d, %d)\n",
                       objects[i].id, "Line",
                       objects[i].data.line.x1, objects[i].data.line.y1,
                       objects[i].data.line.x2, objects[i].data.line.y2);
                break;
            case RECTANGLE:
                printf("%-4d | %-10s | Top-Left: (%d, %d), Width: %d, Height: %d\n",
                       objects[i].id, "Rectangle",
                       objects[i].data.rect.x, objects[i].data.rect.y,
                       objects[i].data.rect.width, objects[i].data.rect.height);
                break;
            case CIRCLE:
                printf("%-4d | %-10s | Center: (%d, %d), Radius: %d\n",
                       objects[i].id, "Circle",
                       objects[i].data.circle.cx, objects[i].data.circle.cy,
                       objects[i].data.circle.radius);
                break;
            case TRIANGLE:
                printf("%-4d | %-10s | V1: (%d, %d), V2: (%d, %d), V3: (%d, %d)\n",
                       objects[i].id, "Triangle",
                       objects[i].data.triangle.x1, objects[i].data.triangle.y1,
                       objects[i].data.triangle.x2, objects[i].data.triangle.y2,
                       objects[i].data.triangle.x3, objects[i].data.triangle.y3);
                break;
        }
    }
    printf("============================================================\n");
}

/**
 * Securely reads and validates integer input from standard input buffer.
 * Keeps standard error messages clean and prevents hanging buffers.
 */
int getIntegerInput(const char *prompt, int minVal, int maxVal) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Trim leading space and newline characters
            buffer[strcspn(buffer, "\r\n")] = '\0';
            
            if (strlen(buffer) == 0) {
                printf("Error: Input cannot be empty. Please enter a value.\n");
                continue;
            }

            char *endptr;
            val = (int)strtol(buffer, &endptr, 10);
            
            // Check for fully consumed digit strings
            if (endptr != buffer && *endptr == '\0') {
                if (val >= minVal && val <= maxVal) {
                    return val;
                } else {
                    printf("Error: Value out of range. Must be between %d and %d.\n", minVal, maxVal);
                }
            } else {
                printf("Error: Invalid numeric input. Please enter a valid integer.\n");
            }
        } else {
            printf("Error: Failed to read terminal input.\n");
            exit(EXIT_FAILURE);
        }
    }
}


/* ============================================================================
 *                    OPTIONAL TERMINAL SCREEN (NCURSES) UI MODE
 * ============================================================================
 */

#ifdef USE_NCURSES
void runNcursesMode(void) {
    char canvas[HEIGHT][WIDTH];
    struct GraphicObject objects[MAX_OBJECTS];
    int objectCount = 0;

    initCanvas(canvas);
    
    // Initialize ncurses screen
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    int canvasY = 1;
    int canvasX = 2;
    int menuY = 3 + HEIGHT + 3; // Position menu below the canvas area
    int menuX = 2;

    while (1) {
        clear();
        mvprintw(0, 2, "=== NCURSES 2D GRAPHICS EDITOR ===");
        displayCanvasNcurses(canvas, canvasY, canvasX);

        mvprintw(menuY, menuX, "=== Main Menu ===");
        mvprintw(menuY + 1, menuX, "1. Add Object");
        mvprintw(menuY + 2, menuX, "2. Delete Object");
        mvprintw(menuY + 3, menuX, "3. Modify Object");
        mvprintw(menuY + 4, menuX, "4. List Objects");
        mvprintw(menuY + 5, menuX, "5. Exit");
        mvprintw(menuY + 6, menuX, "=================");
        refresh();

        int choice = getNcursesInteger("Enter choice (1-5): ", 1, 5, menuY + 8, menuX);
        if (choice == 5) {
            break;
        }

        switch (choice) {
            case 1: { // Add Object
                if (objectCount >= MAX_OBJECTS) {
                    move(menuY + 8, menuX);
                    clrtoeol();
                    printw("Error: Max object limit reached. Press any key...");
                    refresh();
                    getch();
                    break;
                }
                clear();
                mvprintw(0, 2, "=== Select Object Type ===");
                mvprintw(2, 2, "1. Line");
                mvprintw(3, 2, "2. Rectangle");
                mvprintw(4, 2, "3. Circle");
                mvprintw(5, 2, "4. Triangle");
                mvprintw(6, 2, "5. Cancel");
                refresh();

                int typeChoice = getNcursesInteger("Enter choice (1-5): ", 1, 5, 8, 2);
                if (typeChoice == 5) {
                    break;
                }

                struct GraphicObject newObj;
                newObj.type = (ShapeType)typeChoice;

                clear();
                mvprintw(0, 2, "=== Enter Shape Details ===");
                refresh();
                if (typeChoice == 1) { // Line
                    newObj.data.line.x1 = getNcursesInteger("Start X (-100 to 100): ", -100, 100, 2, 2);
                    newObj.data.line.y1 = getNcursesInteger("Start Y (-100 to 100): ", -100, 100, 4, 2);
                    newObj.data.line.x2 = getNcursesInteger("End X (-100 to 100): ", -100, 100, 6, 2);
                    newObj.data.line.y2 = getNcursesInteger("End Y (-100 to 100): ", -100, 100, 8, 2);
                } else if (typeChoice == 2) { // Rectangle
                    newObj.data.rect.x = getNcursesInteger("Top-Left X (-100 to 100): ", -100, 100, 2, 2);
                    newObj.data.rect.y = getNcursesInteger("Top-Left Y (-100 to 100): ", -100, 100, 4, 2);
                    newObj.data.rect.width = getNcursesInteger("Width (1 to 100): ", 1, 100, 6, 2);
                    newObj.data.rect.height = getNcursesInteger("Height (1 to 100): ", 1, 100, 8, 2);
                } else if (typeChoice == 3) { // Circle
                    newObj.data.circle.cx = getNcursesInteger("Center X (-100 to 100): ", -100, 100, 2, 2);
                    newObj.data.circle.cy = getNcursesInteger("Center Y (-100 to 100): ", -100, 100, 4, 2);
                    newObj.data.circle.radius = getNcursesInteger("Radius (0 to 100): ", 0, 100, 6, 2);
                } else if (typeChoice == 4) { // Triangle
                    newObj.data.triangle.x1 = getNcursesInteger("Vertex 1 X (-100 to 100): ", -100, 100, 2, 2);
                    newObj.data.triangle.y1 = getNcursesInteger("Vertex 1 Y (-100 to 100): ", -100, 100, 4, 2);
                    newObj.data.triangle.x2 = getNcursesInteger("Vertex 2 X (-100 to 100): ", -100, 100, 6, 2);
                    newObj.data.triangle.y2 = getNcursesInteger("Vertex 2 Y (-100 to 100): ", -100, 100, 8, 2);
                    newObj.data.triangle.x3 = getNcursesInteger("Vertex 3 X (-100 to 100): ", -100, 100, 10, 2);
                    newObj.data.triangle.y3 = getNcursesInteger("Vertex 3 Y (-100 to 100): ", -100, 100, 12, 2);
                }

                if (addObject(objects, &objectCount, newObj)) {
                    redrawCanvas(canvas, objects, objectCount);
                }
                break;
            }
            case 2: { // Delete Object
                if (objectCount == 0) {
                    move(menuY + 8, menuX);
                    clrtoeol();
                    printw("No objects to delete. Press any key...");
                    refresh();
                    getch();
                    break;
                }
                clear();
                mvprintw(0, 2, "=== Active Objects ===");
                listObjectsNcurses(objects, objectCount, 2, 2);
                refresh();
                int listHeight = objectCount + 4;
                int idToDelete = getNcursesInteger("Enter ID to delete: ", 1, nextObjectId, listHeight + 2, 2);
                if (deleteObject(objects, &objectCount, idToDelete)) {
                    redrawCanvas(canvas, objects, objectCount);
                } else {
                    mvprintw(listHeight + 4, 2, "Error: Object ID not found. Press any key...");
                    refresh();
                    getch();
                }
                break;
            }
            case 3: { // Modify Object
                if (objectCount == 0) {
                    move(menuY + 8, menuX);
                    clrtoeol();
                    printw("No objects to modify. Press any key...");
                    refresh();
                    getch();
                    break;
                }
                clear();
                mvprintw(0, 2, "=== Active Objects ===");
                listObjectsNcurses(objects, objectCount, 2, 2);
                refresh();
                int listHeight = objectCount + 4;
                int idToModify = getNcursesInteger("Enter ID to modify: ", 1, nextObjectId, listHeight + 2, 2);
                
                int foundIndex = -1;
                for (int i = 0; i < objectCount; i++) {
                    if (objects[i].id == idToModify) {
                        foundIndex = i;
                        break;
                    }
                }
                if (foundIndex == -1) {
                    mvprintw(listHeight + 4, 2, "Error: Object ID not found. Press any key...");
                    refresh();
                    getch();
                    break;
                }

                struct GraphicObject modifiedObj;
                modifiedObj.type = objects[foundIndex].type;

                clear();
                mvprintw(0, 2, "=== Enter New Shape Details ===");
                refresh();
                if (modifiedObj.type == LINE) {
                    modifiedObj.data.line.x1 = getNcursesInteger("New Start X (-100 to 100): ", -100, 100, 2, 2);
                    modifiedObj.data.line.y1 = getNcursesInteger("New Start Y (-100 to 100): ", -100, 100, 4, 2);
                    modifiedObj.data.line.x2 = getNcursesInteger("New End X (-100 to 100): ", -100, 100, 6, 2);
                    modifiedObj.data.line.y2 = getNcursesInteger("New End Y (-100 to 100): ", -100, 100, 8, 2);
                } else if (modifiedObj.type == RECTANGLE) {
                    modifiedObj.data.rect.x = getNcursesInteger("New Top-Left X (-100 to 100): ", -100, 100, 2, 2);
                    modifiedObj.data.rect.y = getNcursesInteger("New Top-Left Y (-100 to 100): ", -100, 100, 4, 2);
                    modifiedObj.data.rect.width = getNcursesInteger("New Width (1 to 100): ", 1, 100, 6, 2);
                    modifiedObj.data.rect.height = getNcursesInteger("New Height (1 to 100): ", 1, 100, 8, 2);
                } else if (modifiedObj.type == CIRCLE) {
                    modifiedObj.data.circle.cx = getNcursesInteger("New Center X (-100 to 100): ", -100, 100, 2, 2);
                    modifiedObj.data.circle.cy = getNcursesInteger("New Center Y (-100 to 100): ", -100, 100, 4, 2);
                    modifiedObj.data.circle.radius = getNcursesInteger("New Radius (0 to 100): ", 0, 100, 6, 2);
                } else if (modifiedObj.type == TRIANGLE) {
                    modifiedObj.data.triangle.x1 = getNcursesInteger("New Vertex 1 X (-100 to 100): ", -100, 100, 2, 2);
                    modifiedObj.data.triangle.y1 = getNcursesInteger("New Vertex 1 Y (-100 to 100): ", -100, 100, 4, 2);
                    modifiedObj.data.triangle.x2 = getNcursesInteger("New Vertex 2 X (-100 to 100): ", -100, 100, 6, 2);
                    modifiedObj.data.triangle.y2 = getNcursesInteger("New Vertex 2 Y (-100 to 100): ", -100, 100, 8, 2);
                    modifiedObj.data.triangle.x3 = getNcursesInteger("New Vertex 3 X (-100 to 100): ", -100, 100, 10, 2);
                    modifiedObj.data.triangle.y3 = getNcursesInteger("New Vertex 3 Y (-100 to 100): ", -100, 100, 12, 2);
                }

                if (modifyObject(objects, objectCount, idToModify, modifiedObj)) {
                    redrawCanvas(canvas, objects, objectCount);
                }
                break;
            }
            case 4: { // List Objects
                clear();
                mvprintw(0, 2, "=== Active Objects ===");
                listObjectsNcurses(objects, objectCount, 2, 2);
                int listHeight = objectCount + 4;
                mvprintw(listHeight + 2, 2, "Press any key to return to Main Menu...");
                refresh();
                getch();
                break;
            }
        }
    }

    // Clean up ncurses environment
    endwin();
}

/**
 * Render standard canvas format under ncurses.
 */
void displayCanvasNcurses(char canvas[HEIGHT][WIDTH], int startY, int startX) {
    // Column rulers: Tens digit
    mvprintw(startY, startX, "     ");
    for (int j = 0; j < WIDTH; j++) {
        if (j % 10 == 0) {
            printw("%d", j / 10);
        } else {
            printw(" ");
        }
    }
    
    // Column rulers: Units digit
    mvprintw(startY + 1, startX, "     ");
    for (int j = 0; j < WIDTH; j++) {
        printw("%d", j % 10);
    }

    // Upper boundary
    mvprintw(startY + 2, startX, "   +");
    for (int j = 0; j < WIDTH; j++) {
        printw("-");
    }
    printw("+");

    // Print internal canvas rows with row labels
    for (int i = 0; i < HEIGHT; i++) {
        mvprintw(startY + 3 + i, startX, "%02d |", i);
        for (int j = 0; j < WIDTH; j++) {
            printw("%c", canvas[i][j]);
        }
        printw("|");
    }

    // Lower boundary
    mvprintw(startY + 3 + HEIGHT, startX, "   +");
    for (int j = 0; j < WIDTH; j++) {
        printw("-");
    }
    printw("+");
}

/**
 * Render shape list under ncurses.
 */
void listObjectsNcurses(struct GraphicObject objects[], int objectCount, int startY, int startX) {
    if (objectCount == 0) {
        mvprintw(startY, startX, "No objects created yet.");
        return;
    }
    mvprintw(startY, startX, "%-4s | %-10s | %-40s", "ID", "Type", "Details/Coordinates");
    mvprintw(startY + 1, startX, "------------------------------------------------------------");
    for (int i = 0; i < objectCount; i++) {
        int y = startY + 2 + i;
        switch (objects[i].type) {
            case LINE:
                mvprintw(y, startX, "%-4d | %-10s | Start: (%d, %d), End: (%d, %d)",
                       objects[i].id, "Line",
                       objects[i].data.line.x1, objects[i].data.line.y1,
                       objects[i].data.line.x2, objects[i].data.line.y2);
                break;
            case RECTANGLE:
                mvprintw(y, startX, "%-4d | %-10s | Top-Left: (%d, %d), Width: %d, Height: %d",
                       objects[i].id, "Rectangle",
                       objects[i].data.rect.x, objects[i].data.rect.y,
                       objects[i].data.rect.width, objects[i].data.rect.height);
                break;
            case CIRCLE:
                mvprintw(y, startX, "%-4d | %-10s | Center: (%d, %d), Radius: %d",
                       objects[i].id, "Circle",
                       objects[i].data.circle.cx, objects[i].data.circle.cy,
                       objects[i].data.circle.radius);
                break;
            case TRIANGLE:
                mvprintw(y, startX, "%-4d | %-10s | V1: (%d, %d), V2: (%d, %d), V3: (%d, %d)",
                       objects[i].id, "Triangle",
                       objects[i].data.triangle.x1, objects[i].data.triangle.y1,
                       objects[i].data.triangle.x2, objects[i].data.triangle.y2,
                       objects[i].data.triangle.x3, objects[i].data.triangle.y3);
                break;
        }
    }
}

/**
 * Parse validated keyboard inputs inside ncurses screens.
 */
int getNcursesInteger(const char *prompt, int minVal, int maxVal, int promptY, int promptX) {
    char inputBuf[100];
    int val;
    while (1) {
        // Clear prompt area line
        move(promptY, promptX);
        clrtoeol();
        printw("%s", prompt);
        
        // Turn on terminal echo for visual typing
        echo();
        getnstr(inputBuf, sizeof(inputBuf) - 1);
        noecho();

        // Clear carriage return and newline characters
        inputBuf[strcspn(inputBuf, "\r\n")] = '\0';
        
        if (strlen(inputBuf) == 0) {
            move(promptY + 1, promptX);
            clrtoeol();
            printw("Error: Input cannot be empty. Press any key...");
            refresh();
            getch();
            move(promptY + 1, promptX);
            clrtoeol();
            continue;
        }

        char *endptr;
        val = (int)strtol(inputBuf, &endptr, 10);
        if (endptr != inputBuf && *endptr == '\0') {
            if (val >= minVal && val <= maxVal) {
                move(promptY + 1, promptX);
                clrtoeol();
                return val;
            } else {
                move(promptY + 1, promptX);
                clrtoeol();
                printw("Error: Out of range (%d to %d). Press any key...", minVal, maxVal);
                refresh();
                getch();
                move(promptY + 1, promptX);
                clrtoeol();
            }
        } else {
            move(promptY + 1, promptX);
            clrtoeol();
            printw("Error: Invalid numeric input. Press any key...");
            refresh();
            getch();
            move(promptY + 1, promptX);
            clrtoeol();
        }
    }
}
#endif
