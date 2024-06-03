# This is yaml testing
# Paho MQTT library for MQTT communication
import paho.mqtt.client as mqtt
# JSON library for working with JSON data
import json
# PyODBC library for connection to databases
import pyodbc
from sorcery import dict_of

import random
import string

cnxn = None
cursor = None
new_code = None


def connect_to_db():
    global cnxn, cursor
    cnxn = pyodbc.connect("DRIVER={ODBC Driver 17 for SQL Server};"
                          "SERVER=lifewatch.database.windows.net;"
                          "DATABASE=lifedb;"
                          "UID=fiveguys;"
                          "PWD=t[zn)l_R")
    cursor = cnxn.cursor()


def generate_code(length):
    letters = string.ascii_letters
    connect_to_db()
    code = ''.join(random.choice(letters) for i in range(length))
    cursor.execute('SELECT Code FROM Devices WHERE Code = ?', code)
    result = cursor.fetchone()

    while result is not None:
        code = ''.join(random.choice(letters) for i in range(length))
        cursor.execute('SELECT Code FROM Devices WHERE Code = ?', code)
        result = cursor.fetchone()

    return code


def insert_code(code):
    cursor.execute('INSERT INTO Devices (Code,Battery_Level,Status) VALUES (?,?,?)', code, 0, "Normal")
    cursor.execute('INSERT INTO Location (Code,Longitude,Latitude) VALUES (?,?,?)', code, 0, 0)
    print("Inserted new device!")
    cnxn.commit()


def update_location(Longitude, Latitude, Code):
    connect_to_db()
    cursor.execute('''
            UPDATE Location 
            SET Longitude = ?, Latitude = ?
            WHERE Code = ?
        ''', Longitude, Latitude, Code)
    cnxn.commit()
    print("Location Updated!")


def update_details(Battery, Status, Code):
    connect_to_db()
    cursor.execute('''
               UPDATE Devices 
               SET Battery_Level = ?, Status = ?
               WHERE Code = ?
           ''', Battery, Status, Code)
    cnxn.commit()
    print("Details Updated!")


def send_details(msg):
    message = msg.payload.decode("utf-8")
    data = json.loads(message)
    sessionID = data['sessionID']
    code = data['code']
    req = data.get('req')
    battery = data.get('battery')

    connect_to_db()
    cursor.execute('SELECT Battery_Level, Status FROM Devices WHERE Code = ?', code)
    result = cursor.fetchone()

    if result:
        battery = result[0]
        status = result[1]
    else:
        battery = 0
        status = "NULL"

    if req == "Request":
        req = "Response"
        details = dict_of(sessionID, code, status, battery, req)
        json_object = json.dumps(details)
        client.publish("App/Init", json_object)
        print("Details sent!")


def send_location(msg):
    message = msg.payload.decode("utf-8")
    data = json.loads(message)
    code = data['code']
    coords = data.get('LocationData')
    req = data['req']

    connect_to_db()
    cursor.execute('SELECT Longitude, Latitude FROM Location WHERE Code = ?', code)
    result = cursor.fetchone()

    if result and coords is None:
        req = "Response"
        Long = result[0]
        Lat = result[1]
        LocationData = dict_of(Long, Lat)
        loc = dict_of(code,LocationData,req)
        json_object = json.dumps(loc)
        client.publish("Chip/Message", json_object)
        print("Location sent!")


def add_location(msg):
    message = msg.payload.decode("utf-8")
    data = json.loads(message)
    code = data['code']
    battery = data['battery']
    status = data['status']
    location = data['LocationData']
    longitude = location['Long']
    latitude = location['Lat']

    update_location(longitude, latitude, code)
    update_details(battery, status, code)


def add_device(msg):
    global new_code
    message = msg.payload.decode("utf-8")
    if message == "New Device":
        new_code = generate_code(5)
        print("Generated new code!")
        client.publish("Chip/Init", new_code)
    if message == "ACK":
        print("Acknowledged, inserting new device...")
        insert_code(new_code)


# Function to handle MQTT payload data
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))
    topic = msg.topic
    match topic:
        case "Chip/Init":
            add_device(msg)
        case "Chip/Message":
            message = msg.payload.decode("utf-8")
            data = json.loads(message)
            req = data['req']
            if req == "Request":
                send_location(msg)
            elif req == "Chip":
                add_location(msg)
        case "App/Init":
            send_details(msg)


# Function to subscribe to topics in MQTT broker
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("Chip/Init")
    client.subscribe("Chip/Message")
    client.subscribe("App/Init")


# MQTT client setup and connection
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Set username, password, and connect to MQTT broker
client.username_pw_set("", "")
client.connect("80.115.229.72", 1883, 60)

# Start the infinite loop
client.loop_forever()
