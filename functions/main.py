import base64
from google.cloud import storage

def sensor_pubsub(event, context):
    """Triggered from a message on a Cloud Pub/Sub topic.
    Args:
         event (dict): Event payload.
         context (google.cloud.functions.Context): Metadata for the event.
    """
    pubsub_message = base64.b64decode(event['data']).decode('utf-8')
    print(pubsub_message)
    client = storage.Client()
    bucket = client.get_bucket('sensor-hyt')
    # Then do other things...
    blob = bucket.get_blob('data.txt')
    data = blob.download_as_bytes()
    blob.upload_from_string(str(data, 'UTF-8') + "\n" + pubsub_message)
    
