package com.funnypinky.server.Service.mqtt;


import com.funnypinky.server.Data.MeasureValue;
import com.funnypinky.server.Data.Sensor;
import com.funnypinky.server.Interface.SensorRepository;
import com.funnypinky.server.Service.SensorService;
import com.google.common.base.Predicate;
import com.google.common.collect.Iterables;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.lang.reflect.Array;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.TimeZone;

@Component
public class PushCallback implements MqttCallback {

    @Autowired
    private SensorService sensorService;

    @Autowired
    private SensorRepository repository;

    public PushCallback(SensorRepository repository) {
        this.repository = repository;
    }

    public PushCallback() {

    }

    @Override
    public void connectionLost(Throwable throwable) {

    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        String regex = "^([0-9A-Fa-f]{2}[:-]){5}([0-9A-Fa-f]{2})|([0-9a-fA-F]{4}\\.[0-9a-fA-F]{4}\\.[0-9a-fA-F]{4})$";
        if (topic.matches(regex)) {
            JSONObject content = new JSONObject(message.toString());
            ArrayList<Sensor> sensorList = (ArrayList<Sensor>) repository.findAll();
            Sensor sensor = Iterables.tryFind(sensorList,
                    new Predicate<Sensor>() {
                        public boolean apply(Sensor tempSensor) {
                            return topic.equals(tempSensor.getMacAddress());
                        }
                    }).or(new Sensor(topic));
            LocalDateTime time = LocalDateTime.ofEpochSecond(content.getLong("time"), 0, ZoneOffset.ofHours(2));
            double temperature = content.getDouble("temperature");
            double humidity = content.getDouble("humidity");
            double pressure = content.getDouble("pressure");
            MeasureValue value = new MeasureValue(time, temperature, humidity, pressure);
            sensor.getMeasureValues().add(value);
            sensor.getBatteryValues().put(time, content.getDouble("battery"));
            Sensor testReturn = repository.save(sensor);
            if (testReturn == null) {
                System.err.println("Sensor not saved");
            } else if (testReturn.equals(sensor)) {
                System.out.println("Sensor saved successfully");
            }
        }
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

    }
}
