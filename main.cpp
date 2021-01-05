/*
David Hu     Jan.17.2020    Performance Task: Fruit Ninja
This is the performance task program made by David Hu of course ICS4U.
The game is called "Fruit Ninja", where player slices fruits to earn score, and at the same time tries to avoid
hitting bombs (flashing spheres), which deducts 10 points from the total.
The goal of the game is to reach the highest score possible within the time limit.
Good luck, have Fun!!!
*/

/// Libraries
#include <windows.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <string>
#include <cmath>


using namespace std;

/// Variable Declarations
bool mousePress = false;    /// create boolean variable mousePress and set it to false initially
int score = 0;  /// create integer variable score to record player score on each round (set to 0 initially)
int menuNumber = 0; /// create integer variable menuNumber to keep track of menu pages (set to 0 initially)
int Time;   /// create integer variable Time as the count down timer in each game
float fruitAngle = 0;   /// create float variable called fruitAngle to rotate fruits and title fruits
float TitleAngle1 = 0;  /// create float variable called TitleAngle1 as the circular translation speed for the first title fruit
float TitleAngle2 = 0;  /// create float variable called TitleAngle2 as the circular translation speed for the second title fruit
float wordRotateAngle = 0;  /// create float variable called wordRotateAngle to rotate the words around the center so that it matches the circular translation
int timeLimit = 60; /// create integer variable called timeLimit as the count down timer limit


void mouse(int button, int state, int x, int y)     /// function for handling mouse events
{
    if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)    /// if left mouse button is clicked
    {
        mousePress = true;  /// set mousePress as true
    }
    else
    {
        mousePress = false; /// otherwise set mousePress as false
    }
}

struct fruitsCoordinates    /// create structure to store information about fruits
{
    float x;    /// float variable to record x position
    float y;    /// float variable to record y position
    float yVelocity;    /// float variable to record vertical velocity
    float xVelocity;    /// float variable to record horizontal velocity
    float radius;   /// float variable to record fruits' radius
    int r;  /// variables to store information about the color of the fruit
    int g;
    int b;
};

struct bombCoordinates  /// create structure to store information about bombs
{
    float x;    /// float variable to record x position
    float y;    /// float variable to record y position
    float radius;   /// float variable to record fruits' radius
    float yVelocity;  /// float variable to record vertical velocity
    float xVelocity;  /// float variable to record horizontal velocity
};

struct bladeTrail   /// create structure to store information about bladeTrail
{
    float trailX; /// float variable to record x position
    float trailY; /// float variable to record y position
};

vector <fruitsCoordinates> fruitGenerator;  /// create vector for fruits
vector <bladeTrail> trailCoordinates;   /// create vector for blade trails
vector <bombCoordinates> bombGenerator; /// create vector for bombs

float passiveMouseX;    /// float variable to record x-position of the mouse (mouse movement being passive)
float passiveMouseY;    /// float variable to record y-position of the mouse (mouse movement being passive)

void motion(int x, int y)   /// function for storing information about mousePosition (with the purpose of generating bladeTrail)
{
    bladeTrail temp;    /// create temporary variable of type bladeTrail
    temp.trailX = ((float)x-250)/250;  /// convert pixel coordinates to the one set in ortho and pass the value to trailX
    temp.trailY = -1*((float)y-250)/250; /// convert pixel coordinates to the one set in ortho and pass the value to trailX
    trailCoordinates.push_back(temp);   /// pushBack temp to vector trailCoordinates
    passiveMouseX = ((float)x-250)/250;  /// convert pixel coordinates to the one set in ortho and pass the value to passiveMouseX
    passiveMouseY = -1*((float)y-250)/250; /// convert pixel coordinates to the one set in ortho and pass the value to passiveMouseY
}

