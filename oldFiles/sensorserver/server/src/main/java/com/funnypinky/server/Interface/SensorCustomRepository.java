package com.funnypinky.server.Interface;

import com.funnypinky.server.Data.MeasureValue;

public interface SensorCustomRepository {

    int updateMeasValue(String macAddress, MeasureValue value);
}
