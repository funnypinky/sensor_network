package com.funnypinky.server;

import com.funnypinky.server.Service.ForecastService;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class ConfigurationManager {

    @Bean
    public ForecastService forecastService() {
        return new ForecastService("https://api.openweathermap.org/data/2.5/onecall?lat=52.3333&lon=14.1&exclude=minutely,hourly","42fa60aba475ef486b3dfa098edcb3ae");
    }
}
