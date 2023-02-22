package com.funnypinky.SensorNetwork.Service;

import com.funnypinky.SensorNetwork.Data.Sensor;
import com.funnypinky.SensorNetwork.Interface.SensorRepository;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.List;

@Service
public class SensorService {
    @Autowired
    private SensorRepository sensorRepository;

    public List<Sensor> list() {
        return sensorRepository.findAll();
    }

    public void save(Sensor sensor){
        sensorRepository.save(sensor);
    }
}