void drawingTrail() /// function for drawing bladeTrail
{
    glLineWidth(5); /// set the width of the line to be 5
    glColor3f(0.0,0.0,1.0);   /// colour setup
    for(int i = 0; i < trailCoordinates.size(); i++)  /// loop through every element / point in vector trailCoordinates
    {
        if(trailCoordinates.size() > 1 && i > 1)    /// the first point always start at (0,0), so i has to be greater than 1 to properly draw line segments
            /// a line can only be drawn if there are at least two points, so the size of the vector has to be greater than 1
        {
            glBegin(GL_LINE_STRIP); /// begin drawing with primitive GL_LINE_STRIP
            glVertex3f(trailCoordinates[i].trailX, trailCoordinates[i].trailY, -4);     /// draw vertex with ith element's information
            glVertex3f(trailCoordinates[i-1].trailX, trailCoordinates[i-1].trailY, -4); /// draw vertex with (i-1)th element's information
            glEnd();    /// connect the two vertices to form a line segment, thus ending the drawing
        }

        if(trailCoordinates.size() > 10) /// if there are more than 10 elements in the vector
        {
            trailCoordinates.erase(trailCoordinates.begin()+1); /// eliminate the first element
        }
    }

}

void instantiateBomb()  /// function for storing information about the bombs
{
    bombCoordinates temp;   /// creating temporary variable of type bombCoordinates
    temp.yVelocity = (rand()%11 + 65)/1000.0;  /// randomly instantiate vertical velocity between 0.065 and 0.075
    temp.xVelocity = (rand()%31 - 15)/1000.0;  /// randomly instantiate horizontal velocity between -0.015 and 0.015
    temp.radius = 0.1; /// give bombs a radius of 0.1

    int xMaxBound = 8;  /// set the max bound of x
    int xMinBound = -8; /// set the min bound of x
    temp.x = (rand()%(xMaxBound-xMinBound+1) + xMinBound)/10.0; /// randomly instantiate horizontal position between xMaxBound / 10.0 and xMinBound / 10.0 (10.0 not 10!)
    int yMaxBound = -170;   /// set the max bound of y
    int yMinBound = -120;   /// set the min bound of y
    temp.y = -(rand()%(-1*yMaxBound - -1*yMinBound + 1) + yMinBound*-1)/100.0; /// randomly instantiate vertical position between yMaxBound / 100.0 and yMinBound / 100.0 (100.0 not 100!)
    bombGenerator.push_back(temp); /// push back temp to vector bombGenerator
}

void instantiateFruit() /// function for storing information about the fruits
{
    fruitsCoordinates temp; /// creating temporary variable of type fruitsCoordinates
    temp.yVelocity = (rand()%11 + 65)/1000.0; /// randomly instantiate vertical velocity between 0.065 and 0.075
    temp.xVelocity = (rand()%31 - 15)/1000.0; /// randomly instantiate horizontal velocity between -0.015 and 0.015
    temp.radius = (rand()%8 + 5)/100.0; /// randomly instantiate radius for fruits between 0.05 and 0.12
    int xMaxBound = 8;  /// set the max bound of x
    int xMinBound = -8; /// set the min bound of x
    temp.x = (rand()%(xMaxBound-xMinBound+1) + xMinBound)/10.0; /// randomly instantiate horizontal position between xMaxBound / 10.0 and xMinBound / 10.0 (10.0 not 10!)
    int yMaxBound = -170; /// set the max bound of y
    int yMinBound = -120; /// set the min bound of y
    temp.y = -(rand()%(-1*yMaxBound - -1*yMinBound + 1) + yMinBound*-1)/100.0; /// randomly instantiate vertical position between yMaxBound / 100.0 and yMinBound / 100.0 (100.0 not 100!)
    /// randomly generating r,g,b values to randomize fruit colors
    temp.r = rand()%2;
    temp.g = rand()%2;
    temp.b = rand()%2;
    fruitGenerator.push_back(temp); /// push back temp to vector fruitGenerator
}

void fruitTimer(int t)  /// timer for spawning fruits
{
    int randomSpawningTime = rand()%701 + 300;  /// randomly set spawning time between 300ms and 1000ms
    glutTimerFunc(randomSpawningTime, fruitTimer, 0);   /// call time function with rate defined above
    instantiateFruit(); /// call function to gain information about each fruit
    glutPostRedisplay();    /// indicates the function needs to be redisplayed
}

void bombTimer(int t)   /// timer for spawning bombs
{
    int randomSpawningTime = rand()%3001 + 4000; /// randomly set spawning time between 4000ms and 7000ms
    glutTimerFunc(randomSpawningTime, bombTimer, 0); /// call time function with rate defined above
    instantiateBomb(); /// call function to gain information about each bomb
    glutPostRedisplay(); /// indicates the function needs to be redisplayed
}


