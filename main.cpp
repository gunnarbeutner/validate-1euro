#include <iostream>
#include <matplot/matplot.h>
#include "FilteredDistance.h"
#include "FilteredDistanceKalman.h"
#include "1efilter.h"
#include "datasets.h"

static unsigned long currentTime;

unsigned long micros() {
    return currentTime;
}

#define BROKEN_ONE_EURO_FCMIN 1e-5f
#define BROKEN_ONE_EURO_BETA 1e-7f
#define BROKEN_ONE_EURO_DCUTOFF 1e-5f

#define ONE_EURO_FCMIN 1e-1f
#define ONE_EURO_BETA 1e-3f
#define ONE_EURO_DCUTOFF 5e-3f

#define CUSTOM_ONE_EURO_FCMIN 0.07
#define CUSTOM_ONE_EURO_BETA 1e-3f
#define CUSTOM_ONE_EURO_DCUTOFF 5e-3f

const int refRssi = -54;
const float absorption = 3.5f;

static float distanceForRssi(int rssi) {
    return pow(10, float(refRssi - rssi) / (10.0f * absorption));
}

int main(int, char**){
    FilteredDistance newFilter(ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    broken_one_euro_filter<> oldFilter(1, BROKEN_ONE_EURO_FCMIN, BROKEN_ONE_EURO_BETA, BROKEN_ONE_EURO_DCUTOFF);
    one_euro_filter<> euroFilter(1, ONE_EURO_FCMIN, ONE_EURO_BETA, ONE_EURO_DCUTOFF);
    kalman::FilteredDistance kalmanFilter(0.1, 25);
    FilteredDistance customFilter(CUSTOM_ONE_EURO_FCMIN, CUSTOM_ONE_EURO_BETA, CUSTOM_ONE_EURO_DCUTOFF);

    std::vector<double> tsData, uwbData, rawData, oldData, euroData, newData, kalmanData, customData;

    const auto& measurements = moving_dev;

    const auto startTime = std::get<0>(measurements[0]);

    for (const auto [timestamp, rssi, uwbDistance] : measurements) {
        uwbData.push_back(uwbDistance);

        const auto distance = distanceForRssi(rssi);
        rawData.push_back(distance);

        currentTime = timestamp - startTime;
        tsData.push_back(currentTime / 1e6f);

        newFilter.addMeasurement(distance);
        auto newResult = newFilter.getDistance();
        newData.push_back(newResult);

        kalmanFilter.addMeasurement(distance);
        auto kalmanResult = kalmanFilter.getDistance();
        kalmanData.push_back(kalmanResult);

        // 1€ filter expects timestamps to be in seconds, but we were incorrectly using milliseconds
        auto oldResult = oldFilter(distance, micros() / 1e3f);
        oldData.push_back(oldResult);

        auto euroResult = euroFilter(distance, micros() / 1e6f);
        euroData.push_back(euroResult);

        customFilter.addMeasurement(distance);
        auto customResult = customFilter.getDistance();
        customData.push_back(customResult);

        std::cout << "ts: " << currentTime / 1e6
                  << "; uwb: " << uwbDistance
                  << "; raw: " << distance
                  << "; new: " << newResult
                  << "; kalman: " << kalmanResult
                  << "; old: " << oldResult
                  << "; 1€: " << euroResult
                  << "; custom: " << customResult << "\n";
    }

    std::vector<std::string> labels;

    matplot::hold(matplot::on);

    //labels.push_back("uwb");
    //matplot::plot(tsData, uwbData, "--");

    labels.push_back("raw");
    matplot::plot(tsData, rawData);

    //labels.push_back("old ESPresense");
    //matplot::plot(tsData, oldData, "--+");

    labels.push_back("new ESPresense");
    matplot::plot(tsData, newData, "--o");

    labels.push_back("kalman");
    matplot::plot(tsData, kalmanData, "--o");

    //labels.push_back("1€");
    //matplot::plot(tsData, euroData, "--d");

    //labels.push_back("new ESPresense (custom params)");
    //matplot::plot(tsData, customData, "--s");

    matplot::xlabel("Time (s)");
    matplot::ylabel("Distance (m)");
    matplot::legend(labels);
    matplot::show();
}
