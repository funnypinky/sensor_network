package com.funnypinky.server.Service.mqtt;

import lombok.Getter;
import lombok.Setter;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.boot.context.properties.EnableConfigurationProperties;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.stereotype.Component;

@Component
@Configuration
@EnableConfigurationProperties
@ConfigurationProperties(prefix = "mqtt")
@Setter
@Getter
public class Mqtt {

    @Autowired
    private MqttPushClient mqttPushClient;
    /**
     * User name
     */
    // @Value("username")
    private String username = "broker";
    /**
     * Password
     */
    private String password = "Rechner123";
    /**
     * Connection address
     */
    private String hostUrl ="tcp://10.72.72.124:1883";
    //private String hostUrl ="tcp://127.0.0.1:1883";
    //private String hostUrl ="tcp://mqtt:1883";
    /**
     * Customer Id
     */
    private String clientID = "WeatherServer";
    /**
     * Default connection topic
     */
    private String defaultTopic = "#";
    /**
     * Timeout time
     */
    private int timeout = 30;
    /**
     * Keep connected
     */
    private int keepalive = 100;

    @Bean
    public MqttPushClient getMqttPushClient() {
        System.out.println("hostUrl: "+ hostUrl);
        System.out.println("clientID: "+ clientID);
        System.out.println("username: "+ username);
        System.out.println("password: "+ password);
        System.out.println("timeout: "+timeout);
        System.out.println("keepalive: "+ keepalive);
        mqttPushClient.connect(hostUrl, clientID, username, password, timeout, keepalive);
        // End with / / to subscribe to all topics starting with test
        mqttPushClient.subscribe(defaultTopic, 0);
        return mqttPushClient;
    }
}