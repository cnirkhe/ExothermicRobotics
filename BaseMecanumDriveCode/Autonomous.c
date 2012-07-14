#pragma config(Sensor, dgtl1,  LeftBackEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl3,  LeftFrontEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl5,  ,               sensorTouch)
#pragma config(Sensor, dgtl6,  ,               sensorTouch)
#pragma config(Sensor, dgtl7,  ,               sensorTouch)
#pragma config(Sensor, dgtl8,  ,               sensorTouch)
#pragma config(Sensor, dgtl9,  RightFrontEncoder, sensorQuadEncoder)
#pragma config(Sensor, dgtl11, RightBackEncoder, sensorQuadEncoder)
#pragma config(Motor,  port1,           LeftBackTop,   tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port2,           LeftBackBottom, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port3,           LeftFront,     tmotorServoContinuousRotation, openLoop, reversed)
#pragma config(Motor,  port4,           ExtraOne,      tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port7,           ExtraTwo,      tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port8,           RightFront,    tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port9,           RightBackBottom, tmotorServoContinuousRotation, openLoop)
#pragma config(Motor,  port10,          RightBackTop,  tmotorServoContinuousRotation, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#include "Utility.c";


void autonomousprogram()
{


}



void automove(int angle, int distance)
{
    //We will use these trignometric values a lot
    //So to avoid computation time, we just define their values early on
    //Look in the math package to realize what is going on
    float cos_x = fcosine(angle), sin_x = fsine(angle);

    //Using trig, I will convert the movement to the appropriate linear components
    int x_distance = (int)(distance * cos_x);
    int y_distance = (int)(distance * sin_x);

    ResetEncoders();

    //Variable for PID loop
    int power = 0;

    //Defining them up here so that they make sense scope-wise
    int x_traveled = 0;
    int y_traveled = 0;

    //The PID Progression used will be a gradual ramp up with a defining power variable which will be indexed.
    while( abs(x_traveled - x_distance) + abs(y_traveled - y_distance) < 40) //The number 40 is just a constant for accuracy
    {
        //Calculation of values traveled
        x_traveled = (Encoder(LF) + Encoder(RB) - Encoder(RF) - Encoder(LB))/4;
        int y_traveled =  (Encoder(LF) + Encoder(RB) + Encoder(RF) + Encoder(LB))/4;

        //We split it up this way because division is difficult and time consuming
        if (4 * abs(x_traveled) > 3 * abs(x_distance) || 4 * abs(y_traveled) > 3 * abs(y_distance))
        {
          //RAMP DOWN
          //here the power will be proportional to the distance required to travel
          //we will define two power variables (in the x and y)
          int power_x = (x_distance - x_traveled) * 127.0 / x_distance;
          int power_y = (y_distance - y_traveled) * 127.0 / y_distance;
          move(LF, power_y + power_x);
          move(RB, power_y + power_x);
          move(LB, power_y - power_x);
          move(RF, power_y - power_x);

        }
        else
        {
          //RAMP UP AND MAINTAINING SPEED
          if (power <= 127)
          {
            power = min(127, power + 5);

            //Movement functions for standard motion
            move(LF, power * (sin_x + cos_x));
            move(RB, power * (sin_x + cos_x));
            move(LB, power * (sin_x - cos_x));
            move(RF, power * (sin_x - cos_x));

            wait1Msec(30); //So that ramp up is not instantaneous


          }
        }
    }
}

//We will define positive as clockwise and negative as counterclockwise
void autorotate(int distance)
{
  ResetEncoders();

  //The distance we care about traveling is simply the encoder count of the motors

  //Initializing these variables so that they have a good scope.
  int left_traveled = 0, right_traveled = 0;

  while(abs(abs(left_traveled) - abs(distance)) + abs(abs(right_traveled) - abs(distance)) < 40) //40 is a constant.
  {
     left_traveled = (Encoder(LF) + Encoder(LB)) / 2;
     right_traveled = (Encoder(RF) + Encoder(RB)) / 2;

     if ( 4 * abs(abs(distance) - abs(left_traveled)) > 3 * abs(distance) || 4 * abs(abs(distance) - abs(right_traveled)) > 3 * abs(distance))
     {
          //RAMP DOWN
          //here the power will be proportional to the distance required to travel
          //These are only the magnitudes of the powers
          int power_left = abs(abs(distance) - abs(left_traveled)) * 127.0 / abs(distance);
          int power_right = abs(abs(distance) - abs(left_traveled)) * 127.0 / abs(distance);

          //Setting direction based on initial command
          //Read online about the trinary operator if you are confused by the syntax.
          (distance < 0 ? power_left : power_right) *= -1;

          //Setting direction based on current motion
          if (abs(left_traveled) > abs(distance))
          {
            power_left *= -1;
          }
          if (abs(right_traveled) > abs(distance))
          {
            power_right *= -1;
          }

          move(LF, power_left);
          move(RB, power_left);
          move(LB, power_left);
          move(RF, power_left);


     }
     else
     {
          //RAMP UP AND MAINTAINING SPEED
          if (power <= 127)
          {
            power = min(127, power + 5);

            //Setting the magnitudes
            int power_left = power, power_right = power;

            //Setting the direction
            (distance < 0 ? power_left : power_right) *= -1;

            //Movement functions for standard motion
            move(LF, power_left);
            move(RB, power_left);
            move(LB, power_right);
            move(RF, power_right);

            wait1Msec(30); //So that ramp up is not instantaneous


          }


     }
  }

}
