import processing.serial.*;
import processing.opengl.*;
import toxi.geom.*;
import toxi.processing.*;

final int BAUDRATE = 115200;
final int DIM = 300; // Should match viewport width & height
final int BOX_WIDTH = DIM/2;
final int BOX_HEIGHT = 2*DIM/15;
final int BOX_LENGTH = 2*DIM/3;

// NOTE: requires ToxicLibs to be installed in order to run properly.
// 1. Download from https://github.com/postspectacular/toxiclibs/releases/tag/0021
// 2. Extract into [userdir]/Processing/libraries
//    (location may be different on Mac/Linux)
// 3. Start/restart Processing

ToxiclibsSupport gfx;

Quaternion quat = new Quaternion(1, 0, 0, 0);

void setup() {
  // Viewport using OpenGL rendering. Dims should match DIM (Processing doesn't let us uses variables here)
  size(300, 300, OPENGL);
  gfx = new ToxiclibsSupport(this);

  // setup lights and antialiasing
  lights();
  smooth();

  textSize(BOX_WIDTH/8);
  
  arduinoSetup(BAUDRATE);
}

void draw() {
  readArduinoData(); // Get quaternion values from serial connection to microcontroller
  
  background(100);  // Grey background

  pushMatrix();
    translate(width / 2, height / 2); // translate everything to the middle of the viewport

    // Match orientation of animation to physical world via received quaternion values
    float[] axis = quat.toAxisAngle();
    rotate(axis[0], -axis[1], axis[3], axis[2]);

    // Draw main body of MPU
    fill(0, 0, 255, 200); // Translucent blue
    box(BOX_WIDTH, BOX_HEIGHT, BOX_LENGTH);

    // Draw main IC
    fill(0, 0, 0, 200); // Translucent black
    pushMatrix();
      translate(0, -BOX_HEIGHT/2, BOX_LENGTH/10);
      box(BOX_WIDTH/3, BOX_HEIGHT/2, BOX_WIDTH/3);
    popMatrix();

    // Draw yellow capacitor
    fill(255, 204, 102, 200); // Translucent yellow/tan
    pushMatrix();
      translate(BOX_WIDTH/3, -BOX_HEIGHT/2, BOX_LENGTH/10);
      box(BOX_WIDTH/9, BOX_HEIGHT, 5*BOX_WIDTH/18);
    popMatrix();

    // Draw some of silkscreen
    pushMatrix();
      rotateX(PI/2);
      translate(-4*BOX_WIDTH/10, -BOX_LENGTH/2, BOX_HEIGHT/2+1);
      fill(255, 255, 255, 200); // Translucent white
      text("VCC GND SCL SDA XDA XCL ADO INT", 0, BOX_WIDTH/8, 3*BOX_WIDTH/8, BOX_LENGTH);
    popMatrix();
  
  popMatrix();
}
