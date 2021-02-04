from flask import Flask, request, send_from_directory
from flask_cors import CORS, cross_origin
from lib.db.transactions import connect_db, save_reading_set, get_reading_sets
from lib.util import write_json
import os

app = Flask(__name__)
CORS(app, resources={r"/*": {"origins": "*"}})  # Enable cross-origin resource sharing for AJAX calls
app.config["CORS_SUPPORTS_CREDENTIALS"]=True

@app.route('/')
def index():
    return 'Spectre API: live'

@app.route('/save_reading_set', methods=["POST"])
def api_save_reading_set():
    data = request.get_json(force=True)
    return save_reading_set(data)

@app.route('/data/', methods=["GET"])
def api_get_reading_sets():
    args = {
        "id": request.args.get("id"),
        "start_ts": request.args.get("start_ts"),
        "end_ts": request.args.get("end_ts")
    }
    return get_reading_sets(**args)

@app.route("/data/file")
def get_file():
    """Download a json file of data"""
    data = api_get_reading_sets()
    writepath = os.path.join(app.root_path, 'static', 'spectre_data.json')
    readpath = os.path.join(app.root_path, 'static')

    write_json(data, writepath, archive_if_exists=False)
    return send_from_directory(readpath, 'spectre_data.json', as_attachment=True)

if __name__ == '__main__':
    connect_db()
    app.run(debug=True)