#define SLRC24_PID_H
#ifndef SLRC24_PID_H

#endif //SLRC24_PID_H

const double kPs = 0.038;
const double kIs = 0.0003;
const double kDs = 0.33;

const double kPf = 0.031;
const double kIf = 0.0003;
const double kDf = 0.33;

int totalError = 0;
int prevError = 0;

int pid(int error, char mode = 's') {
    totalError += error;

    double p = mode == 's' ? error * kPs : error * kPf;
    double i = mode == 's' ? totalError * kIs : totalError * kIf;
    double d = mode == 's' ? (error - prevError) * kDs : (error - prevError) * kDf;
    
    prevError = error;

    int correction = (int)(p + i + d);

    return correction;
}

