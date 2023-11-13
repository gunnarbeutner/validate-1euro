#ifndef FILTEREDDISTANCEKALMAN_H
#define FILTEREDDISTANCEKALMAN_H

namespace kalman {

class FilteredDistance {
   public:
    FilteredDistance(float processNoise, float measurementNoise);
    void addMeasurement(float dist);
    const float getDistance() const;
    const bool hasValue() const { return !isFirstMeasurement; }

   private:
    float state[2];                // State: [0] is distance, [1] is rate of change in distance
    float covariance[2][2];        // State covariance
    unsigned long lastUpdateTime;  // Time of the last update
    float processNoise;            // Process noise (Q)
    float measurementNoise;        // Measurement noise (R)
    bool isFirstMeasurement;

    void prediction(float deltaTime);
    void update(float distanceMeasurement);
};

}

#endif  // FILTEREDDISTANCEKALMAN_H