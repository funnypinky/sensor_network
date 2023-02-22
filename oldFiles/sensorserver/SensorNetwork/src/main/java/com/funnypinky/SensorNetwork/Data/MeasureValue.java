package com.funnypinky.SensorNetwork.Data;

import java.time.LocalDateTime;

public class MeasureValue {

    private LocalDateTime measTime;
    private double temperatur;
    private double humidity;
    private double pressure;

    public MeasureValue(){}
    public MeasureValue(LocalDateTime measTime, double temperatur, double humidity, double pressure) {
        this.measTime = measTime;
        this.temperatur = temperatur;
        this.humidity = humidity;
        this.pressure = pressure;
    }

    public LocalDateTime getMeasTime() {
        return measTime;
    }

    public void setMeasTime(LocalDateTime measTime) {
        this.measTime = measTime;
    }

    public double getTemperatur() {
        return temperatur;
    }

    public void setTemperatur(double temperatur) {
        this.temperatur = temperatur;
    }

    public double getHumidity() {
        return humidity;
    }

    public void setHumidity(double humidity) {
        this.humidity = humidity;
    }

    public double getPressure() {
        return pressure;
    }

    public void setPressure(double pressure) {
        this.pressure = pressure;
    }
}
