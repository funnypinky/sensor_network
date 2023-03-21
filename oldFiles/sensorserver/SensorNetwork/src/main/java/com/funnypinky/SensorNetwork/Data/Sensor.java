package com.funnypinky.SensorNetwork.Data;

import org.springframework.data.mongodb.core.mapping.Document;

import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.GeneratedValue;
import javax.persistence.Id;

import java.time.LocalDateTime;
import java.util.ArrayList;
import java.util.HashMap;

import static javax.persistence.GenerationType.IDENTITY;


@Document("sensor")
public class Sensor {

    @Id
    private long id;

    private String macAddress;

    private HashMap<LocalDateTime, MeasureValue> measureValues = new HashMap<>();

    public Sensor(){

    }
    public Sensor(long id) {
        this.id = id;
        macAddress = null;
    }

    public Sensor(long id, String macAddress){
        this.id = id;
        this.macAddress = macAddress;

    }
    public Sensor(String macAddress){
        this.macAddress = macAddress;

    }

    public long getId() {
        return id;
    }

    public String getMacAddress() {
        return macAddress;
    }

    public HashMap<LocalDateTime, MeasureValue> getMeasureValues(){
        return measureValues;
    }
}
