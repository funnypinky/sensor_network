package com.funnypinky.server.Interface;


import com.funnypinky.server.Data.Sensor;
import org.springframework.data.mongodb.core.query.Criteria;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.data.mongodb.repository.Query;
import org.springframework.stereotype.Repository;

import java.util.List;

@Repository
public interface SensorRepository extends MongoRepository<Sensor, String> {

    Sensor findByMacAddress (String macAddress);

    List<Sensor> findAll();

    Sensor save(Sensor sensor);
}
