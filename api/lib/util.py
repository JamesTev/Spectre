import pandas as pd 
import pickle
import os
import json

def write_pkl(data, path, archive_if_exists=True):
    if archive_if_exists and os.path.exists(path):
        old_data = read_pkl(path)
        idx = path.rfind('.') # get everything before file extension
        arch_path = path[:idx]+'_archived'+path[idx:]
        write_pkl(old_data, arch_path, archive_if_exists=False) # archive old file

    with open(path, 'wb') as handle:
        return pickle.dump(data, handle, protocol=pickle.HIGHEST_PROTOCOL)

def read_pkl(path):
    with open(path, 'rb') as handle:
        return pickle.load(handle)


def write_json(data, path, archive_if_exists=True):
    if archive_if_exists and os.path.exists(path):
        old_data = read_json(path)
        idx = path.rfind('.') # get everything before file extension
        arch_path = path[:idx]+'_archived'+path[idx:]
        write_json(old_data, arch_path, archive_if_exists=False) # archive old file

    with open(path, 'w') as f:
        json.dump(data, f, indent=4, ensure_ascii=False)

def read_json(path):
    with open(path) as f:
        return json.load(f)
    
