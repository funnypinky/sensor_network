package com.funnypinky.server.Service.mqtt;

import com.funnypinky.server.Data.MeasureValue;
import com.funnypinky.server.Data.Sensor;
import com.funnypinky.server.Service.SensorService;
import netscape.javascript.JSObject;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.List;

@Component
public class MqttPubClient {

    @Autowired
    private SensorService sensorService;
    @Autowired
    private MqttPushClient mqttPushClient;

    @Scheduled(cron = "*/60 * * * * *")
    public void pubSensor() {
        List<Sensor> sensors = sensorService.list();
        for (Sensor item: sensors) {
            MeasureValue lastValue = item.getLastValue();
            JSONObject jsonObject = new JSONObject();
            jsonObject.append("temperatur",lastValue.getTemperatur());
            jsonObject.append("humidity",lastValue.getHumidity());
            jsonObject.append("pressure",lastValue.getPressure());
            this.mqttPushClient.publish(1,true,"sensor/"+item.getDisplayName(),jsonObject.toString());
        }
    }
}
