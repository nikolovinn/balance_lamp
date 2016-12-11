

/*
   Copyright (c) 2015 Intel Corporation.  All rights reserved.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/*
   This sketch example demonstrates how the BMI160 on the
   Intel(R) Curie(TM) module can be used to read gyroscope data
*/

#include "CurieIMU.h"

//definitions of the output PINs that will be connected to the lasers
const int orderOutputPIN1 = 4;
const int orderOutputPIN2 = 5;
const int chaosOutputPIN1 = 2;
const int chaosOutputPIN2 = 3;

//Array which holds last then values from the gyro sensor
int gyroValues[10];
//Counter of the loops
int loopCounter = 0;


void setup() {
  
  Serial.begin(9600); // initialize Serial communication
  
  // initialize device
  Serial.println("Initializing IMU device...");
  CurieIMU.begin();

  // Set the accelerometer range to 250 degrees/second
  CurieIMU.setGyroRange(250);

  //setting the pins mode to OUTPUT
  pinMode(orderOutputPIN1, OUTPUT);
  pinMode(orderOutputPIN2, OUTPUT);
  pinMode(chaosOutputPIN1, OUTPUT);
  pinMode(chaosOutputPIN2, OUTPUT);
}

void loop() {
  int gxRaw, gyRaw, gzRaw;         //definition of variables that will hold raw gyro values
  float gx, gy, total;      //definition of variables that will hold converted gyro values and a total gyro values

  // read raw gyro measurements from device
  CurieIMU.readGyro(gxRaw, gyRaw, gzRaw);

  // convert the raw gyro data to degrees/second
  gx = convertRawGyro(gxRaw);
  gy = convertRawGyro(gyRaw);

  // calculate the total change per second for the x and y axis
  // we are using the absolute values because we do not need to know the direction of the change
  total =  abs(gx) + abs(gy);

  //if the counter value is less than 10
  //store the value in the gyroValues array
  // and increment the counter
  if(loopCounter < 10)
  {
    gyroValues[loopCounter] = total;
    loopCounter++;
  }
  //otherwise calculate the average of all 10 values in order to get not so chaotical results
  // and turn on or off different lasers
  else
  {
     total = 0;
      for(int i = 0; i < 10; i++)
      {
        total += gyroValues[i];
      }

      total /= 10;

      //if the changes is less the 15 degrees/second
      //then we activate the order lasers
      // and deactivate the chaos lasers
      if(total < 15)
      {
          digitalWrite(orderOutputPIN1, HIGH);
          digitalWrite(orderOutputPIN2, HIGH);
          digitalWrite(chaosOutputPIN1, LOW);
          digitalWrite(chaosOutputPIN2, LOW);
      }
      //otherwise vice-versa 
      else if(total > 15)
      {
         digitalWrite(orderOutputPIN1, LOW);
          digitalWrite(orderOutputPIN2, LOW);
          digitalWrite(chaosOutputPIN1, HIGH);
          digitalWrite(chaosOutputPIN2, HIGH);
      }
      // and set the counter to 0 
      // so the program starts to add new ten values to the array
      loopCounter = 0;
  }

  delay(300);
}

float convertRawGyro(int gRaw) {
  // since we are using 250 degrees/seconds range
  // -250 maps to a raw value of -32768
  // +250 maps to a raw value of 32767
  
  float g = (gRaw * 250.0) / 32768.0;

  return g;
}

