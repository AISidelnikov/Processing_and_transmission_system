# python3.6

import random, json, matplotlib.pyplot as plt
import numpy as np
from paho.mqtt import client as mqtt_client
from datetime import datetime

import numpy as np
from scipy.signal import find_peaks


fig = plt.figure()
ax = fig.add_subplot(1,1,1)

def serch_str(file_path, word):
    result = []
    with open(file_path, 'r') as file:
        for l_on, line in enumerate(file):
            if word in line:
                text = line.split()
                result.append(text[1])
    return result

def scrin(file):
    time =  serch_str(file, 'TIME')
    turn =  serch_str(file, 'TURN')
    turn = [int(line) for line in turn]
    #fig = plt.figure()
    ax = fig.add_subplot()
    
    ax.clear()
    plt.xticks(rotation=90)
    plt.xlabel('Time')
    plt.ylabel('Turn')
    plt.title('Grafic')
    ax.plot(time, turn)
    plt.show()



broker = '192.168.3.8'
port = 1883
topic = "test"
# generate client ID with pub prefix randomly
client_id = f'python-mqtt-{random.randint(0, 100)}'
username = 'bob'
password = '2222'


def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def subscribe(client: mqtt_client):
    now = datetime.now()
    name_file = 'data ' + now.strftime("%H.%M.%S") + '.txt'

    def on_message(client, userdata, msg):
        file = open(name_file, 'a')
        if msg.payload.decode() == "START":
            None
        elif msg.payload.decode() == "STOP":
            client.on_disconnect
            file.close
            scrin(name_file)
        else:
            m_decode=str(msg.payload.decode())
            m_in = json.loads(m_decode)

            print(f"TIME {m_in['HOURS']}:{m_in['MINUTES']}:{m_in['SECONDS']}:{m_in['MILES SECONDS']}:{m_in['MICRO SECONDS']}")
            print(f"TURN {m_in['TURN']}")
            print(f"SLANT {m_in['SLANT']}")
            print(f"OFFSET {m_in['OFFSET']}")
            print(f"OSCILLATION {m_in['OSCILLATION']}")

            file.write(f"TIME {m_in['HOURS']}:{m_in['MINUTES']}:{m_in['SECONDS']}:{m_in['MILES SECONDS']}:{m_in['MICRO SECONDS']}\n")
            file.write(f"TURN {m_in['TURN']}\n")
            file.write(f"SLANT {m_in['SLANT']}\n")
            file.write(f"OFFSET {m_in['OFFSET']}\n")
            file.write(f"OSCILLATION {m_in['OSCILLATION']}\n\n")

    client.subscribe(topic)
    client.on_message = on_message


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == '__main__':
    run()