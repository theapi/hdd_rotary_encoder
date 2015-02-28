import processing.serial.*;
Serial myPort;        // The serial port
int omy = 0;
float cosx, siny;
void setup () {
  // set the window size:
  size(250, 250);        

  // List all the available serial ports
  println(Serial.list());// look in the window when the program runs to see which port to use
  myPort = new Serial(this, Serial.list()[0], 57600);  
  
  // don't generate a serialEvent() unless you get a newline character:
  myPort.bufferUntil('\n');
  // set inital background:
  
  colorMode(HSB, height, height, height); 
}
void draw () {
  // everything happens in the serialEvent()
}

void serialEvent (Serial myPort) {// triggers when new data comes in
background(0);
  String pulses_raw = myPort.readStringUntil('\n'); //read until the new line, like print ln
  
  if (pulses_raw != null) {
    // trim off any whitespace:
    pulses_raw = trim(pulses_raw);
    // convert to an int and map to the screen height:
    float pulses = float(pulses_raw); 
    pulses = pulses * -1; // invert
   
    omy = int(pulses);
    if (omy < 0) {
      omy = omy * -1; // invert
    }
    if (omy < 90) {
      omy = 90; 
    }

    
   
    // 24 per full rotation
    pulses = pulses*3.141592654/12;
    
    siny= sin(pulses);
    cosx= cos(pulses); 
    
    stroke(omy,omy,omy);
    strokeWeight(10);
    fill(omy,omy,omy);

    int h = height -50;
    int w = width - 50;
    //line(width/2, height/2, width/2+width/2*cosx, height/2+ height/2*siny);
    line(width/2, height/2, width/2+ w/2*cosx, height/2+ h/2*siny);

    text(pulses_raw, 25, height -25);
    
 
  }
}