void printstrokefont(string s)  /// function for printing texts using stroke font
{
    for(int i = 0; i < s.length(); i++) /// loop through every character in the given string
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, s.at(i));    /// call stroke font function to print text
    }
}


void drawStrokeFont()   /// function for storing information on the texts that need to be drawn
{
    glColor3f(0.0,1.0,0.0); /// colour setup
    glLineWidth(1); /// define the width of the line

    /// Time
    glPushMatrix(); /// push current matrix stack
    glTranslatef(0.7,0.8,-4.0); /// set the position of the text
    glScalef(0.0005,0.0005,0.0005); /// scale the text
    string str1 = to_string(timeLimit - Time/1000); /// set text to display time
    printstrokefont(str1); /// pass the string to the printing function
    glPopMatrix();  /// pop current matrix stack

    /// Score
    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.85,0.8,-4.0);  /// set the position of the text
    glScalef(0.0005,0.0005,0.0005);  /// scale the text
    string str3 = to_string(score); /// set text to display score
    printstrokefont(str3);  /// pass the string to the printing function
    glPopMatrix();  /// pop current matrix stack
}

void destructionEffect(float x, float y, float radius)  /// function for creating destruction effect
{
    glColor3f(1.0,0.0,0.0); /// set the colour to red

    glBegin(GL_LINE_STRIP); /// begin drawing with primitive GL_LINE_STRIP
    for(int i = 0; i < 360; i++) /// for loop for drawing a center circle
    {
        float theta = i * 3.142 / 180;
        glVertex3f(x + (radius+0.03)*cos(theta), y + (radius+0.03)*sin(theta),-4);
    }
    glEnd(); /// finish drawing the circle
}


void timerIdle(int t)   /// idle timer
{
    glutTimerFunc(20, timerIdle, 0);   /// call itself every
    if(menuNumber == 2) /// if menuNumber == 2, meaning it is currently the mainGamePlay scene
        Time += 20; /// add 20 to Time

    for(int i = 0; i < fruitGenerator.size(); i++)  /// loop through every element (fruit) in the vector
    {
        fruitGenerator[i].yVelocity -= 0.001; /// decrease y velocity by 0.001
        fruitGenerator[i].x += fruitGenerator[i].xVelocity; /// change x position by adding x velocity
        fruitGenerator[i].y += fruitGenerator[i].yVelocity; /// change y position by adding y velocity
    }

    for(int i = 0; i < bombGenerator.size(); i++) /// loop through every element (bomb) in the vector
    {
        bombGenerator[i].yVelocity -= 0.001;    /// decrease y velocity by 0.001
        bombGenerator[i].x += bombGenerator[i].xVelocity;  /// change x position by adding x velocity
        bombGenerator[i].y += bombGenerator[i].yVelocity;  /// change y position by adding y velocity
    }

    fruitAngle += 1;    /// add fruitAngle by 1
    if(fruitAngle > 360)    /// if fruitAngle is greater than 360
        fruitAngle -= 360;  /// decrease fruitAngle by 360

    TitleAngle1 += 0.05;    /// add TitleAngle1 by 0.05
    if(TitleAngle1 > 360 * 3.1415926 / 180) /// convert angle from degrees to radians
        /// if TitleAngle1 is greater than 360 * 3.1415926 / 180
        TitleAngle1 -= 360 * 3.1415926 / 180;  /// decrease TitleAngle1 by 360 * 3.1415926 / 180

    wordRotateAngle += 2.2;   /// add wordRotateAngle by 2.2

    TitleAngle2 += 0.03;    /// add TitleAngle2 by 0.05
    if(TitleAngle2 > 360 * 3.1415926 / 180)     /// convert angle from degrees to radians
        /// if TitleAngle2 is greater than 360 * 3.1415926 / 180
        TitleAngle2 -= 360 * 3.1415926 / 180;   /// decrease TitleAngle2 by 360 * 3.1415926 / 180

    glutPostRedisplay();    /// indicates the function needs to be redisplayed
}

