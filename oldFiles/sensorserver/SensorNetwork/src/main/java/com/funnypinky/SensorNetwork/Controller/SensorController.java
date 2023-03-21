package com.funnypinky.SensorNetwork.Controller;

import com.funnypinky.SensorNetwork.Data.Sensor;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.GetMapping;

import java.util.Collections;
import java.util.List;

@Controller("sensors")
public class SensorController {

    @GetMapping
    public ResponseEntity<List<Sensor>> getTasks() {
        Sensor sensor = new Sensor(1L, "E8:68:E7:2D:28:28");
        List<Sensor> taskList = Collections.singletonList(sensor);

        return ResponseEntity.ok(taskList);
    }

}
