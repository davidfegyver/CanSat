import os 

# replace every occurence of 'sensor_t' with 'adafruit_sensor_t' in .pio\libdeps\esp32cam

path = '.pio\libdeps\esp32cam'

print('Renaming sensor_t to adafruit_sensor_t in libraries')

for root, dirs, files in os.walk(path):
    for file in files:
        if file.endswith('.h') or file.endswith('.cpp'):
            with open(os.path.join(root, file), 'r') as f:
                content = f.read()
            content = content.replace('sensor_t', 'adafruit_sensor_t')
            with open(os.path.join(root, file), 'w') as f:
                f.write(content)

print('Done!')