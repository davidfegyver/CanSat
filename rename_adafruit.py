import os 
import re

path = '.pio\libdeps\esp32cam'

print('Renaming sensor_t to adafruit_sensor_t')

for root, dirs, files in os.walk(path):
    for file in files:
        if file.endswith('.h') or file.endswith('.cpp'):
            with open(os.path.join(root, file), 'r') as f:
                content = f.read()

            content = re.sub(r'\bsensor_t\b', 'adafruit_sensor_t', content)

            with open(os.path.join(root, file), 'w') as f:
                f.write(content)

print('Done!')