package com.funnypinky.SensorNetwork.Interface;

import com.funnypinky.SensorNetwork.Data.Sensor;
import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.mongodb.repository.MongoRepository;

public interface SensorRepository extends MongoRepository<Sensor, Long> {
}
