import processing.serial.*;

String SERIALPORT; // Name of bluetooth serial port that receives data from Arduino

Serial serial; // Serial port declaration

void arduinoSetup (int baudRate) {
  println("Setting up Arduino...");
  
  println("Ports detected:");
  printArray(Serial.list());
  println("Checking for active port... ");
  
  boolean doesPortExist = false;
  while (!doesPortExist) {
    String sList [] = Serial.list();
    
    for (int i=0; i<sList.length; i++) {
      serial = new Serial(this, sList[i], baudRate); // Open serial port
      delay(10);
      
      if (serial.available() > 0) { // Check if any data is being sent over active port
        doesPortExist = true; // If so, assume it is the one we want to be using
        SERIALPORT = sList[i]; // Assign global
        
        print("Using port: ");
        println(SERIALPORT);
        
        break;
      } else {
        serial.stop(); // Otherwise, close the port and check the next
        delay(10);
      }
    }
    if (!doesPortExist) println("No active ports found, checking again...");
  }
  
  delay(500);
  
  println("Arduino setup complete.");
}

// Data coming in in the form (q0,q1,q2,q3), read each line and store into quat
void readArduinoData () {
  String incomingData;
  String parsedData;
  
  do {
    incomingData = serial.readStringUntil('\n'); // Read a single line of the string
    if (incomingData != null) {
      if (incomingData.charAt(0) != '(') continue; // Message should start with an open parentheses
      parsedData = incomingData.trim(); // Remove newlines, carriage returns
      parsedData = parsedData.substring(1, parsedData.length() - 1); // Remove parentheses
      String[] q = split(parsedData, ','); // Split on commas
      if (q.length == 4) {
        quat.set(float(q[0]), float(q[1]), float(q[2]), float(q[3]));
      }
    }
  } while (incomingData != null);
}