void background()   /// Function for Drawing Background
{
    /// Vertical Bars
    glColor4f(1.0,0.0,0.0,0.5);    /// set colour
    glLineWidth(4);                /// set the width of the line to be 4
    glBegin(GL_LINE_STRIP);        /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-0.3, 1.0, -5);     /// draw and connect vertices
    glVertex3f(-0.3, -1.0, -5);
    glEnd();                       /// finish drawing the line
    glBegin(GL_LINE_STRIP);        /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-0.6, 1.0, -5);     /// draw and connect vertices
    glVertex3f(-0.6, -1.0, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.0, 1.0, -5);   /// draw and connect vertices
    glVertex3f(0.0, -1.0, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.3, 1.0, -5);   /// draw and connect vertices
    glVertex3f(0.3, -1.0, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.6, 1.0, -5);   /// draw and connect vertices
    glVertex3f(0.6, -1.0, -5);
    glEnd();                    /// finish drawing the line

    /// Horizontal Bars
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-1.0, 0.5, -5);  /// draw and connect vertices
    glVertex3f(-0.6, 0.5, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-0.6, -0.7, -5); /// draw and connect vertices
    glVertex3f(-0.3, -0.7, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.6, 0.2, -5);   /// draw and connect vertices
    glVertex3f(1, 0.2, -5);
    glEnd();                    /// finish drawing the line

    /// Diagonal Lines
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-0.4, 0.1, -5);  /// draw and connect vertices
    glVertex3f(0.1, -0.1, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(-0.5, -0.2, -5); /// draw and connect vertices
    glVertex3f(0.15, 0.2, -5);
    glEnd();                    /// finish drawing the line
    glBegin(GL_LINE_STRIP);     /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.2, -0.55, -5); /// draw and connect vertices
    glVertex3f(0.5, -0.3, -5);
    glEnd();                     /// finish drawing the line
    glBegin(GL_LINE_STRIP);      /// begin drawing with primitive GL_LINE_STRIP
    glVertex3f(0.35, -0.27, -5); /// draw and connect vertices
    glVertex3f(0.4, -0.73, -5);
    glEnd();                     /// finish drawing the line
}

struct buttons  /// create structure for storing information about buttons
{
    float buttonX;  /// create float variable to store the x value of the button
    float buttonY;  /// create float variable to store the y value of the button
    float buttonRadius; /// create float variable to store the y value of the button
    string buttonName;  /// create string variable to store the name of the buttonName
};

vector <buttons> titleFruits;   /// create vector of type buttons

void instantiateTitle() /// function for storing information about buttons
{
    buttons temp1;  /// create temporary variable of type buttons
    temp1.buttonX = 0;  /// set x coordinate of the first title fruit as 0
    temp1.buttonY = 0.1;/// set y coordinate of the first title fruit as 0.1
    temp1.buttonRadius = 0.2;   /// set the radius for the first title fruit as 0.2
    temp1.buttonName = "Play";  /// set the string for the first title fruit as "Play"
    titleFruits.push_back(temp1);   /// push back the first temp
    buttons temp2; /// create temporary variable of type buttons
    temp2.buttonX = 0; /// set x coordinate of the second title fruit as 0
    temp2.buttonY = -0.3;/// set y coordinate of the second title fruit as -0.3
    temp2.buttonRadius = 0.1;/// set the radius for the second title fruit as 0.1
    temp2.buttonName = "Quit"; /// set the string for the second title fruit as "Quit"
    titleFruits.push_back(temp2); /// push back the second temp
    buttons temp3;/// create temporary variable of type buttons
    temp3.buttonX = 0; /// set x coordinate of the third title fruit as 0
    temp3.buttonY = 0.1;/// set y coordinate of the third title fruit as 0.1
    temp3.buttonRadius = 0.2;/// set the radius for the third title fruit as 0.2
    temp3.buttonName = "Replay"; /// set the string for the third title fruit as "Replay"
    titleFruits.push_back(temp3); /// push back the third temp
}


