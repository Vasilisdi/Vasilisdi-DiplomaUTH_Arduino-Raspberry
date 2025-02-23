const int numChannels = 3;  // Number of analog channels to read
const int analogPins[numChannels] = {A0, A1, A2};  // Analog input pins
const int bufferSize = 10;  // Size of the buffer for each channel

// Arrays to store raw readings and buffer index for each channel
int analogBuffer[numChannels][bufferSize];
int bufferIndex[numChannels];

float tempVal = 0;

void setup() {
    Serial.begin(115200);

    // Initialize buffer index for each channel
    for (int i = 0; i < numChannels; i++) {
        bufferIndex[i] = 0;
    }
}

void loop() {
    // Read analog inputs and store readings in the buffer
    for (int j = 0; j<bufferSize; j++) {
      
      for (int i = 0; i < numChannels; i++) {
          int xRaw = analogRead(analogPins[i]);
          analogBuffer[i][j] = xRaw;
      }
    }
      
    if (tempVal == 100) {
        // Perform calibration or other operations if needed
        tempVal = 0;
    } else {
        // Send data over serial
        for (int j = 0; j < bufferSize; j++) {
          for (int i = 0; i < numChannels; i++) {        
                Serial.print(analogBuffer[i][j]);
                if (i < numChannels - 1) { // Print comma for all values except the last one
                    Serial.print(" , ");
                }
                
            }
            Serial.println(); // Print a newline after each channel's data
            
        }

        tempVal += 1;
    }
}
