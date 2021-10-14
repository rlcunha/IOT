import paho.mqtt.client as mqtt

mqttc = mqtt.Client()
mqttc.connect("mqtt.eclipseprojects.io", 1883, 60)
mqttc.publish("labrmerces/temperatura", "25")
mqttc.publish("labrmerces/lampada", "0")
mqttc.publish("labrmerces/umidade", "12")
mqttc.loop(2)