void drawTitleText()    /// Function for drawing title texts
{
    glColor3f(0.0,1.0,0.0); /// set the colour to green
    glLineWidth(1); /// set the width of the line to be 1

    /// Big Title
    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.45,0.65,-4.0); /// set the position of the text
    glScalef(0.0014,0.0012,0.0012);/// scale the text
    string title = ("Fruit Ninja"); /// create string variable and set text as "Fruit Ninja"
    printstrokefont(title); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    /// First Button
    glPushMatrix(); /// push current matrix stack
    glTranslatef(0 + cos(TitleAngle1)*(0.2+0.05),0.1 + sin(TitleAngle1)*(0.2+0.05),-4.0); /// make the text move in circular motion
    glRotatef(wordRotateAngle,0,0,1); /// rotate the text so it matches with the circular motion
    glScalef(0.0005,0.0005,0.0005);/// scale the text
    string playButtonString = ("Play"); /// create string variable and set text as "Play"
    printstrokefont(playButtonString); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(0 + cos(TitleAngle2)*(0.1+0.05),-0.3 + sin(TitleAngle2)*(0.1+0.05),-4.0); /// make the text move in circular motion
    glRotatef(wordRotateAngle,0,0,1); /// rotate the text so it matches with the circular motion
    glScalef(0.0005,0.0005,0.0005);/// scale the text
    string QuitButtonString = ("Quit"); /// create string variable and set text as "Quit"
    printstrokefont(QuitButtonString); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack
}

void mainMenu() /// function for mainMenu
{
    glClearColor(176/255.0,116/255.0,56/255.0,69/255.0); /// Set background as brown
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// clearing buffers
    glLoadIdentity(); /// load identity matrix
    glTranslatef(0.0,0.0,-3.0);  /// default translation so everything can appear on the screen
    drawTitleText();    /// call drawTitleText Function
    drawingTrail();     /// call drawingTrail Function
    instantiateTitle(); /// call instantiateTitle Function

    if(passiveMouseX != 0 && passiveMouseY != 0)    /// by default, mouse position is (0,0), so only if mouse is not at the origin does it execute following commands
    {
        for(int i = 0; i < 2; i++)  /// Menu Swipe Control, loop through every button
        {
            if(passiveMouseX < titleFruits[i].buttonX + titleFruits[i].buttonRadius &&
                    passiveMouseX > titleFruits[i].buttonX - titleFruits[i].buttonRadius &&
                    passiveMouseY < titleFruits[i].buttonY + titleFruits[i].buttonRadius &&
                    passiveMouseY > titleFruits[i].buttonY - titleFruits[i].buttonRadius)   /// if the mouse position is within the button hitbox
            {
                if(titleFruits[i].buttonName == "Play") /// if the "Play" button has been sliced
                {
                    destructionEffect(titleFruits[i].buttonX, titleFruits[i].buttonY, titleFruits[i].buttonRadius);  /// call destuctionEffect Function
                    menuNumber = 1; /// change menuNumber to 1 so the screen is now the instruction page
                }
                else if(titleFruits[i].buttonName == "Quit")    /// if the "Quit" button has been sliced
                {
                    destructionEffect(titleFruits[i].buttonX, titleFruits[i].buttonY, titleFruits[i].buttonRadius); /// call destuctionEffect Function
                    exit(0);    /// Exit the Program
                }
            }
        }

    }

    for(int i = 0; i < titleFruits.size(); i++)  /// Drawing Menu Fruits, for loop goes through every fruit
    {
        glPushMatrix(); /// push current matrix stack
        glTranslatef(titleFruits[i].buttonX,titleFruits[i].buttonY,-4.0); /// set the position for the title fruit
        glRotatef(fruitAngle, 1, 1, 0); /// rotate the title fruits
        glutSolidSphere(titleFruits[i].buttonRadius, 32, 32); /// call default function to draw 3D spheres with information provided
        glPopMatrix(); /// pop current matrix stack
    }

    glutSwapBuffers();  /// swap the old sphere with the new one

}

void endSceneTexts()    /// Function for Drawing end scene texts
{
    glLineWidth(1); /// set the width of the line to be 1

    /// "Great Job!" Text
    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.4,0.65,-4.0); /// set the position for the text
    glScalef(0.0012,0.0009,0.0009); /// scale the text
    string endingTitle = ("Great Job!"); /// create a string variable and pass the text "Great Job!" to it
    printstrokefont(endingTitle);   /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    /// Score Display
    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.25,0.45,-4.0); /// set the position for the text
    glScalef(0.0008,0.0006,0.0006); /// scale the text
    string endingScore = to_string(score); /// create a string variable and pass the value of score to it
    string endingMessage = "Score: " + endingScore; /// create another string variable that combines score with string "Score: "
    printstrokefont(endingMessage);/// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(0 + cos(TitleAngle1)*(0.2+0.05),0.1 + sin(TitleAngle1)*(0.2+0.05),-4.0);   /// make the text move in circular motion
    glRotatef(wordRotateAngle,0,0,1);   /// rotate the text so it matches with the circular motion
    glScalef(0.0005,0.0005,0.0005); /// scale the text
    string playButtonString = ("Replay"); /// create a string variable and pass text "Replay" to it
    printstrokefont(playButtonString);/// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(0 + cos(TitleAngle2)*(0.1+0.05),-0.3 + sin(TitleAngle2)*(0.1+0.05),-4.0); /// make the text move in circular motion
    glRotatef(wordRotateAngle,0,0,1); /// rotate the text so it matches with the circular motion
    glScalef(0.0005,0.0005,0.0005); /// scale the text
    string QuitButtonString = ("Quit"); /// create a string variable and pass text "Quit" to it
    printstrokefont(QuitButtonString);/// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack
}

