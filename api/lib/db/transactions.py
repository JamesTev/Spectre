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
    exp_keys = ["timestamp", "readings", "device_id", "sample_name", "sample_descr", "params"]
    if not all([k in req_data for k in exp_keys]):
        return 400, "Missing keys in request data"

    set_ref = str(int(time.time()))
    readings = []
    for r in req_data["readings"]:
        ts = datetime.fromtimestamp(r["timestamp"]/1000)
        readings.append(Reading(set_ref=set_ref, timestamp=ts, values=r["values"]))
    
    set_data = {
        "ref": set_ref,
        "timestamp": datetime.fromtimestamp(time.time()),
        "device_id": req_data["device_id"],
        "readings": readings,
        "sample_name": req_data["sample_name"],
        "sample_descr": req_data["sample_descr"],
        "params": req_data["params"]
    }
    
    ReadingSet(**set_data).save()
    return {"error": False, "message": "save successful"}
    
def get_reading_sets(id=None, start_ts=None, end_ts=None):
    if id is not None:
        result = ReadingSet.objects(id=id)
#     elif not any([start_ts, end_ts]):
#         if start_ts is not None:
#             result = ReadingSet.objects()
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
            if k == 'readings':
                for reading in v:
                    reading['timestamp'] = reading['timestamp'].isoformat()
        out.append(tmp)
    return out