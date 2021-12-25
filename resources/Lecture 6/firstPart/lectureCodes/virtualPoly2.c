// in this code, we create a polymorphic struct that can handle different shapes using its void * member 

#include<stdio.h>

//data structure representing 2D position with x and y coordinates
typedef struct Position{
    int x;
    int y;
} Position;
 
//data structure representing a 2D circle with radius, position and area as properties
typedef struct Circle{
    int radius;
    Position center;
    float area;
} Circle;
 
//data structure representing a 2D square with legth, center and area as properties
typedef struct Square{
    int length;
    Position center;
    float area;
} Square;

// 'Object_shape' is an enum that can represent shape (either circle as Object_shape_circle or square as Object_shape_square)
typedef enum Object_shape {
    Object_shape_circle,
    Object_shape_square
} Object_shape;
 
// 'Object' is a polymorphic struct that has two properties, 'shape' of type enum Object_shape, 'Object_handler' that is a void pointer 
// to achieve polymorphic behaviour from this struct we can cast the 'Object_handler' to square or circle, based on the shape that the property 'shape' holds
typedef struct Object
{
    Object_shape shape;
    void * Object_handler;
  
} Object;

//function that takes in pointer to a circle and calculate its area
void areaCircle(Circle * ptr)
{
ptr->area = 3.14* ptr->radius * ptr->radius;
}

//function that takes in pointer to a square and calculate its area
void areaSquare(Square * ptr)
{
ptr->area = ptr->length * ptr->length;
}

void handleShape (Object myObj)
{
    switch (myObj.shape)
    {   // if the shape is of myObj is Object_shape_circle, cast its handler to Circle * and assign to ptr
        // then pass ptr to calculate area of the circle
        case Object_shape_circle:
            {Circle * ptr = (Circle *) myObj.Object_handler;
            areaCircle(ptr);
            printf("It is a circle with area %f\n", ptr->area);
            break;
            }
        // if the shape is of myObj is Object_shape_square, cast its handler to Square * and assign to ptr
        // then pass ptr to calculate area of the square
        case Object_shape_square:
            {Square* ptr = (Square*) myObj.Object_handler;
            areaSquare(ptr);
            printf("It is a square with area %f\n", ptr->area);
            break;
            }

    } 
}

int main()
{   // create a position p
    Position p={1,2};
    // create a variable of type 'Circle' with the follwing radius and position values
    Circle c;
    c.radius= 5;
    c.center = p;

    // create a variable 'myObj' of type 'Object', which is a polymorphic struct 
    Object myObj1;
    
    //assign circle to be the shape of the struct 'myObj'
    myObj1.shape=Object_shape_circle;

    // assign the circle c's pointer to myObj's Object_handler
    myObj1.Object_handler = &c; 

    handleShape(myObj1);

    // create a variable of type 'Square' with the follwing radius and position values
    Square s;
    s.length= 5;
    s.center = p;

    // create a variable 'myObj' of type 'Object', which is a polymorphic struct 
    Object myObj2;
    
    //assign square to be the shape of the struct 'myObj'
    myObj2.shape=Object_shape_square;

    // assign the square s's pointer to myObj's Object_handler
    myObj2.Object_handler = &s; 
    
    handleShape(myObj2);
 
  getchar();
  return 0;   
}