void endScene()  /// Function for drawing end scene
{
    glClearColor(176/255.0,116/255.0,56/255.0,69/255.0); /// Set brown background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// clearing buffers
    glLoadIdentity(); /// Load identity matrix
    glTranslatef(0.0,0.0,-3.0);  /// default translation so everything can appear on the screen
    endSceneTexts();    /// call endSceneText function to generate texts
    drawingTrail();     /// call drawingTrail function to have blade trail effect

    for(int i = 1; i < 3; i++)  /// loop through every title fruit in end scene
    {
        if(passiveMouseX < titleFruits[i].buttonX + titleFruits[i].buttonRadius &&
                passiveMouseX > titleFruits[i].buttonX - titleFruits[i].buttonRadius &&
                passiveMouseY < titleFruits[i].buttonY + titleFruits[i].buttonRadius &&
                passiveMouseY > titleFruits[i].buttonY - titleFruits[i].buttonRadius)  /// if the mouse is within the hitbox of the title fruit
        {
            if(titleFruits[i].buttonName == "Replay")   /// if the title fruit is "Replay" button
            {
                destructionEffect(titleFruits[i].buttonX, titleFruits[i].buttonY, titleFruits[i].buttonRadius); /// enable destruction effect
                score = 0;  /// reset score as 0
                menuNumber = 1; /// change menuNumber to 1, thus change screen to mainGamePlay scene
            }
            else if(titleFruits[i].buttonName == "Quit")    /// if the title fruit is "Quit" Button
            {
                destructionEffect(titleFruits[i].buttonX, titleFruits[i].buttonY, titleFruits[i].buttonRadius); /// enable destruction effect
                exit(0);    /// exit the program
            }

        }
    }

    for(int i = 1; i < 3; i++)  /// draw the title fruits in end scene
    {
        glPushMatrix(); /// push current matrix stack
        glTranslatef(titleFruits[i].buttonX,titleFruits[i].buttonY,-4.0);   /// set the position of the title fruits
        glRotatef(fruitAngle, 1, 1, 0); /// rotate the title fruits
        glutSolidSphere(titleFruits[i].buttonRadius, 32, 32);   /// call default function to draw 3D spheres with information provided
        glPopMatrix(); /// pop current matrix stack
    }

    glutSwapBuffers();  /// swap the old sphere with the new one
}


void drawInstructionPage()  /// Function for drawing the instruction page
{
    float firstLineY = 0.5; /// create a float variable called firstLineY to record the y position of the first line of text
    glClearColor(176/255.0,116/255.0,56/255.0,69/255.0); /// set brown background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// clearing buffers
    glLoadIdentity();   /// load identity matrix
    glTranslatef(0.0,0.0,-3.0);  /// default translation so everything can appear on the screen

    glLineWidth(1); /// set the width of the line to be 1
    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.75,firstLineY,-4.0); /// set the position of the fisrt line of text
    glScalef(0.0006,0.0006,0.0006); /// scale the words
    string instruction1 = ("Slice The Fruits Before They Disappear"); /// create a string variable and pass the text to it as the first line of instruction
    printstrokefont(instruction1);  /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.65,firstLineY-0.25,-4.0);/// set the position of the second line of text
    glScalef(0.0006,0.0006,0.0006); /// scale the words
    string instruction2 = ("Slicing Bombs Deducts 10 Points"); /// create a string variable and pass the text to it as the second line of instruction
    printstrokefont(instruction2); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.3,firstLineY-0.5,-4.0); /// set the position of the third line of text
    glScalef(0.0006,0.0006,0.0006); /// scale the words
    string instruction3 = ("60 SECONDS"); /// create a string variable and pass the text to it as the third line of instruction
    printstrokefont(instruction3); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    glPushMatrix(); /// push current matrix stack
    glTranslatef(-0.4,firstLineY-0.75,-4.0); /// set the position of the fourth line of text
    glScalef(0.0006,0.0006,0.0006); /// scale the words
    string instruction4 = ("Click To Continue"); /// create a string variable and pass the text to it as the fourth line of instruction
    printstrokefont(instruction4); /// pass the string to the printing function
    glPopMatrix(); /// pop current matrix stack

    if(mousePress == true)  /// if mousePress is true, meaning the left mouse button has been clicked
    {
        menuNumber = 2; /// change menuNumber to be 2, so the screen switches to mainGamePlay scene
        mousePress = false; /// reset mousePress boolean as false
    }

    glutSwapBuffers();  ///swap the old sphere with the new one
}


