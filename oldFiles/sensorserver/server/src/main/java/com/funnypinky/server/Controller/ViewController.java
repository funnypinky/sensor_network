package com.funnypinky.server.Controller;

import com.funnypinky.server.Data.Sensor;
import com.funnypinky.server.Service.ForecastService;
import com.funnypinky.server.Service.SensorService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ResponseEntity;
import org.springframework.stereotype.Controller;
import org.springframework.ui.Model;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestMethod;
import org.springframework.web.bind.annotation.RequestParam;

import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;

@Controller
public class ViewController {

    @Autowired
    private SensorService sensorService;
    private ArrayList<ForecastService> forecastList = new ArrayList<>();
    @Autowired
    public ViewController() {
        //https://api.openweathermap.org/data/2.5/onecall?lat=52.3333&lon=14.1&exclude=minutely,hourly&appid=42fa60aba475ef486b3dfa098edcb3ae&units=metric
        this.forecastList.add(new ForecastService("https://api.openweathermap.org/data/2.5/onecall?lat=52.3333&lon=14.1&exclude=minutely,hourly","42fa60aba475ef486b3dfa098edcb3ae"));
    }
    @GetMapping("/")
    public String home(Model model) {
        model.addAttribute("sensors", sensorService.list());
        return "index";
    }
    @RequestMapping(value = "/", method = RequestMethod.POST)
    public String handleForm(@RequestParam("displayName") String name, @RequestParam("macAddress") String macAddress) {
        Sensor sensor = sensorService.findByMacAddress(macAddress);
        sensor.setDisplayName(name);
        sensorService.saveSensor(sensor);
        return "redirect:/";
    }
}
