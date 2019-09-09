import sys
import time
import DAN
import requests

sys.path.insert(0, '/usr/lib/python2.7/bridge/')
from bridgeclient import BridgeClient
import custom

custom.profile_init()
odf_list = custom.odf()
idf_list = custom.idf()

DAN.profile['df_list'] = [t[0] for t in idf_list]

for t in odf_list:
    if t[0] not in DAN.profile['df_list']:
        DAN.profile['df_list'].append(t[0])

print('Detected features:')
for f_name in DAN.profile['df_list']:
    print('    {}'.format(f_name))

DAN.device_registration_with_retry(custom.ServerIP)

BridgeClient().put('Reg_done', '1')

incomming = {}
previous_outcomming = {}
for f_name in [t[0] for t in odf_list]:
    incomming[f_name] = 0

for f_name in [t[0] for t in idf_list]:
    previous_outcomming[f_name] = ''

while True:
    try:
        cache = {}
	check_list=[t[0] for t in odf_list]
	for f_name, index, pin_name in odf_list:

            if f_name not in cache.keys():
  	        PIN = DAN.pull(f_name)
		cache[f_name] = PIN
            else:
	        PIN = cache[f_name]
	
            if PIN != None:
                BridgeClient().put(pin_name, str(int(PIN[index])))
#                BridgeClient().put(pin_name, str(PIN[index]))

                check_list.remove(f_name)
                if f_name not in check_list:
                    incomming[f_name] = incomming[f_name] ^ 1
                    BridgeClient().put('incomming_'+f_name, str(incomming[f_name]))
                    print ('Bridge: change incomming state of '+f_name)
							    
                print '{f}[{d}] -> {p} = {v}, incomming[{f}] = {i}'.format(
                        f=f_name,
                        d=index,
                        p=pin_name,
                        v=str(int(PIN[index])),
#                        v=str(PIN[index]),
                        i=incomming[f_name],
                )


        for f_name, type_ in idf_list:
            outcomming = BridgeClient().get('outcomming_'+f_name)
            if (outcomming != previous_outcomming[f_name]) or True:
                previous_outcomming[f_name] = outcomming
                tmp = BridgeClient().get(f_name)
                if tmp is None:
                    continue            

                v = type_(tmp)
                if v is not None:
                    print 'DAN.push({f}, {v!r})'.format(
                        f=f_name, v=v,
                    )
                    DAN.push(f_name, v)
                

    except requests.exceptions.ConnectionError:
        print("requests.exceptions.ConnectionError")
        BridgeClient().put('Reg_done', '0')
        DAN.device_registration_with_retry()
        BridgeClient().put('Reg_done', '1')

    time.sleep(0.2)
