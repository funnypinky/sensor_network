package com.funnypinky.server.Data;

import lombok.Getter;
import lombok.Setter;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.index.Indexed;
import org.springframework.data.mongodb.core.mapping.Document;

import java.time.LocalDate;
import java.time.LocalDateTime;
import java.util.*;
import java.util.stream.Collectors;

@Getter
@Setter
@Document(value ="sensor")
public class Sensor {
    static final double  VOLTAGE_MAX = 4.95;
    static final double VOLTAGE_MIN = 3.300;
    @Id
    private String id;
    @Indexed(unique = true)
    private String macAddress;

    private String displayName = "unknown";

    private List<MeasureValue> measureValues = new ArrayList<>();

    private TreeMap<LocalDateTime, Double> batteryValues = new TreeMap<>();

    public Sensor(){

    }
    public Sensor(String macAddress) {
        this.macAddress = macAddress;
    }

    public Sensor(String id, String macAddress){
        this.id = id;
        this.macAddress = macAddress;

    }

    public String getId() {
        return id;
    }

    public String getMacAddress() {
        return macAddress;
    }

    public List< MeasureValue> getMeasureValues(){
        return measureValues;
    }

    public List<MeasureValue> getSortedList(){
        Comparator<MeasureValue> comparator = (c1, c2) -> c1.getMeasTime().compareTo(c2.getMeasTime());
        List<MeasureValue> sortList = this.measureValues;
        Collections.sort(sortList,comparator);
        return sortList;
    }

    public List<MeasureValue> getLastDayValue() {
        LocalDate today = getSortedList().get(getSortedList().size()-1).getMeasTime().toLocalDate();
        List<MeasureValue> returnList = new ArrayList<>();
        for (MeasureValue item: this.measureValues) {
            if(item.getMeasTime().toLocalDate().isEqual(today))  {
                returnList.add(item);
            }
        }
        return returnList;
    }
    public String getDisplayName() {
        return displayName;
    }

    public TreeMap<LocalDateTime, Double> getBatteryValues() {
        return batteryValues;
    }

    public void setDisplayName(String displayName) {
        this.displayName = displayName;
    }
    public MeasureValue getLastValue(){
        return getSortedList().get(getSortedList().size()-1);
    }
    public double getBatteryLevel() {
        double adc = this.batteryValues.get(this.batteryValues.lastKey());
        double battery_percentage = 100 * (adc - VOLTAGE_MIN) / (VOLTAGE_MAX - VOLTAGE_MIN);

        if (battery_percentage < 0)
            battery_percentage = 0;
        if (battery_percentage > 100)
            battery_percentage = 100;

        return battery_percentage;
    }
}

