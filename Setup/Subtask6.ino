#include <Enes100.h>

//To compile you better have helpers.h and Enes100.h. I also edited helpers.h so you need to update it.


const float kP = 100.0;
#define MAX_TURN_SPEED 255


void setAngle(double target) {
  ps("Targeting angle: ");
  pl(target);
  Enes100.updateLocation();
  const double thresh = .15;
  double *curr = &Enes100.location.theta;
  int left = 0, right = 0;
  if (target - theta > 0) { // need to turn left
    left = MAX_TURN_SPEED;
    right = -1 * MAX_TURN_SPEED;
  } else if (target - theta < 0) { // need to turn right
    left = -1 * MAX_TURN_SPEED;
    right = MAX_TURN_SPEED;

  }
  updateCoords();
  while (abs(target - theta) > thresh) {
    //    while (true) {
    updateCoords();
    //        put(*curr);
    //        put(target);
    //        put(millis());
    double p = kP * (target - *curr);
    //        putl(p);
    p = constrain(p, -MAX_TURN_SPEED, MAX_TURN_SPEED);
    //        putl(p);

    Enes100.print("Theta: ");
    Enes100.print(theta);
    Enes100.print(" Target: ");
    Enes100.println(target);
    setMotors(left, right);
    delay(100);
    setMotors(0);
  }
  ps("Got to angle: ");
  putl(target);
  putl(*curr);
  setMotors(0);
}

void moveTo(double tx, double ty) {
  ps("Going to x:");
  p(tx);
  ps(" y:");
  pl(ty);
  double dis = 100;
  double speed = 0;
  while (dis > 0.2) { //Within 5 CM
    updateCoords();
    dis = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    //We could go directly to a point, but we could also shoot for a point on a line.
    float angle = atan2(ty - y, tx - x);
    float newTheta = theta + HALF_PI;
    if (newTheta > PI) newTheta -= 2 * PI;
    //    ps("c:");
    //    p(toDeg(newTheta));
    //Our current will stay the same. It is within the range -pi/2 to 3pi/2
    //Our target anle, however will change based on how we want to approach the thing.
    //For example, if (assume for a second our current is -pi to pi) we are targeting
    //.9pi and are currently at -.9 pi, we should instead try to target -1.1 pi, which is equivalent
    //We will try to find a potential new target.
    //Basically, we should be able to get the target within a pi of the current.
    float delta = abs(angle - theta);
    if (delta > PI) {
      if (angle > PI) {
        angle = angle - 2 * PI;
      }
      else {
        angle = angle + 2 * PI;
      }
    }
    while (angle > PI) {
      angle -= PI;
    }
    while (angle < -1 * PI) {
      angle += PI;
    }
    setAngle(angle);
    //    p(" ft:") //fixed target;
    //    p(toDeg(angle));
    const float alpha = 0.35;
    speed = alpha * (20 / (abs(angle - theta))) + ((1 - alpha) * speed);
    if (!(speed < 200)) speed = 200;
    if (dis < 0.2) //If we are closer than 20 cm, aka twice the length of richaron.
      speed *= dis * .5;// @ dis = 0.2, dis * 0.5 = 1. This way it linearly slows to a certian point.
    speed = 255;
    //    ps(" s: ");
    //    p(speed);
    double p = kP * (angle - newTheta);
    p = constrain(p, -MAX_TURN_SPEED, MAX_TURN_SPEED);
    //    putl(dis);
    setMotors(speed);
    delay(200);
    setMotors(0, 0);
  }
}