void mainGamePlay() /// Main game play function
{
    glClearColor(176/255.0,116/255.0,56/255.0,69/255.0); /// set brown background
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /// clearing buffers
    glLoadIdentity(); /// load the identity matrix
    glTranslatef(0.0,0.0,-3.0);  /// default translation so everything can appear on the screen
    background(); /// call the background function to draw the background


    for(int i = 0; i < fruitGenerator.size(); i++)  /// loop through every element / fruit in the vector to generate fruits
    {
        glColor3d(fruitGenerator[i].r,fruitGenerator[i].g,fruitGenerator[i].b); /// set the random colour
        glPushMatrix(); /// push a matrix
        glTranslatef(fruitGenerator[i].x,fruitGenerator[i].y,-4); /// set the position of the fruit, since velocity changes this line moves the fruits
        glRotated(fruitAngle,1,1,0); /// rotate the sphere
        glutSolidSphere(fruitGenerator[i].radius,32,32); /// draw the solid sphere
        glPopMatrix(); /// pop the matrix, thus finishing drawing for this object
    }

    for(int i = 0; i < bombGenerator.size(); i++)   /// loop through every element / bomb in the vector to generate bombs
    {
        glColor3d(rand()%2,rand()%2,rand()%2); /// randomly set a color every frame
        glPushMatrix(); /// push current matrix stack
        glTranslatef(bombGenerator[i].x, bombGenerator[i].y,-4); /// set the position of the bomb, since velocity changes this line moves the bombs
        glRotated(fruitAngle,1,1,0); /// rotate the sphere
        glutSolidSphere(bombGenerator[i].radius,32,32); /// draw the solid sphere
        glPopMatrix(); /// pop the matrix, thus finishing drawing for this object
    }

    /// slicing fruits (swipe control)
    for(int i = 0; i < fruitGenerator.size(); i++) /// loop through every element / fruit in the vector
    {
        if((passiveMouseX < fruitGenerator[i].x + fruitGenerator[i].radius) && (passiveMouseX > fruitGenerator[i].x - fruitGenerator[i].radius)
                && (passiveMouseY < fruitGenerator[i].y + fruitGenerator[i].radius) && (passiveMouseY > fruitGenerator[i].y - fruitGenerator[i].radius))
            /// if the mouse position is within the fruit's hitbox
        {
            destructionEffect(fruitGenerator[i].x, fruitGenerator[i].y, fruitGenerator[i].radius);  /// enable destruction effect
            score++;    /// add one to score
            fruitGenerator.erase(fruitGenerator.begin() + i);   /// erase the fruit that has been sliced from the vector
        }
    }

    /// slicing bombs (swipe control)
    for(int i = 0; i < bombGenerator.size(); i++)  /// loop through every element / bomb in the vector
    {
        if((passiveMouseX < bombGenerator[i].x + bombGenerator[i].radius) && (passiveMouseX > bombGenerator[i].x - bombGenerator[i].radius)
                && (passiveMouseY < bombGenerator[i].y + bombGenerator[i].radius) && (passiveMouseY > bombGenerator[i].y - bombGenerator[i].radius))
            /// if the mouse position is within the fruit's hitbox
        {
            destructionEffect(bombGenerator[i].x, bombGenerator[i].y, bombGenerator[i].radius); /// enable destruction effect
            if(score - 10 < 0)  /// if a bomb is sliced, points get subtracted by 10. If score is less than 0 after the subtraction, set the score to be 0 instead
            {
                score = 0;  /// reset score as 0
            }
            else    /// if the score is still greater than 0 after subtraction
            {
                score -= 10;    /// minus 10 to the score
            }
            bombGenerator.erase(bombGenerator.begin() + i); /// erase the bomb that has been sliced from the vector
        }
    }

    /// autimatically destroy bomb if it is outside the screen
    for(int i = 0; i < bombGenerator.size(); i++)   /// loop through every element / bomb in the vector
    {
        if(bombGenerator[i].y < -2) /// if the y coordinate of the bomb is less than -2
            bombGenerator.erase(bombGenerator.begin()+i);   /// erase the bomb from the vector

    }

    /// autimatically destroy fruit if it is outside the screen
    for(int i = 0; i < fruitGenerator.size(); i++) /// loop through every element / fruit in the vector
    {
        int vectorSize = fruitGenerator.size(); /// define integer variable to record the size of the vector
        if(fruitGenerator[i].y < -2)   /// if the y coordinate of the fruit is less than -2
        {
            fruitGenerator.erase(fruitGenerator.begin()+i); /// erase the fruit from the vector
            vectorSize--; /// subtract 1 from vectorSize
        }
    }

    glColor3f(0.0,1.0,0.0); /// set the colour to green
    drawStrokeFont(); /// call drawStrokeFont function to enable time and score text
    drawingTrail(); /// call drawingTrail function to enable blade trail feature

    if(timeLimit - Time/1000 == 0)  /// if time counter goes to 0
    {
        Time = 0;   /// reset time as 0
        menuNumber = 3; /// change menuNumber to 3, thus changing the screen to end scene
    }

    glutSwapBuffers();  ///swap the old sphere with the new one
}


