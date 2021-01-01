import mongoengine as me

class Reading(me.EmbeddedDocument):
    """
    Captures a single spectrometer reading. Raw pixel values are stored
    in values field. Optional `notes` for the reading can be recorded.
    """
    set_id = me.StringField(max_length=20, required=True)
    timestamp = me.DateField(required=True)
    values = me.ListField(required=True)
    notes = me.StringField(max_length=200, required=False)
    
    meta = {'ordering': ['-timestamp']}

class ReadingSet(me.Document):
    """
    Capture a set of readings (usually of same sample) that are linked via 
    the `id` (PK) - `set_id` (FK) fields. `device_id` is necessary to record
    which group member the capture device (spectrometer) belongs to.
    """
    id = me.StringField(max_length=20, required=True, unique=True)
    timestamp = me.DateField(required=True)
    readings = me.EmbeddedDocumentListField(Reading)
    sample_name = me.StringField(max_length=80, required=True)
    sample_descr = me.StringField(max_length=200, required=False)
    device_id = me.StringField(max_length=50, required=True)
    params = me.DictField(required=True) # pertinent device params
    
    meta = {'allow_inheritance': True} # more specific reading sets can inherit from this class