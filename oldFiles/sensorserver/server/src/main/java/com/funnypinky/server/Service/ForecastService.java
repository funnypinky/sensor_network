package com.funnypinky.server.Service;

import org.json.JSONObject;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.time.ZonedDateTime;
import java.util.Map;

/*
* @TODO: Cache the forecast for 1 hour -done
* */
@RestController
@RequestMapping(value = "api/data/forecast")
public class ForecastService {
    private String url;
    private String apiKey;

    private String jsonString;

    private long lastCall = 0;

    public ForecastService(String url, String apiKey) {
        this.apiKey = apiKey;
        this.url = url;
    }

    @GetMapping("/")
    public Map<String, Object> getForecast() {
        return pullForecast().toMap();
    }
    public JSONObject pullForecast() {
        String urlString = url+"&appid=" + this.apiKey + "&units=metric";
        StringBuilder result = new StringBuilder();
        URL url = null;
        JSONObject returnObj = null;
        long current = ZonedDateTime.now().toInstant().toEpochMilli();

        try {
            if(current-lastCall >= 3.6e+6) {
                url = new URL(urlString);
                URLConnection conn = url.openConnection();
                BufferedReader rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
                String line;
                while ((line = rd.readLine()) != null) {
                    result.append(line);
                }
                rd.close();
                jsonString = result.toString();
                lastCall = current;
            }
        } catch (MalformedURLException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        }
        returnObj = new JSONObject(jsonString);
    return returnObj;
    }
}
