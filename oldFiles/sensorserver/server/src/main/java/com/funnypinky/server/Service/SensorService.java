package com.funnypinky.server.Service;

import com.funnypinky.server.Data.Sensor;
import com.funnypinky.server.Interface.SensorRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.mongodb.core.MongoTemplate;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SensorService {
    @Autowired
    private final SensorRepository sensorRepository;

    @Autowired
    private MongoTemplate mongoTemplate;

    public SensorService(SensorRepository sensorRepository) {
        this.sensorRepository = sensorRepository;
    }

    public List<Sensor> list() {
        return sensorRepository.findAll();
    }

    public Sensor saveSensor(Sensor sensor) {
        return this.sensorRepository.save(sensor);
    }
    public Sensor findByMacAddress(String macAddress) {
        return sensorRepository.findByMacAddress(macAddress);
    }
}
