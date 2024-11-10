#include <SensorPanel.h>
#include <Arduino.h>

SensorPanel::SensorPanel(uint8_t *sensorPins) {
    SensorPanel::qtr.setTypeRC();
    SensorPanel::qtr.setSensorPins(sensorPins, 8);
}

bool SensorPanel::calibrate(){
    // for (int i = 0; i < seconds * 40; i++) {
    //     SensorPanel::qtr.calibrate();
    // }
    SensorPanel::qtr.calibrate();
    // uint16_t maxSensorValues[] = {2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500, 2500,
    //                              2500, 2500};
    // uint16_t minSensorValues[] = {280, 280, 188, 276, 188, 276, 192, 276, 276, 280, 184, 184, 184, 92, 92, 92};
    // qtr.virtualCalibrate(maxSensorValues, minSensorValues);

    return true;
}

uint16_t SensorPanel::readLine(uint16_t *sensorValues) {
    return SensorPanel::qtr.readLineBlack(sensorValues);
}

void SensorPanel::read() {
    SensorPanel::position = SensorPanel::readLine(panelReading);

    error = (int) position - 3500;

    for (int i = 0; i < SensorPanel::SensorCount; i++) {        
        rawReadings[i] = panelReading[i];
        panelReading[i] = panelReading[i] > 500 ? 0 : 1;
        // Serial.print(panelReading[i]);
        // Serial.print("  ");
    }
    //Serial.println(position);


    SensorPanel::updatePattern();

}

void SensorPanel::updatePattern() {

    int sensorCount = 0;

    for (int i = 0; i < 8; i++) {
        sensorCount += panelReading[i];
    }

    if(sensorCount == 8){
        pattern = 'T';
    }else{
        pattern = 'N';
    }

}
