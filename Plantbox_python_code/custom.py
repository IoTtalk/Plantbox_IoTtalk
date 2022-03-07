import DAN

ServerIP='http://iot.iottalk.tw:9999' #P=None:AutoSearch, or ='IP':Connect to this IP

def profile_init():
    DAN.profile['dm_name']='Plantbox'
    DAN.profile['d_name']='Plantbox_311'

def odf():  # int only
    return [
	('Color-O', 0, 'LED-R-O'),
	('Color-O', 1, 'LED-G-O'),
	('Color-O', 2, 'LED-B-O'),
	('AddWater-O', 0, 'AddWater-O'),
	('PumpWater-O', 0, 'PumpWater-O'),
	('Spray-O', 0, 'Spray-O'),
    ]

def idf():
    return [
       ('CO2-I', float),
       ('Humidity-I', float),
       ('O2-I', float),
       ('Temperature-I', float),
       ('WaterLevel-I', float),
    ]
