import mongoengine as me
from .models import ReadingSet, Reading
import json
from datetime import datetime
from dotenv import load_dotenv
import time
import os

load_dotenv()

def connect_db(user=None, pw=None):
    me.connect(db='spectre_db', host=os.getenv("DB_URI"))

def save_reading_set(req_data):
    """
    Creates and saves a reading set from raw data. `data` should be a 2D list containing 1 or more readings.
    """
    from .models import ReadingSet
    exp_keys = ['ref', 'timestamp', 'readings', 'calibration_readings', 'sample_name', 'sample_descr', 'adulterant_type',
                 'api_type', 'solvent', 'adulterant_mass', 'api_mass', 'solvent_vol', 'device_id', 'params']

    set_ref = str(int(time.time()))
    readings = []
    
    def proc_readings(reading_data):
        readings = []
        for r in reading_data:
            ts = datetime.fromtimestamp(r["timestamp"]/1000)
            readings.append(Reading(set_ref=set_ref, timestamp=ts, values=r["values"]))
        return readings
    
    set_data = {
        "ref": set_ref,
        "timestamp": datetime.fromtimestamp(time.time()),
        "readings": proc_readings(req_data["readings"]),
        "calibration_readings": proc_readings(req_data["calibration_readings"]),
    }
    for k in exp_keys:
        if not k in set_data:
            if not k in req_data:
                print(k)
                return "Missing keys in request", 400
            set_data[k] = req_data[k]
    try:
        ReadingSet(**set_data).save()
    except Exception as e:
        return {"error": True, "message": str(e)}, 500
    
    return {"error": False, "message": "save successful"}, 200
    
def get_reading_sets(id=None, start_ts=None, end_ts=None, head=None, tail=None):
    
    if id is not None:
        result = ReadingSet.objects(id=id)
#     elif not any([start_ts, end_ts]):
#         if start_ts is not None:
#             result = ReadingSet.objects()
    else:
        if head is not None:
            result = ReadingSet.objects[:int(head)]
        elif tail is not None:
            result = ReadingSet.objects().order_by('-_id').limit(int(tail))
        else:
            result = ReadingSet.objects()
    result = process_objects(result)
    if len(result) == 0:
        return {}
    
    return json.dumps(result)
        
def process_objects(queryset):
    out = []    
    q = queryset.as_pymongo()
    for obj in q:
        tmp = {}
        for k,v in obj.items():
            if not k.startswith('_'):
                if isinstance(v, datetime):
                    v = v.isoformat()
                tmp[k] = v
            if k == 'readings' or k == 'calibration_readings':
                for reading in v:
                    reading['timestamp'] = reading['timestamp'].isoformat()
        out.append(tmp)
    return out