void moveToMission(double tx, double ty) {
  ps("Going to x:");
  p(tx);
  ps(" y:");
  pl(ty);
  double dis = 100;
  double speed = 0;

  while (dis > 0.1) { //Within 10 CM
    updateCoords();
    dis = abs(ty - y);
    //    dis = sqrt((tx - x) * (tx - x) + (ty - y) * (ty - y));
    //We could go directly to a point, but we could also shoot for a point on a line.
    float angle = atan2(ty - y, tx - x);
    float newTheta = theta + HALF_PI;
    if (newTheta > PI) newTheta -= 2 * PI;
    //    ps("c:");
    //    p(toDeg(newTheta));
    //Our current will stay the same. It is within the range -pi/2 to 3pi/2
    //Our target anle, however will change based on how we want to approach the thing.
    //For example, if (assume for a second our current is -pi to pi) we are targeting
    //.9pi and are currently at -.9 pi, we should instead try to target -1.1 pi, which is equivalent
    //We will try to find a potential new target.
    //Basically, we should be able to get the target within a pi of the current.
    float delta = abs(angle - theta);
    if (delta > PI) {
      if (angle > PI) {
        angle = angle - 2 * PI;
      }
      else {
        angle = angle + 2 * PI;
      }
    }
    while (angle > PI) {
      angle -= PI;
    }
    while (angle < -1 * PI) {
      angle += PI;
    }
    setAngle(angle);
    //    p(" dis:") //fixed target;
    //    p(dis);
    speed *= dis * .5;// @ dis = 0.2, dis * 0.5 = 1. This way it linearly slows to a certian point.
    speed = 200;
    //    ps(" s: ");
    //    p(speed);
    setMotors(speed);
    delay(55);
    setMotors(0, 0);
  }
}
void randomWobble() {
  //1 second of (not) random wobble
  setMotors(255, 0);
  delay(200);
  setMotors(0, 255);
  delay(200);
  setMotors(255, 0);
  delay(200);
  setMotors(0, 255);
  delay(200);
  setMotors(255, 0);
  delay(200);
  setMotors(0);
}

bool sanityCheck() {
  return Enes100.location.theta < PI * 1.1 &&
         Enes100.location.theta > -PI * 1.1 &&
         Enes100.location.x > -0.1 &&
         Enes100.location.x < 4.1 &&
         Enes100.location.y > -0.1 &&
         Enes100.location.y < 2.1;
}

float toDeg(float rad) {
  return 180.0f * rad / PI;
}

//For TDS sensor
int getMedianNum(int bArray[], int iFilterLen)
{
  int bTab[iFilterLen];
  for (byte i = 0; i < iFilterLen; i++)
    bTab[i] = bArray[i];
  int i, j, bTemp;
  for (j = 0; j < iFilterLen - 1; j++)
  {
    for (i = 0; i < iFilterLen - j - 1; i++)
    {
      if (bTab[i] > bTab[i + 1])
      {
        bTemp = bTab[i];
        bTab[i] = bTab[i + 1];
        bTab[i + 1] = bTemp;
      }
    }
  }
  if ((iFilterLen & 1) > 0)
    bTemp = bTab[(iFilterLen - 1) / 2];
  else
    bTemp = (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
  return bTemp;
}

//return true if salty
bool getSalinity(){
  static unsigned long analogSampleTimepoint = millis();
   if(millis()-analogSampleTimepoint > 40U)     //every 40 milliseconds,read the analog value from the ADC
   {
     analogSampleTimepoint = millis();
     analogBuffer[analogBufferIndex] = analogRead(TdsSensorPin);    //read the analog value and store into the buffer
     analogBufferIndex++;
     if(analogBufferIndex == SCOUNT) 
         analogBufferIndex = 0;
   }   
   static unsigned long printTimepoint = millis();
//   if(millis()-printTimepoint > 40U)
//   {
      printTimepoint = millis();
      for(copyIndex=0;copyIndex<SCOUNT;copyIndex++)
        analogBufferTemp[copyIndex]= analogBuffer[copyIndex];
      averageVoltage = getMedianNum(analogBufferTemp,SCOUNT) * (float)VREF / 1024.0; // read the analog value more stable by the median filtering algorithm, and convert to voltage value
      float compensationCoefficient=1.0+0.02*(temperature-25.0);    //temperature compensation formula: fFinalResult(25^C) = fFinalResult(current)/(1.0+0.02*(fTP-25.0));
      float compensationVolatge=averageVoltage/compensationCoefficient;  //temperature compensation
      tdsValue=(133.42*compensationVolatge*compensationVolatge*compensationVolatge - 255.86*compensationVolatge*compensationVolatge + 857.39*compensationVolatge)*0.5; //convert voltage value to tds value
      Serial.print("voltage:");
      Serial.print(averageVoltage,2);
      Serial.print("V   ");
      Serial.print("TDS Value:");
      Serial.print(tdsValue,0);
      Serial.println("ppm"); 
//   }
      if (tdsValue < 300)
      return false;
    else
      return true;

   
  // return false;
}
