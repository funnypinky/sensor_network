package com.funnypinky.SensorNetwork.Service.mqtt;


import com.funnypinky.SensorNetwork.Data.MeasureValue;
import com.funnypinky.SensorNetwork.Data.Sensor;
import com.funnypinky.SensorNetwork.Interface.SensorRepository;
import com.funnypinky.SensorNetwork.Service.SensorService;
import com.google.common.base.Predicate;
import com.google.common.collect.Iterables;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttMessage;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Component;

import java.lang.reflect.Array;
import java.time.Instant;
import java.time.LocalDateTime;
import java.time.ZoneOffset;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.TimeZone;

@Component
public class PushCallback implements MqttCallback {

    @Autowired
    private SensorService sensorService;


    public PushCallback() {

    }


    @Override
    public void connectionLost(Throwable throwable) {

    }

    @Override
    public void messageArrived(String topic, MqttMessage message) throws Exception {
        JSONObject content = new JSONObject(message.toString());
        ArrayList<Sensor> sensors = (ArrayList<Sensor>) sensorService.list();
        String sensorName = topic;
        if(topic.equals(content.getString("source"))){
            Sensor sensor = Iterables.tryFind(sensors,
                    new Predicate<Sensor>() {
                        public boolean apply(Sensor tempSensor) {
                            return sensorName.equals(tempSensor.getMacAddress());
                        }
                    }).or(new Sensor(sensorName));
            LocalDateTime time = LocalDateTime.ofEpochSecond(content.getLong("time"),0,ZoneOffset.UTC);
            double temperature = content.getDouble("temperature");
            double humidity = content.getDouble("humidity");
            double pressure = content.getDouble("pressure");
            MeasureValue value = new MeasureValue(time,temperature,humidity,pressure);
            sensor.getMeasureValues().put(time,value);
            sensorService.save(sensor);
        }
    }

    @Override
    public void deliveryComplete(IMqttDeliveryToken iMqttDeliveryToken) {

    }
}