void display()  /// display function
{
    if(menuNumber == 0) /// if menuNumber is equal to 0
    {
        mainMenu(); /// call mainMenu function to display main menu
    }
    else if(menuNumber == 1) /// if menuNumber is equal to 1
    {
        drawInstructionPage(); /// call drawInstructionPage function to display the instruction page
    }
    else if(menuNumber == 2) /// if menuNumber is equal to 2
    {
        mainGamePlay(); /// call mainGamePlay function to display the main game play
    }
    else if(menuNumber == 3) /// if menuNumber is equal to 3
    {
        endScene(); /// call endScene function to display end scene
    }
}



void reshape(int w,int h) /// reshape function, called when window is resized
{
    glViewport(0,0,(GLsizei)w,(GLsizei)h);  /// define view port
    glMatrixMode(GL_PROJECTION);    /// change matrix mode to projection
    glLoadIdentity();     /// load identity matrix
    glOrtho(-1.0,1.0,-1.0,1.0,-10.0,100.0);
    glMatrixMode(GL_MODELVIEW); /// change matrix mode to modelview
    glLoadIdentity();   /// load identity matrix
}


/// lighting settings
const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_position[] = { 2.0f, 5.0f, 5.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 100.0f };


int main(int argc, char* argv[])    /// main function
{
    srand ( time(NULL) );   /// initialize random number generator by getting a different time sequence every time the program runs
    glutInit(&argc,argv); /// necessary initialization for GLUT
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA); /// define the display mode
    glutInitWindowPosition(300,100); /// set the position of the window
    glutInitWindowSize(500,500); /// set the size of the window
    glutCreateWindow("Fruit Ninja"); /// window title
    glutDisplayFunc(display); /// called when display needs to be redrawn
    glutReshapeFunc(reshape); /// called when size of the window changes
    glutTimerFunc(1000, fruitTimer, 0); /// timer function call (fruitTimer)
    glutTimerFunc(1000, bombTimer, 0); /// timer function call (bombTimer)
    glutTimerFunc(20, timerIdle, 0); /// timer function call (timerIdle)

    glEnable(GL_DEPTH_TEST);    /// enabling depth test
    glDepthFunc(GL_LESS);       /// Specifing depth comparison function
    glutMouseFunc(mouse);       /// mouse function call
    glutPassiveMotionFunc(motion);  /// mouse movement (passive) call

/// lightings / shadings
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);

    glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glutMainLoop();
    return 0;
}

