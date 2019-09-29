import time, requests
import DAN

ServerURL = 'http://iot.iottalk.tw:9999'      #with non-secure connection
#ServerURL = 'https://DomainName' #with SSL connection
Reg_addr = None #if None, Reg_addr = MAC address
DAN.profile['d_name']='PlantboxHL'

DAN.profile['dm_name']='HiddenLayer'
DAN.profile['df_list']=[]
Cell_num = 30

def getCellIDF_name(num):
    return 'Cell'+'{:02d}'.format(int(num))+'-I'

def getCellODF_name(num):
    return 'Cell'+'{:02d}'.format(int(num))+'-O'

for i in range(Cell_num):
    DAN.profile['df_list'].append(getCellIDF_name(i+1))
    DAN.profile['df_list'].append(getCellODF_name(i+1))
#DAN.profile['d_name']= 'Assign a Device Name' 

DAN.device_registration_with_retry(ServerURL, Reg_addr)
#DAN.deregister()  #if you want to deregister this device, uncomment this line
#exit()            #if you want to deregister this device, uncomment this line

#print(DAN.profile['df_list'])


while 1:
    try:
        for i in range(Cell_num):
            data = DAN.pull(getCellODF_name(i+1))
            if data != None:
                DAN.push(getCellIDF_name(i+1), data[0])

    except Exception as e:
        print(e)
        if str(e).find('mac_addr not found:') != -1:
            print('Reg_addr is not found. Try to re-register...')
            DAN.device_registration_with_retry(ServerURL, Reg_addr)
        else:
            print('Connection failed due to unknow reasons.')
            time.sleep(1)    

    time.sleep(0.2)

