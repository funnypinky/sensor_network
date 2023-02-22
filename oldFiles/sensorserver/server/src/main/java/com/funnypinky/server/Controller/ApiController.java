package com.funnypinky.server.Controller;

import com.funnypinky.server.Data.MeasureValue;
import com.funnypinky.server.Data.Sensor;
import com.funnypinky.server.Service.SensorService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Controller;
import org.springframework.web.bind.annotation.*;

import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.function.Function;
import java.util.stream.Collectors;

@RestController
@RequestMapping(value = "api/data/sensors")
public class ApiController {
    @Autowired
    private SensorService sensorService;

    @GetMapping("{sensorName}/lastDay")
    public Map<LocalDateTime, MeasureValue> getLastDay(@PathVariable("sensorName") String macAddress) {
        Sensor sensor = sensorService.findByMacAddress(macAddress);
        Map<LocalDateTime, MeasureValue> map = sensor.getLastDayValue().stream()
                .collect(Collectors.toMap(MeasureValue::getMeasTime, Function.identity()));
        return new TreeMap<>(map);
    }
}
